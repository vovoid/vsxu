#include <filesystem/archive/vsx_filesystem_archive_vsxz_writer.h>
#include <filesystem/vsx_filesystem_helper.h>
#include <vsx_compression_lzma.h>
#include <vsx_compression_lzham.h>
#include <tools/vsx_thread_pool.h>
#include <filesystem/archive/vsx_filesystem_archive_vsxz_header.h>
#include <filesystem/vsx_filesystem_identifier.h>

namespace vsx
{
void filesystem_archive_vsxz_writer::create(const char* filename)
{
  archive_filename = filename;
}

void filesystem_archive_vsxz_writer::add_file
(
  vsx_string<> filename,
  vsx_string<> disk_filename,
  bool deferred_multithreaded
)
{
  VSX_UNUSED(deferred_multithreaded);
  filesystem_archive_file_write file_info;
  file_info.operation = filesystem_archive_file_write::operation_add;
  file_info.filename = filename;
  file_info.source_filename = disk_filename;
  archive_files.move_back(std::move(file_info));
}


void filesystem_archive_vsxz_writer::add_string(vsx_string<> filename, vsx_string<> payload, bool deferred_multithreaded)
{
  filesystem_archive_file_write file_info;
  file_info.filename = filename;
  req(payload.size());
  file_info.data.allocate( payload.size() - 1 );
  file_info.operation = filesystem_archive_file_write::operation_add;

  foreach (payload, i)
    file_info.data[i] = (unsigned char)payload[i];

  archive_files.move_back(std::move(file_info));
}


void filesystem_archive_vsxz_writer::archive_files_saturate_all()
{
  foreach (archive_files, i)
  {
    filesystem_archive_file_write &archive_file = archive_files[i];
    req_continue(archive_file.operation == filesystem_archive_file_write::operation_add);
    archive_file.data = filesystem_helper::file_read(archive_file.source_filename);
  }
}

void filesystem_archive_vsxz_writer::close()
{
  // Read all files from disk
  archive_files_saturate_all();

  // 1. add files to compression chunks
  filesystem_archive_chunk_write chunk_uncompressed;
  filesystem_archive_chunk_write chunk_text_files;
  filesystem_archive_chunk_write chunk_others[6];
  size_t chunk_other_iterator = 0;

  foreach (archive_files, i)
  {
    // peek data, handle text files
    if (filesystem_identifier::is_text_file(archive_files[i].data))
    {
      chunk_text_files.add_file( &archive_files[i] );
      continue;
    }

    // if file is large, try to compress it, if it is pointless, store in uncompressed chunk
    if (archive_files[i].data.size() > 8192)
    {
      vsx_ma_vector<unsigned char> lzma_compressed = vsx::compression_lzma::compress( archive_files[i].data );
      float lzma_ratio = (float)(lzma_compressed.size()) / (float)(archive_files[i].data.size());

      vsx_ma_vector<unsigned char> lzham_compressed = vsx::compression_lzham::compress( archive_files[i].data );
      float lzham_ratio = (float)(lzham_compressed.size()) / (float)(archive_files[i].data.size());

      if ( lzma_ratio > 0.8 && lzham_ratio > 0.8)
      {
        chunk_uncompressed.add_file( &archive_files[i] );
        continue;
      }
    }

    chunk_others[chunk_other_iterator].add_file( &archive_files[i] );

    // when chunks reach 1 MB, start rotating
    if (chunk_others[chunk_other_iterator].uncompressed_data.size() > 1024*1024)
    {
      chunk_other_iterator++;
      if (chunk_other_iterator == 6)
        chunk_other_iterator = 0;
    }
  }



  // 2. compress chunks
  // 3. write archive to disk


  FILE* file = fopen(archive_filename.c_str(),"wb");

  vsxz_header header;
  header.file_count = archive_files.size();


  foreach (archive_files, i)
    if (archive_files[i].compression_type != filesystem_archive_file_write::compression_none)
    {
      header.compressed_uncompressed_data_size += archive_files[i].data.size();
      header.file_count_compressed++;
    }

  foreach (archive_files, i)
    header.file_info_table_size +=
        sizeof(vsxz_header_file_info) +
        archive_files[i].filename.size()
      ;

  fwrite(&header, sizeof(vsxz_header), 1, file);

  foreach (archive_files, i)
  {
    vsxz_header_file_info file_header;
    file_header.compression_type = (uint8_t)archive_files[i].compression_type;
    file_header.filename_size = archive_files[i].filename.size();
    file_header.compressed_size = archive_files[i].compressed_data.get_sizeof();
    file_header.uncompressed_size = archive_files[i].data.get_sizeof();
    fwrite(&file_header, sizeof(vsxz_header_file_info), 1, file);
    fwrite(archive_files[i].filename.get_pointer(), sizeof(char), archive_files[i].filename.size(), file);
  }

  foreach (archive_files, i)
    fwrite(
      archive_files[i].compressed_data.get_pointer(),
      archive_files[i].compressed_data.get_sizeof(),
      1,
      file
    );

  fclose(file);
}

}

