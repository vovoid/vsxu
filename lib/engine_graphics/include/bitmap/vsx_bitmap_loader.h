#pragma once

#include <tools/vsx_singleton_counter.h>

#include "loaders/vsx_bitmap_loader_png.h"
#include "loaders/vsx_bitmap_loader_jpg.h"
#include "loaders/vsx_bitmap_loader_tga.h"
#include "loaders/vsx_bitmap_loader_dds.h"

class vsx_bitmap_loader
{
  static vsx_bitmap_loader_base* get_data_loader_by_image_type(vsx_string<>& filename);

  static void do_load(vsx_bitmap* bitmap, vsx_string<>filename, vsx::filesystem* filesystem, bool thread);

public:

  static void reload(
      vsx_bitmap* bitmap, // target bitmap
      vsx_string<>filename, // filename, i.e. "my_path/my_image.png" or "other_path/other_image.jpg"
      vsx::filesystem* filesystem, // filesystem pointer or vsx::filesystem::get_instance()
      bool thread, // load in a thread or not, recommended when doing bitmap transforms
      uint64_t hint // bitmap transforms and other hints, see vsx_bitmap::loader_hint enum
  );

  static void load(
      vsx_bitmap* bitmap, // target bitmap
      vsx_string<> filename, // filename, i.e. "my_path/my_image.png" or "other_path/other_image.jpg"
      vsx::filesystem* filesystem, // filesystem pointer or vsx::filesystem::get_instance()
      bool thread, // load in a thread or not, recommended when doing bitmap transforms
      uint64_t hint // bitmap transforms and other hints, see vsx_bitmap::loader_hint enum
  );
};
