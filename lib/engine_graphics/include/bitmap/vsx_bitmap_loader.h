#pragma once

#include <tools/vsx_singleton_counter.h>

#include "loaders/vsx_bitmap_loader_png.h"
#include "loaders/vsx_bitmap_loader_jpg.h"
#include "loaders/vsx_bitmap_loader_tga.h"
#include "loaders/vsx_bitmap_loader_dds.h"

class vsx_bitmap_loader
{
  inline static vsx_bitmap_loader_base* get_data_loader_by_image_type(vsx_string<>& filename)
  {
    if (vsx_string_helper::verify_filesuffix(filename, "png"))
      return vsx_bitmap_loader_png::get_instance();

    if (vsx_string_helper::verify_filesuffix(filename, "jpg"))
      return vsx_bitmap_loader_jpg::get_instance();

    if (vsx_string_helper::verify_filesuffix(filename, "tga"))
      return vsx_bitmap_loader_tga::get_instance();

    if (vsx_string_helper::verify_filesuffix(filename, "dds"))
      return vsx_bitmap_loader_dds::get_instance();

    return 0x0;
  }

  inline static void do_load(vsx_bitmap* bitmap, vsx_string<>filename, vsx::filesystem* filesystem, bool thread)
  {
    vsx_bitmap_loader_base* data_loader = vsx_bitmap_loader::get_data_loader_by_image_type( filename );
    req_error(data_loader, "unknown image format");
    data_loader->load( bitmap, filename, filesystem, thread );
  }

public:

  inline static void reload(
      vsx_bitmap* bitmap, // target bitmap
      vsx_string<>filename, // filename, i.e. "my_path/my_image.png" or "other_path/other_image.jpg"
      vsx::filesystem* filesystem, // filesystem pointer or vsx::filesystem::get_instance()
      bool thread, // load in a thread or not, recommended when doing bitmap transforms
      uint64_t hint // bitmap transforms and other hints, see vsx_bitmap::loader_hint enum
  )
  {
    bitmap->hint = hint;
    do_load(bitmap, filename, filesystem, thread);
  }

  inline static void load(
      vsx_bitmap* bitmap, // target bitmap
      vsx_string<> filename, // filename, i.e. "my_path/my_image.png" or "other_path/other_image.jpg"
      vsx::filesystem* filesystem, // filesystem pointer or vsx::filesystem::get_instance()
      bool thread, // load in a thread or not, recommended when doing bitmap transforms
      uint64_t hint // bitmap transforms and other hints, see vsx_bitmap::loader_hint enum
  )
  {
    if (bitmap->references > 1)
      ret(bitmap->timestamp = vsx_singleton_counter::get());
    bitmap->hint = hint;

    do_load(bitmap, filename, filesystem, thread);
  }
};
