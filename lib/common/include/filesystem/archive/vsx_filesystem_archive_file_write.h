#pragma once

#include <container/vsx_ma_vector.h>
#include <string/vsx_string.h>
#include"vsx_filesystem_archive_vsxz_header.h"
#include <vsx_compression_lzma.h>
#include <vsx_compression_lzham.h>
#include <tools/perf/vsx_perf.h>

namespace vsx
{
  class filesystem_archive_file_write
  {
  public:
    vsx_string<> filename;
    vsx_string<> source_filename;
    vsx_ma_vector<unsigned char> data;


    // for deferred adding of a file
    enum operation_t{
      operation_none,
      operation_add
    };

    operation_t operation = operation_none;
  };

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

    void compress()
    {
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

  };
}

