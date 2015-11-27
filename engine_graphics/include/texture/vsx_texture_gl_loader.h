#ifndef VSX_TEXTURE_GL_LOADER_H
#define VSX_TEXTURE_GL_LOADER_H

#include <vsx_bitmap.h>
#include "vsx_texture_gl.h"
#include "vsx_texture_data.h"
#include "vsx_texture_data_transform.h"
#include <vsx_gl_global.h>

namespace vsx_texture_gl_loader
{

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
      size, // size
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
        size, // size x
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
        size, // size x
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

/**
 * @brief upload_2d
 * @param texture_gl
 * @param texture_data
 */
inline void upload_2d( vsx_texture_gl* texture_gl, vsx_texture_data* texture_data )
{
  GLboolean oldStatus = glIsEnabled(texture_gl->gl_type);

  glEnable(texture_gl->gl_type);
  glBindTexture(texture_gl->gl_type, texture_gl->gl_id);

  if (texture_data->mipmaps)
  {
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glTexParameteri(texture_gl->gl_type, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
    glTexParameteri(texture_gl->gl_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(texture_gl->gl_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    float rMaxAniso;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &rMaxAniso);
    glTexParameterf( texture_gl->gl_type, GL_TEXTURE_MAX_ANISOTROPY_EXT, rMaxAniso);

  } else
  {
    glTexParameteri(texture_gl->gl_type, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(texture_gl->gl_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(texture_gl->gl_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }

  // source format
  GLenum source_format = 0;
  if (texture_data->channels == 3)
    source_format = GL_RGB;

  if (texture_data->channels == 4)
    source_format = GL_RGBA;

  // source type
  GLenum source_type = 0;
  if (texture_data->storage_format == vsx_texture_data::byte_storage)
    source_type = GL_UNSIGNED_BYTE;

  if (texture_data->storage_format == vsx_texture_data::float_storage)
    source_type = GL_FLOAT;

  // target format
  GLint target_format = 0;
  if (texture_data->channels == 3)
    target_format = GL_RGB;

  if (texture_data->channels == 4)
    target_format = GL_RGBA; // GL_COMPRESSED_RGB_ARB

  // look for GL_ARB_texture_view if dxt5 compression is available
  glTexImage2D(
    texture_gl->gl_type,  // opengl type
    0,  // mipmap level
    target_format, // storage type
    texture_data->width,
    texture_data->height,
    0,      // border 0 or 1
    source_format,   // source data format
    source_type, // source data type
    texture_data->data[0] // pointer to data
  );

  if(!oldStatus)
    glDisable(texture_gl->gl_type);

  texture_gl->uploaded_to_gl = true;
}


/**
 * @brief upload_cube
 * @param texture_gl
 * @param texture_data
 */
inline void upload_cube(vsx_texture_gl* texture_gl, vsx_texture_data* texture_data )
{
  glEnable(texture_gl->gl_type);
  glBindTexture(texture_gl->gl_type, texture_gl->gl_id);

  if (texture_data->mipmaps)
  {
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glTexParameteri(texture_gl->gl_type, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
    glTexParameteri(texture_gl->gl_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(texture_gl->gl_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    float rMaxAniso;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &rMaxAniso);
    glTexParameterf( texture_gl->gl_type, GL_TEXTURE_MAX_ANISOTROPY_EXT, rMaxAniso);

  } else
  {
    glTexParameteri(texture_gl->gl_type, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(texture_gl->gl_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(texture_gl->gl_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }

  // source format
  GLenum source_format = 0;
  if (texture_data->channels == 3)
    source_format = GL_RGB;

  if (texture_data->channels == 4)
    source_format = GL_RGBA;

  if (source_format == 0)
    VSX_ERROR_RETURN("Source format not set");

  // source type
  GLenum source_type = 0;
  if (texture_data->storage_format == vsx_texture_data::byte_storage)
    source_type = GL_UNSIGNED_BYTE;

  if (texture_data->storage_format == vsx_texture_data::float_storage)
    source_type = GL_FLOAT;

  if (source_type == 0)
    VSX_ERROR_RETURN("Source type not set");

  // target format
  GLint target_format = 0;
  if (texture_data->channels == 3)
    target_format = GL_RGB;

  if (texture_data->channels == 4)
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

  for (size_t i = 0; i < 6; i++)
    glTexImage2D(
      sides[i],  // opengl target
      0,  // mipmap level
      target_format, // storage type
      texture_data->height, // size x
      texture_data->height, // size y
      0,      // border 0 or 1
      source_format,   // source data format
      source_type, // source data type
      texture_data->data[i] // pointer to data
    );

  glDisable( texture_gl->gl_type );
  texture_gl->uploaded_to_gl = true;
}

inline void upload_bitmap_2d(vsx_texture_gl* texture_gl, vsx_bitmap* bitmap, bool mipmaps, bool flip_vertical)
{
  vsx_texture_data texture_data(VSX_TEXTURE_DATA_TYPE_2D, false);
  texture_data.data[0] = bitmap->data;
  texture_data.channels = bitmap->channels;
  texture_data.width = bitmap->width;
  texture_data.height = bitmap->height;
  texture_data.mipmaps = mipmaps;

  if (bitmap->storage_format == vsx_bitmap::byte_storage)
    texture_data.storage_format = vsx_texture_data::byte_storage;

  if (bitmap->storage_format == vsx_bitmap::float_storage)
    texture_data.storage_format = vsx_texture_data::float_storage;

  if (flip_vertical && !bitmap->flipped_vertically)
  {
    vsx_texture_data_transform::get_instance()->flip_vertically(&texture_data);
    bitmap->data = texture_data.data[0];
    bitmap->flipped_vertically = true;
  }

  upload_2d(
    texture_gl,
    &texture_data
  );

  // ensure that ~vsx_texture_data()
  // does not free, memory is owned by bitmap
  texture_data.data[0] = 0;
}


}

#endif
