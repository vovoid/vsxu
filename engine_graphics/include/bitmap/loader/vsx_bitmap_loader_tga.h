#ifndef VSX_BITMAP_LOADER_H
#define VSX_BITMAP_LOADER_H

#include "vsx_bitmap_loader_base.h"
#include <bitmap/vsx_bitmap_transform.h>

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

    vsxf* filesystem = thread_info->filesystem;

    vsxf_handle* file_handle = filesystem->f_open(thread_info->filename.c_str(), "rb");
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
    thread_info->bitmap->channels = channels;
    thread_info->bitmap->width = header.width;
    thread_info->bitmap->height = header.height;
    thread_info->bitmap->channels_bgra = true;

    num_bytes = channels * header.width * header.height;
    data = malloc( num_bytes );
    filesystem->f_read( data, num_bytes, file_handle );

    thread_info->bitmap->filename = thread_info->filename;
    thread_info->bitmap->data[0] = data;

    if (thread_info->hint.flip_vertically)
      vsx_bitmap_transform::get_instance()->flip_vertically(thread_info->bitmap);

    if (thread_info->hint.split_cubemap)
      vsx_bitmap_transform::get_instance()->split_into_cubemap(thread_info->bitmap);

    __sync_fetch_and_add( &(thread_info->bitmap->data_ready), 1 );

  end:
//    vsx_bitmap_loader_png::get_instance()->thread_cleanup_queue.produce( (vsx_bitmap_loader_thread*)thread_info->thread );
    delete thread_info;

    return 0;
  }

  void load_internal(vsx_string<> filename, vsxf* filesystem, vsx_bitmap* bitmap, bool thread, vsx_texture_loader_thread_info* thread_info)
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



#endif
