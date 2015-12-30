#pragma once

#include "vsx_bitmap_loader_base.h"
#include <bitmap/vsx_bitmap_transform.h>
#include <vsx_singleton_counter.h>

class vsx_bitmap_loader_tga
    : public vsx_bitmap_loader_base
{

  struct tga_header {
     uint8_t id_length;
     uint8_t color_map_type;
     uint8_t data_type_code;

     int16_t color_map_origin;
     int16_t color_map_length;
     uint8_t color_map_depth;

     int16_t x_origin;
     int16_t y_origin;
     int16_t width;
     int16_t height;

     uint8_t bits_per_pixel;
     uint8_t image_descriptor;
  } __attribute__((packed));


  static void* worker(void *ptr)
  {
    vsx_texture_loader_thread_info* thread_info = ((vsx_texture_loader_thread_info*)ptr);
    vsx_bitmap* bitmap = thread_info->bitmap;
    vsx::filesystem* filesystem = thread_info->filesystem;

    vsx::file* file_handle = filesystem->f_open(thread_info->filename.c_str(), "rb");
    tga_header header;
    filesystem->f_read( &header, sizeof(header), file_handle );

    void* data = 0;
    unsigned long num_bytes = 0;
    int channels = 0;

    if (header.data_type_code != 2)
      goto end;

    // Read past the ID text
    if (header.id_length)
    {
      char buf[256];
      filesystem->f_read( buf, header.id_length, file_handle );
    }

    channels = header.bits_per_pixel / 8;
    bitmap->channels = channels;
    bitmap->width = header.width;
    bitmap->height = header.height;
    bitmap->channels_bgra = true;

    num_bytes = channels * header.width * header.height;
    data = malloc( num_bytes );
    filesystem->f_read( data, num_bytes, file_handle );

    filesystem->f_close(file_handle);

    bitmap->filename = thread_info->filename;
    bitmap->data_set( data, 0, 0, num_bytes );

    handle_transformations(bitmap);

    bitmap->timestamp = vsx_singleton_counter::get();
    __sync_fetch_and_add( &(bitmap->data_ready), 1 );

  end:
    delete thread_info;

    return 0;
  }

  void load_internal(vsx_string<> filename, vsx::filesystem* filesystem, vsx_bitmap* bitmap, bool thread, vsx_texture_loader_thread_info* thread_info)
  {
    if (!thread)
      return (void)worker((void*)thread_info);

    new vsx_bitmap_loader_thread(worker, thread_info);
  }
public:

  static vsx_bitmap_loader_tga* get_instance()
  {
    static vsx_bitmap_loader_tga vtlt;
    return &vtlt;
  }
};
