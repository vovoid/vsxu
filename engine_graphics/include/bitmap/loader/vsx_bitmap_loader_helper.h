#ifndef VSX_BITMAP_LOADER_HELPER_H
#define VSX_BITMAP_LOADER_HELPER_H

#include <vsx_singleton_counter.h>

#include "vsx_bitmap_loader_png.h"
#include "vsx_bitmap_loader_jpg.h"
#include "vsx_bitmap_loader_tga.h"

class vsx_bitmap_loader_helper
{

  inline static vsx_bitmap_loader_base* get_data_loader_by_image_type(vsx_string<>& filename)
  {
    if (verify_filesuffix(filename, "png"))
      return vsx_bitmap_loader_png::get_instance();

    if (verify_filesuffix(filename, "jpg"))
      return vsx_bitmap_loader_jpg::get_instance();

    if (verify_filesuffix(filename, "tga"))
      return vsx_bitmap_loader_tga::get_instance();

    return 0x0;
  }

  inline static void do_load(vsx_bitmap* bitmap, vsx_string<>filename, vsxf* filesystem, bool thread, uint64_t hint)
  {
    vsx_bitmap_loader_base* data_loader = vsx_bitmap_loader_helper::get_data_loader_by_image_type( filename );
    req_error(data_loader, "unknown image format");
    data_loader->load( bitmap, filename, filesystem, thread, hint );
  }

public:

  inline static void reload(vsx_bitmap* bitmap, vsx_string<>filename, vsxf* filesystem, bool thread, uint64_t hint)
  {
    do_load(bitmap, filename, filesystem, thread, hint);
  }

  inline static void load(vsx_bitmap* bitmap, vsx_string<>filename, vsxf* filesystem, bool thread, uint64_t hint)
  {
    if (bitmap->references > 1)
      ret(bitmap->timestamp = vsx_singleton_counter::get());

    do_load(bitmap, filename, filesystem, thread, hint);
  }
};

#endif
