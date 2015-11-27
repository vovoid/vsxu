#ifndef VSX_TEXTURE_DATA_LOADER_H
#define VSX_TEXTURE_DATA_LOADER_H

#include "vsx_texture_data_loader_base.h"
#include "vsx_texture_data_transform.h"

class vsx_texture_data_loader_tga
    : public vsx_texture_data_loader_base
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
    thread_info->texture->texture_data->channels = channels;
    thread_info->texture->texture_data->width = header.width;
    thread_info->texture->texture_data->height = header.height;

    num_bytes = channels * header.width * header.height;
    data = malloc( num_bytes );
    filesystem->f_read( data, num_bytes, file_handle );

    thread_info->texture->filename = thread_info->filename;
    thread_info->texture->texture_data->data[0] = data;
    thread_info->texture->texture_data->mipmaps = thread_info->mipmaps;

    if (thread_info->perform_flip_vertical)
      vsx_texture_data_transform::get_instance()->flip_vertically(thread_info->texture->texture_data);

    if (thread_info->perform_split_into_cubemap)
      vsx_texture_data_transform::get_instance()->split_into_cubemap(thread_info->texture->texture_data);

    __sync_fetch_and_add( &(thread_info->texture->texture_data->data_ready), 1 );

  end:
//    vsx_texture_data_loader_tga::get_instance()->thread_cleanup_queue.produce( thread_info->thread );
    delete thread_info;

    return 0;
  }

  void load_tga(vsx_string<> filename, vsxf* filesystem, vsx_texture* texture, bool mipmaps, bool thread, bool flip_vertical, bool split_cubemap)
  {
    vsx_texture_loader_thread_info* thread_info = new vsx_texture_loader_thread_info();
    thread_info->filename = filename;
    thread_info->mipmaps = mipmaps;
    thread_info->filesystem = filesystem;
    thread_info->texture = texture;
    thread_info->perform_flip_vertical = flip_vertical;
    thread_info->perform_split_into_cubemap = split_cubemap;

    if (thread)
    {
      thread_info->thread = new vsx_texture_data_loader_thread;
      pthread_attr_init(&thread_info->thread->worker_t_attr);
      pthread_create(&(thread_info->thread->worker_t), &(thread_info->thread->worker_t_attr), &worker, (void*)thread_info);
      return;
    }

    worker((void*)thread_info);
  }


public:

  void cleanup(vsx_texture_loader_thread_info* thread_info)
  {
    pthread_join( thread_info->thread->worker_t, 0 );
    pthread_attr_destroy(&thread_info->thread->worker_t_attr);
  }

#define THREAD_YES true
#define THREAD_NO false

#define FLIP_VERTICAL_YES true
#define FLIP_VERTICAL_NO false

#define SPLIT_CUBEMAP_YES true
#define SPLIT_CUBEMAP_NO false

  vsx_texture* load(vsx_string<>filename, vsxf* filesystem, bool mipmaps, bool flip_vertical)
  {
    vsx_texture* texture = new vsx_texture(true);
    if (handle_cache(filename, texture, VSX_TEXTURE_DATA_TYPE_2D))
      return texture;

    load_tga(filename, filesystem, texture, mipmaps, THREAD_NO, flip_vertical, SPLIT_CUBEMAP_NO);
    return texture;
  }

  vsx_texture* load_thread(vsx_string<> filename, vsxf* filesystem, bool mipmaps, bool flip_vertical)
  {
    vsx_texture* texture = new vsx_texture(true);
    if (handle_cache(filename, texture, VSX_TEXTURE_DATA_TYPE_2D))
      return texture;

    load_tga(filename, filesystem, texture, mipmaps, THREAD_YES, flip_vertical, SPLIT_CUBEMAP_NO);
    return texture;
  }

  vsx_texture* load_cube(vsx_string<> filename, vsxf* filesystem, bool mipmaps)
  {
    vsx_texture* texture = new vsx_texture(true);
    if (handle_cache(filename, texture, VSX_TEXTURE_DATA_TYPE_CUBE))
      return texture;

    load_tga(filename, filesystem, texture, mipmaps, THREAD_NO, FLIP_VERTICAL_NO, SPLIT_CUBEMAP_YES);
    return texture;
  }

  vsx_texture* load_cube_thread(vsx_string<> filename, vsxf* filesystem, bool mipmaps)
  {
    vsx_texture* texture = new vsx_texture(true);
    if (handle_cache(filename, texture, VSX_TEXTURE_DATA_TYPE_CUBE))
      return texture;
    load_tga(filename, filesystem, texture, mipmaps, THREAD_YES, FLIP_VERTICAL_NO, SPLIT_CUBEMAP_YES);
    return texture;
  }

#undef THREAD_YES
#undef THREAD_NO

#undef FLIP_VERTICAL_YES
#undef FLIP_VERTICAL_NO

#undef SPLIT_CUBEMAP_YES
#undef SPLIT_CUBEMAP_NO


  static vsx_texture_data_loader_tga* get_instance()
  {
    static vsx_texture_data_loader_tga vtlt;
    return &vtlt;
  }
};



#endif
