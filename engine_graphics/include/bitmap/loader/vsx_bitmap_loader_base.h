#pragma once

#include <bitmap/vsx_bitmap.h>
#include <bitmap/vsx_bitmap_transform.h>
#include "vsx_bitmap_loader_thread.h"
#include "vsx_bitmap_loader_thread_info.h"

#include <filesystem/vsx_filesystem.h>
#include <tools/vsx_req.h>

class vsx_bitmap_loader_base
{
protected:

  virtual void load_internal(vsx_string<> filename, vsx_filesystem::filesystem* filesystem, vsx_bitmap* bitmap, bool thread, vsx_texture_loader_thread_info* thread_info) = 0;

  static void handle_transformations(vsx_bitmap* bitmap)
  {
    if (bitmap->hint & vsx_bitmap::flip_vertical_hint)
      vsx_bitmap_transform::get_instance()->flip_vertically(bitmap);

    if (bitmap->hint & vsx_bitmap::cubemap_split_6_1_hint)
      vsx_bitmap_transform::get_instance()->split_into_cubemap(bitmap);

    if (bitmap->hint & vsx_bitmap::cubemap_sphere_map)
      vsx_bitmap_transform::get_instance()->sphere_map_into_cubemap(bitmap);
  }

public:

  void load(vsx_bitmap* bitmap, vsx_string<>filename, vsx_filesystem::filesystem* filesystem, bool thread)
  {
    vsx_texture_loader_thread_info* thread_info = new vsx_texture_loader_thread_info();
    thread_info->filename = filename;
    thread_info->filesystem = filesystem;
    thread_info->bitmap = bitmap;

    bitmap->data_ready = 0;
    load_internal(filename, filesystem, bitmap, thread, thread_info);
  }
};
