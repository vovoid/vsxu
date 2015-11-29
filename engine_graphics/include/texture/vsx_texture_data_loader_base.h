#ifndef VSX_TEXTURE_DATA_LOADER_BASE_H
#define VSX_TEXTURE_DATA_LOADER_BASE_H

#include "vsx_texture.h"
#include "vsx_texture_data_loader_thread.h"
#include "vsx_texture_data_loader_thread_info.h"

#include <vsxfst.h>
#include <tools/vsx_req.h>
#include <vsx_fifo.h>


class vsx_texture_data_loader_base
{
protected:

  vsx_fifo<vsx_texture_data_loader_thread*, 64> thread_cleanup_queue;

  virtual void load_internal(vsx_string<> filename, vsxf* filesystem, vsx_texture_data* texture_data, bool thread, vsx_texture_loader_thread_info* thread_info) = 0;

public:

  void cleanup()
  {
    // consume thread_cleanup_queue
    //pthread_join( thread_info->thread->worker_t, 0 );
    //pthread_attr_destroy(&thread_info->thread->worker_t_attr);
  }

  void load(vsx_texture_data* texture_data, vsx_string<>filename, vsxf* filesystem, bool thread, vsx_texture_data_hint hint)
  {
    vsx_texture_loader_thread_info* thread_info = new vsx_texture_loader_thread_info();
    thread_info->filename = filename;
    thread_info->filesystem = filesystem;
    thread_info->texture_data = texture_data;
    thread_info->hint = hint;

    texture_data->data_ready = 0;
    load_internal(filename, filesystem, texture_data, thread, thread_info);
  }
};

#endif
