#include <filesystem/tree/vsx_filesystem_tree_reader.h>
#include <filesystem/archive/vsxz/vsx_filesystem_archive_vsxz_info.h>
#include <filesystem/archive/vsxz/vsx_filesystem_archive_vsxz_header.h>
#include <filesystem/mmap/vsx_filesystem_mmap.h>
#include <string/vsx_string_helper.h>

using namespace vsx;

void filesystem_archive_vsxz_info::get_info(const char* archive_filename, vsx_nw_vector< vsx_string<> >& result)
{
  file_mmap* mmap = filesystem_mmap::create(archive_filename);
  req(mmap);
  req(mmap->size > sizeof(vsxz_header) + sizeof(vsxz_header_file_info));
  vsxz_header* header = (vsxz_header*)mmap->data;

  if (header->identifier[0] != 'V' || header->identifier[1] != 'S' || header->identifier[2] != 'X' || header->identifier[3] != 'Z')
  {
    result.push_back(vsx_string<>("Error, not a VSXZ archive"));
    return;
  }

  vsxz_header_chunk_info* chunk_info_table =
      (vsxz_header_chunk_info*) (
        mmap->data +
        sizeof(vsxz_header) +
        header->tree_size +
        sizeof(vsxz_header_file_info) * header->file_count
      );

  vsx_filesystem_tree_reader tree;
  tree.initialize( mmap->data + sizeof(vsxz_header) );

  vsx_nw_vector< vsx_string<> > filenames;
  vsx_nw_vector< uint32_t > payloads;
  tree.get_filename_payload_list(filenames, payloads);

  vsxz_header_file_info* file_info_table = (vsxz_header_file_info*) (
        mmap->data +
        sizeof(vsxz_header) +
        header->tree_size
      );

  size_t total_uncompressed_size_from_chunks = 0;

  result.push_back( vsx_string<>("Chunk info:\n"));
  for (size_t chunk_i = 0; chunk_i < header->chunk_count; chunk_i++)
  {
    result.push_back( vsx_string<>("    chunk ") + vsx_string_helper::i2s((int)chunk_i) + ": \n");
    result.push_back( vsx_string<>("        compression type:  "+vsx_string_helper::i2s(chunk_info_table[chunk_i].compression_type)+" (1=lzma, 2=lzham)\n"));
    result.push_back( vsx_string<>("        compressed size:   "+vsx_string_helper::f2s(chunk_info_table[chunk_i].compressed_size / (1024.0f*1024)) + " MB \n"));
    result.push_back( vsx_string<>("        uncompressed size: "+vsx_string_helper::f2s(chunk_info_table[chunk_i].uncompressed_size / (1024.0f*1024)) + " MB \n"));

    if (chunk_i)
      total_uncompressed_size_from_chunks += chunk_info_table[chunk_i].uncompressed_size;

    vsx_string<>("        Filenames:\n");
    foreach (filenames, i)
    {
      if (file_info_table[payloads[i] - 1].chunk == chunk_i)
        result.push_back(vsx_string<>("            ") + filenames[i] + "\n");
    }
  }

  for (size_t i = 0; i < header->file_count; i++)
  {
    result.push_back(
          vsx_string<>(" file info table #") + vsx_string_helper::i2s((int)i) + "  - chunk: " +
          vsx_string_helper::i2s(file_info_table[i].chunk) + "  offset: " +
          vsx_string_helper::i2s(file_info_table[i].offset) + "  size: " +
          vsx_string_helper::i2s(file_info_table[i].size) + "\n"
    );
  }

  result.push_back(
        vsx_string<>(" uncompressed size from chunks: ") + vsx_string_helper::i2s((int)total_uncompressed_size_from_chunks) + "\n" +
        vsx_string<>(" uncompressed size from header: ") + vsx_string_helper::i2s(header->compression_uncompressed_memory_size) + "\n"
  );
}
