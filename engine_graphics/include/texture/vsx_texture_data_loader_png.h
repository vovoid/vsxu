#ifndef VSX_TEXTURE_DATA_LOADER_PNG_H
#define VSX_TEXTURE_DATA_LOADER_PNG_H

#include "vsx_texture_data_loader_base.h"

class vsx_texture_data_loader_png
    : public vsx_texture_data_loader_base
{

  void load_png(vsx_string<> filename, vsxf* filesystem, vsx_texture* texture, bool mipmaps, bool thread)
  {
    vsx_texture_loader_thread_info* thread_info = new vsx_texture_loader_thread_info();
    thread_info->filename = filename;
    thread_info->mipmaps = mipmaps;
    thread_info->filesystem = filesystem;
    thread_info->texture = texture;

    if (thread)
    {
      thread_info->thread = new vsx_texture_data_loader_thread;
      pthread_attr_init(&thread_info->thread->worker_t_attr);
      pthread_create(&(thread_info->thread->worker_t), &(thread_info->thread->worker_t_attr), &worker, (void*)this);
      return;
    }

    worker((void*)thread_info);
  }

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
      return 0;
    }

    if (pp.Palette)
      free(pp.Palette);

    thread_info->texture->filename = thread_info->filename;
    thread_info->texture->texture_data->width = pp.Width;
    thread_info->texture->texture_data->height = pp.Height;
    thread_info->texture->texture_data->data[0] = pp.Data;
    thread_info->texture->texture_data->components = pp.Components;
    thread_info->texture->texture_data->mipmaps = thread_info->mipmaps;

    __sync_fetch_and_add( &(thread_info->texture->texture_data->data_ready), 1 );

    vsx_texture_data_loader_png::get_instance()->thread_cleanup_queue.produce( thread_info->thread );
    delete thread_info;

    return 0;
  }

public:

  void png_worker_cleanup(vsx_texture_loader_thread_info* thread_info)
  {
    pthread_join( thread_info->thread->worker_t, 0 );
    pthread_attr_destroy(&thread_info->thread->worker_t_attr);
  }

  vsx_texture* load(vsx_string<>filename, vsxf* filesystem, bool mipmaps = true)
  {
    vsx_texture* texture = new vsx_texture(true);
    if (handle_cache(filename, texture, VSX_TEXTURE_DATA_TYPE_2D))
      return texture;

    load_png(filename, filesystem, texture, mipmaps, false);
    return texture;
  }

  vsx_texture* load_thread(vsx_string<> filename, vsxf* filesystem, bool mipmaps = true)
  {
    vsx_texture* texture = new vsx_texture(true);
    if (handle_cache(filename, texture, VSX_TEXTURE_DATA_TYPE_2D))
      return texture;

    load_png(filename, filesystem, texture, mipmaps, true);
    return texture;
  }

  vsx_texture* load_cube(vsx_string<> filename, vsxf* filesystem, bool mipmaps)
  {
    vsx_texture* texture = new vsx_texture(true);
    if (handle_cache(filename, texture, VSX_TEXTURE_DATA_TYPE_CUBE))
      return texture;

    load_png(filename, filesystem, texture, mipmaps, false);
    return texture;
  }

  vsx_texture* load_cube_thread(vsx_string<> filename, vsxf* filesystem, bool mipmaps)
  {
    vsx_texture* texture = new vsx_texture(true);
    if (handle_cache(filename, texture, VSX_TEXTURE_DATA_TYPE_CUBE))
      return texture;
    load_png(filename, filesystem, texture, mipmaps, true);
    return texture;
  }

  static vsx_texture_data_loader_png* get_instance()
  {
    static vsx_texture_data_loader_png vtlp;
    return &vtlp;
  }
};

#endif
