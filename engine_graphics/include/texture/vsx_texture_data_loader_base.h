#ifndef VSX_TEXTURE_LOADER_H
#define VSX_TEXTURE_LOADER_H

#include "vsx_texture.h"
#include "vsx_texture_data_cache.h"
#include "vsx_texture_gl_cache.h"
#include "vsx_texture_data_loader_thread.h"
#include "vsx_texture_data_loader_thread_info.h"

#include <vsxfst.h>
#include <tools/vsx_req.h>
#include <vsx_fifo.h>


class vsx_texture_data_loader_base
{
protected:

  vsx_fifo<vsx_texture_data_loader_thread*, 64> thread_cleanup_queue;

  vsx_texture_data* handle_cache(vsx_string<>& filename, vsx_texture_data_hint hint)
  {
    if (vsx_texture_data_cache::get_instance()->has(filename, hint))
      return vsx_texture_data_cache::get_instance()->aquire(filename, hint);
    return 0x0;
  }


  virtual void load_internal(vsx_string<> filename, vsxf* filesystem, vsx_texture_data* texture_data, bool thread, vsx_texture_data_hint hint) = 0;

public:

  void cleanup()
  {
    // consume thread_cleanup_queue
    //pthread_join( thread_info->thread->worker_t, 0 );
    //pthread_attr_destroy(&thread_info->thread->worker_t_attr);
  }

  vsx_texture_data* load(vsx_string<>filename, vsxf* filesystem, bool thread, vsx_texture_data_hint hint)
  {
    vsx_texture_data* texture_data = handle_cache(filename, hint);
    if (texture_data)
      return texture_data;

    texture_data = vsx_texture_data_cache::get_instance()->create( filename, hint );
    load_internal(filename, filesystem, texture_data, thread, hint);
    return texture_data;
  }
};

#endif
