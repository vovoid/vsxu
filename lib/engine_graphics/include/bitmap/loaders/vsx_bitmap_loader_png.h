#pragma once

#include "vsx_bitmap_loader_base.h"
#include <bitmap/vsx_bitmap_transform.h>
#include <tools/vsx_singleton_counter.h>
#include <tools/vsx_thread_pool.h>
#include <lodepng.h>

class vsx_bitmap_loader_png
    : public vsx_bitmap_loader_base
{

  static void worker(vsx_bitmap* bitmap, vsx::filesystem* filesystem, vsx_string<> filename)
  {
    vsx::file* file = filesystem->f_open(filename.c_str(), "rb");
    req(file);

    unsigned char* data = (unsigned char*)filesystem->f_gets_entire(file);

    unsigned char* result;
    lodepng_decode32(&result, &bitmap->width, &bitmap->height, data, filesystem->f_get_size(file));
    filesystem->f_close(file);

    bitmap->filename = filename;
    bitmap->channels = 4;
    bitmap->data_set( result, 0, 0, bitmap->width * bitmap->height * bitmap->channels );

    handle_transformations(bitmap);

    bitmap->timestamp = vsx_singleton_counter::get();
    bitmap->data_ready.fetch_add(1);
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

