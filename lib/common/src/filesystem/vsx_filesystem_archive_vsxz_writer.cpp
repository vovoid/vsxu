#include <filesystem/archive/vsx_filesystem_archive_vsxz_writer.h>
#include <filesystem/vsx_filesystem_helper.h>
#include <vsx_compression_lzma.h>
#include <tools/vsx_thread_pool.h>
#include <filesystem/archive/vsx_filesystem_archive_vsxz_header.h>

namespace vsx
{
void filesystem_archive_vsxz_writer::create(const char* filename)
{
  archive_filename = filename;
}


void filesystem_archive_vsxz_writer::file_add_all_worker(vsx_nw_vector<filesystem_archive_file_write*>* work_list)
{
  for (size_t i = 0; i < work_list->size(); i++)
  {
    filesystem_archive_file_write* info = (*work_list)[i];
    vsx_string<> filename = info->filename;

    info->uncompressed_data = filesystem_helper::file_read(filename);
    info->compressed_data = compression_lzma::compress( info->uncompressed_data );
    info->compression_type = filesystem_archive_file_write::compression_lzma;

    // when compression is pointless
    if (info->compressed_data.size() > info->uncompressed_data.size())
    {
      info->compressed_data = info->uncompressed_data;
      info->compression_type = filesystem_archive_file_write::compression_none;
    }
  }
}

void filesystem_archive_vsxz_writer::file_add_all()
{
  size_t pooled_size = 0;
  vsx_nw_vector<filesystem_archive_file_write*>* pool = new vsx_nw_vector<filesystem_archive_file_write*>();
  size_t files_to_process = 0;
  foreach (archive_files, i)
  {
    filesystem_archive_file_write &archive_file = archive_files[i];
    req_continue(archive_file.uncompressed_data.size());
    req_continue(archive_file.operation == filesystem_archive_file_write::operation_add);

    files_to_process++;
    bool is_last = i == (archive_files.size() - 1);

    pool->push_back(&archive_file);
    pooled_size += archive_file.uncompressed_data.size();

    if (pooled_size <= work_chunk_size && !is_last)
      continue;

    vsx_thread_pool::instance()->add(
      [=](vsx_nw_vector<filesystem_archive_file_write*>* work_pool)
      {
        file_add_all_worker(work_pool);
        delete work_pool;
      },
      pool
    );

    if (!is_last)
      pool = new vsx_nw_vector<filesystem_archive_file_write*>();

    pooled_size = 0;
  }

  vsx_thread_pool::instance()->add(
    [=](vsx_nw_vector<filesystem_archive_file_write*>* work_pool)
    {
      file_add_all_worker(work_pool);
      delete work_pool;
    },
    pool
  );

  req(files_to_process);
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
  file_info.filename = filename;
  file_info.source_filename = disk_filename;
  archive_files.move_back(std::move(file_info));
}


void filesystem_archive_vsxz_writer::add_string(vsx_string<> filename, vsx_string<> payload, bool deferred_multithreaded)
{
  filesystem_archive_file_write file_info;
  file_info.filename = filename;
  file_info.uncompressed_data.allocate( payload.size() );
  file_info.operation = filesystem_archive_file_write::operation_add;

  foreach (payload, i)
    file_info.uncompressed_data[i] = (unsigned char)payload[i];

  archive_files.move_back(std::move(file_info));
}


void filesystem_archive_vsxz_writer::close()
{
  FILE* file = fopen(archive_filename.c_str(),"wb");

  vsxz_header header;
  header.file_count = archive_files.size();

  foreach (archive_files, i)
    header.file_info_table_size +=
        sizeof(vsxz_header_file_info) +
        archive_files[i].filename.size()
      ;

  foreach (archive_files, i)
    header.data_size += archive_files[i].compressed_data.get_sizeof();

  fwrite(&header, sizeof(vsxz_header), 1, file);

  foreach (archive_files, i)
  {
    vsxz_header_file_info file_header;
    file_header.compression_type = (uint8_t)archive_files[i].compression_type;
    file_header.compressed_size = archive_files[i].compressed_data.get_sizeof();
    file_header.uncompressed_size = archive_files[i].uncompressed_data.get_sizeof();
    fwrite(&file_header, sizeof(vsxz_header_file_info), 1, file);
  }

  foreach (archive_files, i)
    fwrite(archive_files[i].compressed_data.get_pointer(), archive_files[i].compressed_data.get_sizeof(), 1, file);

  fclose(file);
}

}

