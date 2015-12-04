#ifndef VSX_BITMAP_LOADER_BASE_H
#define VSX_BITMAP_LOADER_BASE_H

#include "vsx_bitmap_loader_thread.h"
#include "vsx_bitmap_loader_thread_info.h"

#include <vsxfst.h>
#include <tools/vsx_req.h>

class vsx_bitmap_loader_base
{
protected:

  virtual void load_internal(vsx_string<> filename, vsxf* filesystem, vsx_bitmap* bitmap, bool thread, vsx_texture_loader_thread_info* thread_info) = 0;

public:

  void load(vsx_bitmap* bitmap, vsx_string<>filename, vsxf* filesystem, bool thread, uint64_t hint)
  {
    vsx_texture_loader_thread_info* thread_info = new vsx_texture_loader_thread_info();
    thread_info->filename = filename;
    thread_info->filesystem = filesystem;
    thread_info->bitmap = bitmap;
    thread_info->hint = hint;

    bitmap->data_ready = 0;
    load_internal(filename, filesystem, bitmap, thread, thread_info);
  }
};

#endif
