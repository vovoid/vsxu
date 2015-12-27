#pragma once

#include "vsx_bitmap_loader_base.h"
#include <bitmap/vsx_bitmap_transform.h>
#include <vsx_singleton_counter.h>

class vsx_bitmap_loader_png
    : public vsx_bitmap_loader_base
{

  static void* worker(void *ptr)
  {
    vsx_texture_loader_thread_info* thread_info = ((vsx_texture_loader_thread_info*)ptr);
    vsx_bitmap* bitmap = thread_info->bitmap;

    pngRawInfo pp;
    if (
        !pngLoadRaw(
          thread_info->filename.c_str(),
          &pp,
          thread_info->filesystem)
        )
      VSX_ERROR_RETURN_V("Could not load PNG",0)

    if (pp.Palette)
      free(pp.Palette);

    bitmap->filename = thread_info->filename;
    bitmap->width = pp.Width;
    bitmap->height = pp.Height;
    bitmap->channels = pp.Components;
    bitmap->data_set( pp.Data, 0, 0, bitmap->width * bitmap->height * bitmap->channels );

    handle_transformations(bitmap);

    delete thread_info;

    bitmap->timestamp = vsx_singleton_counter::get();
    __sync_fetch_and_add( &(bitmap->data_ready), 1 );

    return 0;
  }

  void load_internal(vsx_string<> filename, vsx_filesystem::filesystem* filesystem, vsx_bitmap* bitmap, bool thread, vsx_texture_loader_thread_info* thread_info)
  {
    if (!thread)
      return (void)worker((void*)thread_info);

    new vsx_bitmap_loader_thread(worker, thread_info);
  }

public:

  static vsx_bitmap_loader_png* get_instance()
  {
    static vsx_bitmap_loader_png vtlp;
    return &vtlp;
  }
};

