#pragma once

#include "vsx_bitmap_loader_base.h"
#include <bitmap/vsx_bitmap_transform.h>
#include <tools/vsx_singleton_counter.h>
#include <jpeg_decoder.h>
#include <filesystem/vsx_filesystem_helper.h>

class vsx_bitmap_loader_jpg
    : public vsx_bitmap_loader_base
{
  static void worker(vsx_bitmap* bitmap, vsx::filesystem* filesystem, vsx_string<> filename)
  {
    vsx::file* file = filesystem->f_open(filename.c_str(), "rb");
    req_v(file);
    
    unsigned char* data = (unsigned char*)filesystem->f_gets_entire(file);
    filesystem->f_close(file);
    Jpeg::Decoder decoder(data, filesystem->f_get_size(file));

    bitmap->filename = filename;
    bitmap->width = decoder.GetWidth();
    bitmap->height = decoder.GetHeight();
    bitmap->data_set( decoder.GetImage() );

    if (decoder.IsColor())
      bitmap->channels = 3;
    else
      bitmap->channels = 1;

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

  static vsx_bitmap_loader_jpg* get_instance()
  {
    static vsx_bitmap_loader_jpg vtlj;
    return &vtlj;
  }

};
