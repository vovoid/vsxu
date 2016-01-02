#pragma once

#include <container/vsx_ma_vector.h>
#include <string/vsx_string.h>

namespace vsx
{
  class filesystem_archive_file_read
  {
  public:
    vsx_string<> filename;
    size_t archive_position = 0;
    vsx_ma_vector<unsigned char> compressed_data;
    vsx_ma_vector<unsigned char> uncompressed_data;
  };
}

