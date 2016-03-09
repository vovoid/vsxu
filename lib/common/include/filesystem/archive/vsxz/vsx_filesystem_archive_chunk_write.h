#pragma once

#include <container/vsx_ma_vector.h>
#include "vsx_filesystem_archive_vsxz_header.h"
#include <filesystem/archive/vsx_filesystem_archive_file_write.h>
#include <filesystem/tree/vsx_filesystem_tree_writer.h>
#include <tools/perf/vsx_perf.h>

namespace vsx
{

class filesystem_archive_chunk_write
{
public:

  vsx_ma_vector<unsigned char> uncompressed_data;
  vsx_ma_vector<unsigned char> compressed_data;
  vsx_ma_vector< vsxz_header_file_info > file_info_table;
  vsx_ma_vector< filesystem_archive_file_write* > archive_file_write;


  enum compression_type_t{
    compression_none = 0,
    compression_lzma = 1,
    compression_lzham = 2
  };

  compression_type_t compression_type = compression_none;

  bool has_files()
  {
    return file_info_table.size() > 0;
  }


  void add_file(filesystem_archive_file_write* file_info)
  {
    archive_file_write.push_back( file_info );

    vsxz_header_file_info info;
    info.offset = uncompressed_data.size();
    info.size = uncompressed_data.size();

    foreach(file_info->data, i)
      uncompressed_data.push_back(file_info->data[i]);

    file_info_table.move_back( std::move(info));
  }

  bool is_size_above_treshold()
  {
    return uncompressed_data.size() > 1024*1024;
  }

  void set_chunk_id(uint16_t index)
  {
    foreach (file_info_table, i)
      file_info_table[i].chunk = index;
  }

  void add_to_tree(vsx_filesystem_tree_writer& tree, size_t &index)
  {
    foreach (archive_file_write, i) {
      tree.add_file(archive_file_write[i]->filename, index);
      index++;
    }
  }

  uint32_t get_compressed_uncompressed_size()
  {
    if (compression_type == compression_none)
      return 0;

    return uncompressed_data.get_sizeof();
  }

  void compress()
  {
    req(uncompressed_data.size());

    vsx_ma_vector< unsigned char> uncompressed_data_readback;
    vsx_perf perf;
    perf.cpu_instructions_start();

    // LZHAM
    compressed_data = vsx::compression_lzham::compress( uncompressed_data );
    perf.cpu_instructions_begin();
      uncompressed_data_readback = vsx::compression_lzham::uncompress(compressed_data, uncompressed_data.size() );
    long long cpu_instructions_lzham = perf.cpu_instructions_end();

    // LZMA
    compressed_data = vsx::compression_lzma::compress( uncompressed_data );
    perf.cpu_instructions_begin();
      uncompressed_data_readback = vsx::compression_lzma::uncompress(compressed_data, uncompressed_data.size() );
    long long cpu_instructions_lzma = perf.cpu_instructions_end();

    if (cpu_instructions_lzham < cpu_instructions_lzma)
      compressed_data = vsx::compression_lzham::compress( uncompressed_data );

    perf.cpu_instructions_stop();
  }

  void write_file_info_table(FILE* file)
  {
    foreach (file_info_table, i)
      fwrite(&file_info_table[i], sizeof(vsxz_header_file_info), 1, file);
  }

  void write_chunk_info_table(FILE* file)
  {
    if (!has_files())
      return;

    vsxz_header_chunk_info info;
    info.compressed_size = compressed_data.size();
    info.uncompressed_size = uncompressed_data.size();
    info.compression_type = (uint16_t)compression_type;
    fwrite(&info, sizeof(vsxz_header_chunk_info), 1, file);
  }


  void write_data(FILE* file)
  {
    req(compressed_data.size() || uncompressed_data.size());
    if (compressed_data.size())
      fwrite(compressed_data.get_pointer(), 1, compressed_data.get_sizeof(), file);
    else
      fwrite(uncompressed_data.get_pointer(), 1, uncompressed_data.get_sizeof(), file);
  }
};

}
