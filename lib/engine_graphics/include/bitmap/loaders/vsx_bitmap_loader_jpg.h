#pragma once

#include "vsx_bitmap_loader_base.h"
#include <bitmap/vsx_bitmap_transform.h>
#include <tools/vsx_singleton_counter.h>
#include <filesystem/vsx_filesystem_helper.h>

class vsx_bitmap_loader_jpg
    : public vsx_bitmap_loader_base
{
  static void worker(vsx_bitmap* bitmap, vsx::filesystem* filesystem, vsx_string<> filename);

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
