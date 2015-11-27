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

  bool handle_cache(vsx_string<>& filename, vsx_texture* texture, size_t type)
  {
    if (vsx_texture_gl_cache::get_instance()->has(filename))
    {
      texture->filename = filename;
      texture->texture_gl = vsx_texture_gl_cache::get_instance()->aquire(filename);
      texture->texture_data = vsx_texture_data_cache::get_instance()->aquire(filename);
      return true;
    }

    texture->texture_data = vsx_texture_data_cache::get_instance()->create( filename, type );
    texture->texture_gl = vsx_texture_gl_cache::get_instance()->create(filename);
    return false;
  }

public:

  virtual vsx_texture* load(vsx_string<> filename, vsxf* filesystem, bool mipmaps, bool flip_vertical) = 0;
  virtual vsx_texture* load_thread(vsx_string<> filename, vsxf* filesystem, bool mipmaps, bool flip_vertical) = 0;
  virtual vsx_texture* load_cube(vsx_string<> filename, vsxf* filesystem, bool mipmaps) = 0;
  virtual vsx_texture* load_cube_thread(vsx_string<> filename, vsxf* filesystem, bool mipmaps) = 0;

  void destroy(vsx_texture* &texture)
  {
    vsx_texture_gl_cache::get_instance()->destroy( texture->filename );
    vsx_texture_data_cache::get_instance()->destroy( texture->filename );
    texture->texture_data = 0x0;
    texture->texture_gl = 0x0;
    delete texture;
    texture = 0;
  }
};

#endif
