#pragma once

#include "vsx_bitmap_loader_base.h"
#include <bitmap/vsx_bitmap_transform.h>
#include <tools/vsx_singleton_counter.h>

class vsx_bitmap_loader_jpg
    : public vsx_bitmap_loader_base
{
  static void* worker(void *ptr)
  {
    vsx_texture_loader_thread_info* thread_info = ((vsx_texture_loader_thread_info*)ptr);

    CJPEGTest cj;
    vsx_string<> ret;
    cj.LoadJPEG( thread_info->filename, ret, thread_info->filesystem);

    vsx_bitmap* bitmap = thread_info->bitmap;

    bitmap->filename = thread_info->filename;
    bitmap->width = cj.GetResX();
    bitmap->height = cj.GetResY();
    bitmap->data_set( cj.m_pBuf );
    bitmap->channels = 3;

    handle_transformations(bitmap);

    delete thread_info;

    bitmap->timestamp = vsx_singleton_counter::get();
    __sync_fetch_and_add( &(bitmap->data_ready), 1 );

    return 0;
  }

  void load_internal(vsx_string<> filename, vsx::filesystem* filesystem, vsx_bitmap* bitmap, bool thread, vsx_texture_loader_thread_info* thread_info)
  {
    if (!thread)
      return (void)worker((void*)thread_info);

    new vsx_bitmap_loader_thread(worker, thread_info);
  }

public:

  static vsx_bitmap_loader_jpg* get_instance()
  {
    static vsx_bitmap_loader_jpg vtlj;
    return &vtlj;
  }

};
