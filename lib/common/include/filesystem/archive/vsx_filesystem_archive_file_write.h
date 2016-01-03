#pragma once

#include <container/vsx_ma_vector.h>
#include <string/vsx_string.h>

namespace vsx
{
  class filesystem_archive_file_write
  {
  public:
    vsx_string<> filename;
    vsx_string<> source_filename;

    // for deferred adding of a file
    enum operation_t{
      operation_none,
      operation_add
    };

    operation_t operation = operation_none;

    enum compression_type_t{
      compression_none = 0,
      compression_lzma = 1,
      compression_lzham = 2
    };

    compression_type_t compression_type = compression_none;

    vsx_ma_vector<unsigned char> compressed_data;
    vsx_ma_vector<unsigned char> uncompressed_data;
  };
}

