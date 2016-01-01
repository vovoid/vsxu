#pragma once


#include <vsx_platform.h>
#include <inttypes.h>

namespace vsx
{
  class file_mmap
  {
  public:
    vsx_string<> filename;

    unsigned char* data = 0x0;
    size_t size = 0;

    #if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
      int handle = 0x0;
    #endif

    #if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
      HANDLE handle = 0x0;
      HANDLE map_handle = 0x0;
    #endif
  };

}

