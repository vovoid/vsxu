#pragma once

#include <inttypes.h>

namespace vsx
{
  class file
  {
  public:
    vsx_string<> filename;

    size_t position = 0;  // position in the data stream
    size_t size = 0;  // size of the data stream in bytes

    enum mode_t {
      mode_none = 0,
      mode_read = 1,
      mode_write = 2
    };

    mode_t mode = mode_none;

    // in case of archive or mmap, this is used
    unsigned char* file_data = 0x0;

    bool file_data_volatile = false;

    FILE* handle = 0x0;

    ~file()
    {
      req(file_data);

      if (mode == mode_write)
      {
        delete (vsx_nw_vector<char>*)file_data;
        return;
      }

      if (file_data_volatile)
        return;

      free(file_data);
    }
  };

}

