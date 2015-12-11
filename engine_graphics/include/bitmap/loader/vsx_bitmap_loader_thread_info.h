#pragma once

#include <vsxfst.h>
#include "vsx_bitmap_loader_thread.h"

class vsx_texture_loader_thread_info
{
public:
  void* thread = 0x0;
  vsxf* filesystem = 0x0;
  vsx_bitmap* bitmap = 0x0;
  vsx_string<> filename;
};
