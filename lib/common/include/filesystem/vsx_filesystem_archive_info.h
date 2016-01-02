#pragma once

#include <container/vsx_ma_vector.h>

namespace vsx
{
  class filesystem_archive_info
  {
  public:
    vsx_string<> filename;

    size_t archive_position = 0;
    enum operation_t{
      operation_none,
      operation_add
    };

    operation_t operation = operation_none;

    vsx_ma_vector<unsigned char> compressed_data;
    vsx_ma_vector<unsigned char> uncompressed_data;
  };
}

