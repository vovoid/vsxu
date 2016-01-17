#pragma once

#include "vsx_bitmap_loader_base.h"
#include <bitmap/vsx_bitmap_transform.h>
#include <tools/vsx_singleton_counter.h>

class vsx_bitmap_loader_png
    : public vsx_bitmap_loader_base
{

  static void* worker(vsx_bitmap* bitmap, vsx::filesystem* filesystem, vsx_string<> filename)
  {
    pngRawInfo pp;
    if (
        !pngLoadRaw(
          filename.c_str(),
          &pp,
          filesystem)
        )
      VSX_ERROR_RETURN_V("Could not load PNG",0)

    if (pp.Palette)
      free(pp.Palette);

    bitmap->filename = filename;
    bitmap->width = pp.Width;
    bitmap->height = pp.Height;
    bitmap->channels = pp.Components;
    bitmap->data_set( pp.Data, 0, 0, bitmap->width * bitmap->height * bitmap->channels );

    handle_transformations(bitmap);

    bitmap->timestamp = vsx_singleton_counter::get();
    __sync_fetch_and_add( &(bitmap->data_ready), 1 );

    return 0;
  }

  void load_internal(vsx_string<> filename, vsx::filesystem* filesystem, vsx_bitmap* bitmap, bool thread)
  {
    if (!thread)
      return (void)worker(bitmap, filesystem, filename);

    vsx_thread_pool::instance()->add(
      [=]
      (vsx_bitmap* bitmap, vsx::filesystem* filesystem, vsx_string<> filename)
      {
        worker(bitmap, filesystem, filename);
      },
      bitmap,
      filesystem,
      filename
    );
  }

public:

  static vsx_bitmap_loader_png* get_instance()
  {
    static vsx_bitmap_loader_png vtlp;
    return &vtlp;
  }
};

