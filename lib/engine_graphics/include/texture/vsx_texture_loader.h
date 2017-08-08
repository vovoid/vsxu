#pragma once

#include "gl/vsx_texture_gl_cache.h"

#include <bitmap/vsx_bitmap_loader.h>

class vsx_texture_loader
{

  static vsx_texture_gl* handle_cache(vsx_string<> filename, vsx::filesystem* filesystem, bool thread, uint64_t bitmap_loader_hint, uint64_t hint, bool try_to_reload = false)
  {
    if (vsx_texture_gl_cache::get_instance()->has(filename, bitmap_loader_hint, hint))
      return vsx_texture_gl_cache::get_instance()->aquire(filename, filesystem, thread, bitmap_loader_hint, hint, try_to_reload );
    return 0;
  }

public:

  template <class T = vsx_texture_gl>
  static inline std::unique_ptr<vsx_texture<T>> load(
      vsx_string<> filename, // i.e. "my_image.png" or "my_image.jpg"
      vsx::filesystem* filesystem,  // your filesystem or vsx::filesystem::get_instance()
      bool thread,  // load in a thread or not, recommended when doing bitmap transforms
      uint64_t bitmap_loader_hint, // bitmap transforms and other hints, see vsx_bitmap::loader_hint enum
      uint64_t gl_loader_hint, // gl hint, see vsx_texture_gl::loader_hint
      bool try_to_reload = false // notifies the cache that if the image is already loaded, just reload it
  )
  {
    std::unique_ptr<vsx_texture<T>> texture( new vsx_texture<T>(true) );
    texture->texture = handle_cache(filename, filesystem, thread, bitmap_loader_hint, gl_loader_hint, try_to_reload );
    if (texture->texture)
      return std::move(texture);

    texture->texture = vsx_texture_gl_cache::get_instance()->create(filename, bitmap_loader_hint, gl_loader_hint);

    vsx_bitmap_loader::load(
        texture->texture->bitmap,
        filename,
        filesystem,
        thread,
        bitmap_loader_hint
      )
    ;

    return std::move(texture);
  }

  template <class T = vsx_texture_gl>
  static inline void destroy(vsx_texture<T>* &texture)
  {
    if (!texture)
      VSX_ERROR_RETURN("Texture is null");

    if (!texture->texture)
      VSX_ERROR_RETURN("Trying to destroy a texture without texture");

    if (!texture->texture->bitmap)
      VSX_ERROR_RETURN("Trying to destroy a texture without bitmap");

    if (!texture->texture->bitmap->attached_to_cache)
      VSX_ERROR_RETURN("Trying to destroy an unconnected texture");

    if (!texture->texture->bitmap->filename.size())
      VSX_ERROR_RETURN("Trying to destroy a texture with no filename set");

    delete texture;
    texture = 0x0;
  }
};


#define vsx_texture_load_gl_once(t, filename, filesystem, gl_hint) \
  if (!t) \
    t = vsx_texture_loader::load(filename, filesystem, true, 0, gl_hint);
