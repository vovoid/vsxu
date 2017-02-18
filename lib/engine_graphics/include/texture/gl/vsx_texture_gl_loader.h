#pragma once

#include <bitmap/vsx_bitmap.h>
#include "vsx_texture_gl.h"
#include <bitmap/vsx_bitmap.h>
#include <bitmap/vsx_bitmap_transform.h>
#include <vsx_gl_global.h>

namespace vsx_texture_gl_loader
{

  inline GLenum get_compression_format(vsx_bitmap* bitmap)
  {
    if (bitmap->compression == vsx_bitmap::compression_dxt1)
      return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;

    if (bitmap->compression == vsx_bitmap::compression_dxt3)
      return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;

    if (bitmap->compression == vsx_bitmap::compression_dxt5)
      return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;

    return 0;
  }

/**
 * @brief upload_1d
 * @param texture_gl
 * @param data
 * @param size
 * @param mipmaps
 * @param bpp
 * @param bpp2
 */
inline void upload_1d(vsx_texture_gl* texture_gl, void* data, unsigned long size, bool mipmaps = false, int bpp = 4, int bpp2 = GL_BGRA_EXT)
{
  GLboolean oldStatus = glIsEnabled(texture_gl->gl_type);

  glEnable(texture_gl->gl_type);
  glBindTexture(texture_gl->gl_type, texture_gl->gl_id);

  if (mipmaps)
  {
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glTexParameteri(texture_gl->gl_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(texture_gl->gl_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(texture_gl->gl_type, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
  } else
  {
    glTexParameteri(texture_gl->gl_type, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(texture_gl->gl_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(texture_gl->gl_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }

  // no compression
  if (bpp == GL_RGBA32F_ARB)
  {
    glTexImage1D(
      texture_gl->gl_type,  // opengl type
      0, // mipmap level
      GL_RGBA32F_ARB, // internal format
      (int)size, // size
      0, // border
      bpp2, // source data format
      GL_FLOAT, // source data type
      data
    );
  } else
  {
    if (bpp == 3)
    {
      glTexImage1D(
        texture_gl->gl_type,  // opengl type
        0,  // mipmap level
        GL_COMPRESSED_RGB_ARB, // storage type
        (int)size, // size x
        0,      // border 0 or 1
        bpp2,   // source data format
        GL_UNSIGNED_BYTE, // source data type
        data // pointer to data
      );
    }
    else
    {
      glTexImage1D(
        texture_gl->gl_type,  // opengl type
        0,  // mipmap level
        GL_COMPRESSED_RGBA_ARB, // storage type
        (int)size, // size x
        0,      // border 0 or 1
        bpp2,   // source data format
        GL_UNSIGNED_BYTE, // source data type
        data // pointer to data
      );
    }
  }

  if(!oldStatus)
  {
    glDisable(texture_gl->gl_type);
  }
}

inline void handle_anisotropic_mip_map_min_mag(vsx_texture_gl* texture_gl)
{
  if (texture_gl->hint & vsx_texture_gl::anisotropic_filtering_hint)
  {
    float rMaxAniso;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &rMaxAniso);
    glTexParameterf( texture_gl->gl_type, GL_TEXTURE_MAX_ANISOTROPY_EXT, rMaxAniso);
  }


  // generated mipmaps - only if they do not already exist in the bitmap
  if (
     texture_gl->hint & vsx_texture_gl::generate_mipmaps_hint &&
      1 == texture_gl->bitmap->get_mipmap_level_count()
  )
    glTexParameteri(texture_gl->gl_type, GL_GENERATE_MIPMAP, GL_TRUE);

  // when bitmap has mip maps, set max level to that of the bitmaps'
  if (
    texture_gl->bitmap->get_mipmap_level_count() > 1
    ||
    !(texture_gl->hint & vsx_texture_gl::generate_mipmaps_hint)
  )
    glTexParameteri(texture_gl->gl_type, GL_TEXTURE_MAX_LEVEL, (GLint)texture_gl->bitmap->get_mipmap_level_count() - 1);


  // magnification filter
  glTexParameteri(texture_gl->gl_type, GL_TEXTURE_MAG_FILTER, texture_gl->hint & vsx_texture_gl::linear_interpolate_hint?GL_LINEAR:GL_NEAREST);


  // any kind of mip maps
  if ( !(texture_gl->hint & vsx_texture_gl::generate_mipmaps_hint || texture_gl->bitmap->get_mipmap_level_count() > 1) )
  {
    if (texture_gl->hint & vsx_texture_gl::linear_interpolate_hint && texture_gl->hint & vsx_texture_gl::mipmap_linear_interpolate_hint)
      glTexParameteri(texture_gl->gl_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    if (!(texture_gl->hint & vsx_texture_gl::linear_interpolate_hint) && texture_gl->hint & vsx_texture_gl::mipmap_linear_interpolate_hint)
      glTexParameteri(texture_gl->gl_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

    if ((texture_gl->hint & vsx_texture_gl::linear_interpolate_hint) && !(texture_gl->hint & vsx_texture_gl::mipmap_linear_interpolate_hint))
      glTexParameteri(texture_gl->gl_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

    if (!(texture_gl->hint & vsx_texture_gl::linear_interpolate_hint) && !(texture_gl->hint & vsx_texture_gl::mipmap_linear_interpolate_hint))
      glTexParameteri(texture_gl->gl_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    return;
  }

  // no mip maps at all
  glTexParameteri(texture_gl->gl_type, GL_TEXTURE_MIN_FILTER, texture_gl->hint & vsx_texture_gl::linear_interpolate_hint?GL_LINEAR:GL_NEAREST);

}

/**
 * @brief upload_2d
 * @param texture_gl
 * @param bitmap
 */
inline void upload_2d( vsx_texture_gl* texture_gl )
{
  GLboolean oldStatus = glIsEnabled(texture_gl->gl_type);

  glEnable(texture_gl->gl_type);
  glBindTexture(texture_gl->gl_type, texture_gl->gl_id);

  handle_anisotropic_mip_map_min_mag(texture_gl);

  vsx_bitmap* bitmap = texture_gl->bitmap;
  bitmap->lock.aquire();

  // source format
  GLenum source_format = 0;
  if (bitmap->channels == 3)
  {
    if (bitmap->channels_bgra)
      source_format = GL_BGR;
    else
      source_format = GL_RGB;
  }

  if (bitmap->channels == 4)
  {
    if (bitmap->channels_bgra)
      source_format = GL_BGRA;
    else
      source_format = GL_RGBA;
  }

  // source type
  GLenum source_type = 0;
  if (bitmap->storage_format == vsx_bitmap::byte_storage)
    source_type = GL_UNSIGNED_BYTE;

  if (bitmap->storage_format == vsx_bitmap::float_storage)
    source_type = GL_FLOAT;

  // target format
  GLint target_format = 0;
  if (bitmap->channels == 3)
    target_format = GL_RGB;

  if (bitmap->channels == 4)
    target_format = GL_RGBA; // GL_COMPRESSED_RGB_ARB

  texture_gl->mip_map_levels_uploaded = 0;
  for (size_t mip_map_level = 0; mip_map_level < vsx_bitmap::mip_map_level_max; mip_map_level++)
  {
    if (!bitmap->data_get(mip_map_level,0))
      break;


    if (get_compression_format(bitmap))
    {
      glCompressedTexImage2D(
        texture_gl->gl_type,  // opengl type
        (GLint)mip_map_level, // mipmap level
        get_compression_format(bitmap),
        bitmap->width >> mip_map_level,
        bitmap->height >> mip_map_level,
        0, // border 0 or 1
        (GLsizei)bitmap->data_size_get(mip_map_level, 0),
        bitmap->data_get(mip_map_level, 0)
      );
      texture_gl->mip_map_levels_uploaded++;
      continue;
    }

    if ((bitmap->width * bitmap->channels) % 4)
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(
      texture_gl->gl_type, // opengl type
      (GLint)mip_map_level, // mipmap level
      target_format, // storage type
      bitmap->width >> mip_map_level,
      bitmap->height >> mip_map_level,
      0, // border 0 or 1
      source_format, // source data format
      source_type, // source data type
      bitmap->data_get(mip_map_level, 0) // pointer to data
    );

    if ((bitmap->width * bitmap->channels) % 4)
      glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    texture_gl->mip_map_levels_uploaded++;
  }

  bitmap->lock.release();

  if(!oldStatus)
    glDisable(texture_gl->gl_type);

  texture_gl->uploaded_to_gl = true;
}


/**
 * @brief upload_cube
 * @param texture_gl
 * @param bitmap
 */
inline void upload_cube( vsx_texture_gl* texture_gl )
{
  glEnable(texture_gl->gl_type);
  glBindTexture(texture_gl->gl_type, texture_gl->gl_id);

  handle_anisotropic_mip_map_min_mag(texture_gl);

  vsx_bitmap* bitmap = texture_gl->bitmap;

  // source format
  GLenum source_format = 0;
  if (bitmap->channels == 3)
    source_format = GL_RGB;

  if (bitmap->channels == 4)
    source_format = GL_RGBA;

  if (source_format == 0)
    VSX_ERROR_RETURN("Source format not set");

  // source type
  GLenum source_type = 0;
  if (bitmap->storage_format == vsx_bitmap::byte_storage)
    source_type = GL_UNSIGNED_BYTE;

  if (bitmap->storage_format == vsx_bitmap::float_storage)
    source_type = GL_FLOAT;

  if (source_type == 0)
    VSX_ERROR_RETURN("Source type not set");

  // target format
  GLint target_format = 0;
  if (bitmap->channels == 3)
    target_format = GL_RGB;

  if (bitmap->channels == 4)
    target_format = GL_RGBA; // GL_COMPRESSED_RGB_ARB

  if (target_format == 0)
    VSX_ERROR_RETURN("Target format not set");

  GLenum sides[] = {
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB,
    GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB
  };

  texture_gl->mip_map_levels_uploaded = 0;
  for (size_t mip_map_level = 0; mip_map_level < vsx_bitmap::mip_map_level_max; mip_map_level++)
  {
    if (!bitmap->data_get(mip_map_level, 0 ) )
      break;

    texture_gl->mip_map_levels_uploaded++;

    if (get_compression_format(bitmap))
    {
      for (size_t cube_map_side = 0; cube_map_side < 6; cube_map_side++)
        glCompressedTexImage2D(
          sides[cube_map_side],  // opengl target
          (GLint)mip_map_level, // mipmap level
          get_compression_format(bitmap),
          bitmap->width >> mip_map_level,
          bitmap->height >> mip_map_level,
          0, // border 0 or 1
          (GLsizei)bitmap->data_size_get(mip_map_level, cube_map_side),
          bitmap->data_get(mip_map_level, cube_map_side)
        );
      // next mip map level
      continue;
    }

    for (size_t cube_map_side = 0; cube_map_side < 6; cube_map_side++)
      glTexImage2D(
        sides[cube_map_side],  // opengl target
        (GLint)mip_map_level,  // mipmap level
        target_format, // storage type
        bitmap->height, // size x
        bitmap->height, // size y
        0,      // border 0 or 1
        source_format,   // source data format
        source_type, // source data type
        bitmap->data_get(mip_map_level, cube_map_side) // pointer to data
      );
  }

  glDisable( texture_gl->gl_type );
  texture_gl->uploaded_to_gl = true;
}

inline void upload_bitmap_2d(vsx_texture_gl* texture_gl, vsx_bitmap* bitmap, bool flip_vertical)
{
  texture_gl->bitmap = bitmap;

  if (flip_vertical)
    vsx_bitmap_transform::get_instance()->flip_vertically(bitmap);

  upload_2d( texture_gl );
}


}
