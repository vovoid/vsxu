#ifndef VSX_TEXTURE_LOAD_THREAD_INFO_H
#define VSX_TEXTURE_LOAD_THREAD_INFO_H

#include <vsxfst.h>
#include "vsx_texture.h"
#include "vsx_texture_data_loader_thread.h"

class vsx_texture_loader_thread_info
{
public:
  vsx_texture_data_loader_thread* thread;
  vsxf* filesystem;
  vsx_texture* texture;
  vsx_string<> filename;
  bool perform_flip_vertical;
  bool perform_split_into_cubemap;
  bool mipmaps;

  vsx_texture_loader_thread_info()
    :
      thread(0x0),
      filesystem(0x0),
      texture(0x0),
      filename(),
      perform_flip_vertical(false),
      perform_split_into_cubemap(false),
      mipmaps(false)
  {
  }
};

#endif
