#pragma once

#include <stdio.h>
#include <tools/vsx_singleton_counter.h>

#include <string/vsx_string_helper.h>

#include <bitmap/vsx_bitmap_transform.h>
#include "vsx_bitmap_writer_base.h"

class vsx_bitmap_writer_tga
    : public vsx_bitmap_writer_base
{

  VSX_PACK_BEGIN
  struct tga_header {
     uint8_t id_length = 0;
     uint8_t color_map_type = 0;
     uint8_t data_type_code = 0;

     int16_t color_map_origin = 0;
     int16_t color_map_length = 0;
     uint8_t color_map_depth = 0;

     int16_t x_origin = 0;
     int16_t y_origin = 0;
     int16_t width = 0;
     int16_t height = 0;

     uint8_t bits_per_pixel = 0;
     uint8_t image_descriptor = 0;
  }
  VSX_PACK_END


  static void worker_cubemap_side(vsx_bitmap* bitmap, vsx_string<> filename, size_t side = 0)
  {
    vsx_string<> target_filename = vsx_string_helper::add_filename_suffix(filename, vsx_string_helper::i2s(side));
    FILE* file_handle = fopen( target_filename.c_str(), "wb");
    tga_header header;

    header.data_type_code = 2;
    header.bits_per_pixel = bitmap->channels * 8;
    header.width = bitmap->height;
    header.height = bitmap->height;
    fwrite(&header, sizeof(header), 1, file_handle );
    fwrite(bitmap->data_get(0, side), 1, bitmap->data_size_get(0, side), file_handle );
    fclose(file_handle);
  }

  static void worker(vsx_bitmap* bitmap, vsx_string<> filename)
  {
    FILE* file_handle = fopen(filename.c_str(), "wb");
    tga_header header;
    header.data_type_code = 2;
    header.bits_per_pixel = bitmap->channels * 8;
    header.width = bitmap->width;
    header.height = bitmap->height;
    fwrite(&header, sizeof(header), 1, file_handle );
    fwrite(bitmap->data_get(), 1, bitmap->data_size_get(), file_handle );
    fclose(file_handle);
  }

  void write_internal(vsx_string<> filename, vsx_bitmap* bitmap)
  {
    if (bitmap->sides_count_get() > 1)
    {
      for_n(i, 0, bitmap->sides_count_get())
        worker_cubemap_side(bitmap, filename, i);
      return;
    }
    worker(bitmap, filename);
  }

public:

  static vsx_bitmap_writer_tga* get_instance()
  {
    static vsx_bitmap_writer_tga vtrt;
    return &vtrt;
  }
};
