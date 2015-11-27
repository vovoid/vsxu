#ifndef VSX_TEXTURE_DATA_LOADER_HELPER_H
#define VSX_TEXTURE_DATA_LOADER_HELPER_H

#include "vsx_texture_data_loader_png.h"
#include "vsx_texture_data_loader_jpg.h"
#include "vsx_texture_data_loader_tga.h"

namespace vsx_texture_data_loader_helper
{
  inline vsx_texture* load(vsx_string<> filename, vsxf* filesystem, bool mipmaps, bool flip_vertical)
  {
    if (verify_filesuffix(filename, "png"))
      return vsx_texture_data_loader_png::get_instance()->load(filename, filesystem, mipmaps, flip_vertical);

    if (verify_filesuffix(filename, "jpg"))
      return vsx_texture_data_loader_jpg::get_instance()->load(filename, filesystem, mipmaps, flip_vertical);

    if (verify_filesuffix(filename, "tga"))
      return vsx_texture_data_loader_tga::get_instance()->load(filename, filesystem, mipmaps, flip_vertical);

    VSX_ERROR_RETURN_V("Unknown filetype",0x0);
  }

  inline void destroy(vsx_texture* &texture)
  {
    if (!texture)
      VSX_ERROR_RETURN("Texture is null");

    if (!texture->texture_gl)
      VSX_ERROR_RETURN("Trying to destroy a texture without texture_gl");

    if (!texture->texture_data)
      VSX_ERROR_RETURN("Trying to destroy a texture without texture_data");

    if (!texture->texture_data->attached_to_cache)
      VSX_ERROR_RETURN("Trying to destroy an unconnected texture");

    if (!texture->filename.size())
      VSX_ERROR_RETURN("Trying to destroy a texture with no filename set");

    if (verify_filesuffix(texture->filename,"png"))
    {
      vsx_texture_data_loader_png::get_instance()->destroy(texture);
      return;
    }

    if (verify_filesuffix(texture->filename,"jpg"))
    {
      vsx_texture_data_loader_jpg::get_instance()->destroy(texture);
      return;
    }

    if (verify_filesuffix(texture->filename,"tga"))
    {
      vsx_texture_data_loader_tga::get_instance()->destroy(texture);
      return;
    }
  }
}

#endif
