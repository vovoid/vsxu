#ifndef VSX_TEXTURE_DATA_LOADER_HELPER_H
#define VSX_TEXTURE_DATA_LOADER_HELPER_H

#include "vsx_texture_data_loader_png.h"
#include "vsx_texture_data_loader_jpg.h"
#include "vsx_texture_data_loader_tga.h"
#include "vsx_texture_gl_hint.h"

class vsx_texture_loader
{

  static vsx_texture_gl* handle_cache(vsx_string<> filename, vsx_texture_gl_hint hint)
  {
    if (vsx_texture_gl_cache::get_instance()->has(filename, hint))
      return vsx_texture_gl_cache::get_instance()->aquire(filename, hint);

    return 0;
  }

public:

  static inline vsx_texture* load(vsx_string<> filename, vsxf* filesystem, bool thread, vsx_texture_gl_hint hint)
  {
    vsx_texture* texture = new vsx_texture(true);
    texture->texture_gl = handle_cache(filename, hint );
    if (texture->texture_gl)
      return texture;

    texture->texture_gl = vsx_texture_gl_cache::get_instance()->create(filename, hint);

    vsx_texture_data_loader_base* data_loader = 0x0;

    if (verify_filesuffix(filename, "png"))
      data_loader = vsx_texture_data_loader_png::get_instance();

    if (verify_filesuffix(filename, "jpg"))
      data_loader = vsx_texture_data_loader_jpg::get_instance();

    if (verify_filesuffix(filename, "tga"))
      data_loader = vsx_texture_data_loader_tga::get_instance();

    if (!data_loader)
      VSX_ERROR_RETURN_V("Unsupported image format", texture);

    texture->texture_gl->texture_data =
      data_loader->load(
        filename,
        filesystem,
        thread,
        vsx_texture_data_hint(
          hint.data_flip_vertically,
          hint.data_split_cubemap
        )
      );

    return texture;
  }

  static inline void destroy(vsx_texture* &texture)
  {
    if (!texture)
      VSX_ERROR_RETURN("Texture is null");

    if (!texture->texture_gl)
      VSX_ERROR_RETURN("Trying to destroy a texture without texture_gl");

//    if (!texture->texture_gl->texture_data)
//      VSX_ERROR_RETURN("Trying to destroy a texture without texture_data");

//    if (!texture->texture_gl->texture_data->attached_to_cache)
//      VSX_ERROR_RETURN("Trying to destroy an unconnected texture");

//    if (!texture->texture_gl->texture_data->filename.size())
//      VSX_ERROR_RETURN("Trying to destroy a texture with no filename set");

    vsx_texture_gl_cache::get_instance()->destroy( texture->texture_gl );
    delete texture;
    texture = 0;
  }
};

#endif
