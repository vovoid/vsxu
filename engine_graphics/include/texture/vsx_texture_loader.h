#ifndef VSX_TEXTURE_LOADER_H
#define VSX_TEXTURE_LOADER_H

#include "gl/vsx_texture_gl_loader_hint.h"
#include "gl/vsx_texture_gl_cache.h"

#include <bitmap/loader/vsx_bitmap_loader_hint.h>
#include <bitmap/loader/vsx_bitmap_loader_helper.h>

class vsx_texture_loader
{

  static vsx_texture_gl* handle_cache(vsx_string<> filename, vsxf* filesystem, bool thread, vsx_texture_gl_loader_hint hint)
  {
    if (vsx_texture_gl_cache::get_instance()->has(filename, hint))
      return vsx_texture_gl_cache::get_instance()->aquire(filename, filesystem, thread, hint );
    return 0;
  }

public:

  template <class T = vsx_texture_gl>
  static inline vsx_texture<T>* load(vsx_string<> filename, vsxf* filesystem, bool thread, vsx_texture_gl_loader_hint hint)
  {
    vsx_texture<T>* texture = new vsx_texture<T>(true);
    texture->texture = handle_cache(filename, filesystem, thread, hint );
    if (texture->texture)
      return texture;

    texture->texture = vsx_texture_gl_cache::get_instance()->create(filename, hint);

    vsx_bitmap_loader_helper::load(
        texture->texture->bitmap,
        filename,
        filesystem,
        thread,
        vsx_bitmap_loader_hint(
          hint.data_flip_vertically,
          hint.data_split_cubemap
        )
      )
    ;

    return texture;
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

    vsx_texture_gl_cache::get_instance()->destroy( texture->texture );
    delete texture;
    texture = 0;
  }
};

#endif
