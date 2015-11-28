#ifndef VSX_TEXTURE_LOAD_THREAD_INFO_H
#define VSX_TEXTURE_LOAD_THREAD_INFO_H

#include <vsxfst.h>
#include "vsx_texture.h"
#include "vsx_texture_data_loader_thread.h"

class vsx_texture_loader_thread_info
{
public:
  void* thread = 0x0;
  vsxf* filesystem = 0x0;
  vsx_texture_data* texture_data = 0x0;
  vsx_string<> filename;
  vsx_texture_data_hint hint;
};

#endif
