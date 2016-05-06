#pragma once

#include <inttypes.h>
#include <container/vsx_ma_vector.h>

namespace vsx
{
  class file
  {
  public:
    vsx_string<> filename;

    union
    {
      FILE* handle = 0x0;
      size_t position;  // position in the data stream
    };
    size_t size;  // size of the data stream in bytes


    vsx_ma_vector<unsigned char> data;
  };

}

