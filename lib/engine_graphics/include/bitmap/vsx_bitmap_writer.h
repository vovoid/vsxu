#pragma once

#include "writers/vsx_bitmap_writer_base.h"
#include "writers/vsx_bitmap_writer_tga.h"


class vsx_bitmap_writer
{

  inline static vsx_bitmap_writer_base* get_data_loader_by_image_type(vsx_string<>& filename)
  {
    if (vsx_string_helper::verify_filesuffix(filename, "tga"))
      return vsx_bitmap_writer_tga::get_instance();

    return 0x0;
  }

  inline static void do_write(vsx_bitmap* bitmap, vsx_string<>filename)
  {
    vsx_bitmap_writer_base* data_writer = vsx_bitmap_writer::get_data_loader_by_image_type( filename );
    req_error(data_writer, "unknown image format");
    data_writer->write( bitmap, filename );
  }
public:

  inline static void write(
      vsx_bitmap* bitmap, // target bitmap
      vsx_string<> filename // filename, i.e. "my_path/my_image.tga"
  )
  {
    do_write(bitmap, filename);
  }

};
