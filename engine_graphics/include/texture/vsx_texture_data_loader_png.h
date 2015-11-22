#ifndef VSX_TEXTURE_DATA_LOADER_PNG_H
#define VSX_TEXTURE_DATA_LOADER_PNG_H

#include "vsx_texture_data_loader_base.h"
#include "vsx_texture_data_transform.h"

class vsx_texture_data_loader_png
    : public vsx_texture_data_loader_base
{

  static void* worker(void *ptr)
  {
    vsx_texture_loader_thread_info* thread_info = ((vsx_texture_loader_thread_info*)ptr);

    pngRawInfo pp;
    if (
        !pngLoadRaw(
          thread_info->filename.c_str(),
          &pp,
          thread_info->filesystem)
        )
    {

      delete thread_info;
      VSX_ERROR_RETURN_V("Could not load PNG",0)
    }

    if (pp.Palette)
      free(pp.Palette);

    thread_info->texture->filename = thread_info->filename;
    thread_info->texture->texture_data->width = pp.Width;
    thread_info->texture->texture_data->height = pp.Height;
    thread_info->texture->texture_data->data[0] = pp.Data;
    thread_info->texture->texture_data->channels = pp.Components;
    thread_info->texture->texture_data->mipmaps = thread_info->mipmaps;

    if (thread_info->perform_flip_vertical)
      vsx_texture_data_transform::get_instance()->flip_vertically(thread_info->texture->texture_data);

    if (thread_info->perform_split_into_cubemap)
      vsx_texture_data_transform::get_instance()->split_into_cubemap(thread_info->texture->texture_data);

    __sync_fetch_and_add( &(thread_info->texture->texture_data->data_ready), 1 );

//    vsx_texture_data_loader_png::get_instance()->thread_cleanup_queue.produce( thread_info->thread );
    delete thread_info;

    return 0;
  }

  void load_png(vsx_string<> filename, vsxf* filesystem, vsx_texture* texture, bool mipmaps, bool thread, bool flip_vertical, bool split_cubemap)
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

  vsx_texture* load(vsx_string<>filename, vsxf* filesystem, bool mipmaps = true)
  {
    vsx_texture* texture = new vsx_texture(true);
    if (handle_cache(filename, texture, VSX_TEXTURE_DATA_TYPE_2D))
      return texture;

    load_png(filename, filesystem, texture, mipmaps, THREAD_NO, FLIP_VERTICAL_YES, SPLIT_CUBEMAP_NO);
    return texture;
  }

  vsx_texture* load_thread(vsx_string<> filename, vsxf* filesystem, bool mipmaps = true)
  {
    vsx_texture* texture = new vsx_texture(true);
    if (handle_cache(filename, texture, VSX_TEXTURE_DATA_TYPE_2D))
      return texture;

    load_png(filename, filesystem, texture, mipmaps, THREAD_YES, FLIP_VERTICAL_YES, SPLIT_CUBEMAP_NO);
    return texture;
  }

  vsx_texture* load_cube(vsx_string<> filename, vsxf* filesystem, bool mipmaps)
  {
    vsx_texture* texture = new vsx_texture(true);
    if (handle_cache(filename, texture, VSX_TEXTURE_DATA_TYPE_CUBE))
      return texture;

    load_png(filename, filesystem, texture, mipmaps, THREAD_NO, FLIP_VERTICAL_NO, SPLIT_CUBEMAP_YES);
    return texture;
  }

  vsx_texture* load_cube_thread(vsx_string<> filename, vsxf* filesystem, bool mipmaps)
  {
    vsx_texture* texture = new vsx_texture(true);
    if (handle_cache(filename, texture, VSX_TEXTURE_DATA_TYPE_CUBE))
      return texture;
    load_png(filename, filesystem, texture, mipmaps, THREAD_YES, FLIP_VERTICAL_NO, SPLIT_CUBEMAP_YES);
    return texture;
  }

#undef THREAD_YES
#undef THREAD_NO

#undef FLIP_VERTICAL_YES
#undef FLIP_VERTICAL_NO

#undef SPLIT_CUBEMAP_YES
#undef SPLIT_CUBEMAP_NO


  static vsx_texture_data_loader_png* get_instance()
  {
    static vsx_texture_data_loader_png vtlp;
    return &vtlp;
  }
};

#endif
