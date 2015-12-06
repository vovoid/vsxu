#ifndef VSX_BITMAP_LOADER_PNG_H
#define VSX_BITMAP_LOADER_PNG_H

#include "vsx_bitmap_loader_base.h"
#include <bitmap/vsx_bitmap_transform.h>
#include <vsx_singleton_counter.h>

class vsx_bitmap_loader_png
    : public vsx_bitmap_loader_base
{

  static void* worker(void *ptr)
  {
    vsx_texture_loader_thread_info* thread_info = ((vsx_texture_loader_thread_info*)ptr);


    pngRawInfo pp;
    if (
        !pngLoadRaw(
          thread_info->filename.c_str(),
          &pp,
          thread_info->filesystem)
        )
    {
      VSX_ERROR_RETURN_V("Could not load PNG",0)
    }

    if (pp.Palette)
      free(pp.Palette);

    thread_info->bitmap->filename = thread_info->filename;
    thread_info->bitmap->width = pp.Width;
    thread_info->bitmap->height = pp.Height;
    thread_info->bitmap->data_set( pp.Data );
    thread_info->bitmap->channels = pp.Components;

    if (thread_info->hint & vsx_bitmap::flip_vertical_hint)
      vsx_bitmap_transform::get_instance()->flip_vertically(thread_info->bitmap);

    if (thread_info->hint & vsx_bitmap::cubemap_split_6_1_hint)
      vsx_bitmap_transform::get_instance()->split_into_cubemap(thread_info->bitmap);

    thread_info->bitmap->timestamp = vsx_singleton_counter::get();
    __sync_fetch_and_add( &(thread_info->bitmap->data_ready), 1 );

    delete thread_info;

    return 0;
  }

  void load_internal(vsx_string<> filename, vsxf* filesystem, vsx_bitmap* bitmap, bool thread, vsx_texture_loader_thread_info* thread_info)
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

#endif
