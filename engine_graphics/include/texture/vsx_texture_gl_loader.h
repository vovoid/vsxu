#ifndef VSX_TEXTURE_GL_LOADER_H
#define VSX_TEXTURE_GL_LOADER_H

#include "vsx_texture_gl.h"

namespace vsx_texture_gl_loader
{
// upload a bitmap from RAM to the GPU
// as an openGL texture. requires that init_opengl_texture
// has been run.
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


inline void upload_2d(vsx_texture_gl* texture_gl, void* data, unsigned long size_x, unsigned long size_y,bool mipmaps = false, int bpp = 4, int bpp2 = GL_BGRA_EXT, bool upside_down = true)
{
  // prepare data
  if (upside_down)
  {
    if (bpp == GL_RGBA32F_ARB)
    {
      GLfloat* data2 = new GLfloat[size_x * size_y * 4];
      int dy = 0;
      int sxbpp = size_x*4;
      for (int y = size_y-1; y >= 0; --y) {
        for (unsigned long x = 0; x < size_x*4; ++x) {
          data2[dy*sxbpp + x] = ((GLfloat*)data)[y*sxbpp + x];
        }
        ++dy;
      }
      data = (void*)data2;
    }
    else
    {
      unsigned char* data2 = new unsigned char[(size_x) * (size_y) * (bpp)];
      int dy = 0;
      int sxbpp = size_x*bpp;
      for (int y = size_y-1; y >= 0; --y)
      {
        //printf("y: %d\n",y);
        int dysxbpp = dy*sxbpp;
        int ysxbpp = y * sxbpp;
        for (size_t x = 0; x < size_x*bpp; ++x)
        {
          data2[dysxbpp + x] = ((unsigned char*)data)[ysxbpp + x];
        }
        ++dy;
      }
      data = (void*)data2;
    }
  }




  GLboolean oldStatus = glIsEnabled(texture_gl->gl_type);

  glEnable(texture_gl->gl_type);
  glBindTexture(texture_gl->gl_type, texture_gl->gl_id);

  if (mipmaps)
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

  // no compression
  if (bpp == GL_RGBA32F_ARB)
  {
    glTexImage2D(texture_gl->gl_type, 0, bpp , size_x, size_y, 0, bpp2, GL_FLOAT, data);
  } else
  {
    if (bpp == 3)
    {
      glTexImage2D(
        texture_gl->gl_type,  // opengl type
        0,  // mipmap level
//        GL_RGB, // storage type
        GL_COMPRESSED_RGB_ARB, // storage type
        size_x, // size x
        size_y, // size y
        0,      // border 0 or 1
        bpp2,   // source data format
        GL_UNSIGNED_BYTE, // source data type
        data // pointer to data
      );
    }
    else
    {
      glTexImage2D(
        texture_gl->gl_type,  // opengl type
        0,  // mipmap level
        GL_COMPRESSED_RGBA_ARB, // storage type
//        GL_RGBA, // storage type
        size_x, // size x
        size_y, // size y
        0,      // border 0 or 1
        bpp2,   // source data format
        GL_UNSIGNED_BYTE, // source data type
        data // pointer to data
      );
    }
    // original:
    //       glTexImage2D(texture_info->ogl_type, 0,bpp , size_x, size_y, 0, bpp2, GL_UNSIGNED_BYTE, data);
  }

  if (upside_down)
  {
    if (bpp == GL_RGBA32F_ARB)
    {
      delete[] (GLfloat*)data;
    } else
    {
      delete[] (unsigned char*)data;
    }
  }

  if(!oldStatus)
    glDisable(texture_gl->gl_type);

  texture_gl->uploaded_to_gl = true;
}


inline void upload_bitmap_2d(vsx_texture_gl* texture_gl, vsx_bitmap* vbitmap, bool mipmaps = false, bool upside_down = true)
{
  upload_2d(
    texture_gl,
    vbitmap->data,
    vbitmap->size_x,
    vbitmap->size_y,
    mipmaps,
    vbitmap->bpp,
    vbitmap->bformat,
    upside_down
  );
}

// assumes width is 6x height (maps in order: -x, z, x, -z, -y, y
inline void upload_cube(vsx_texture_gl* texture_gl, void* data, unsigned long size_x, unsigned long size_y,bool mipmaps = false, int bpp = 4, int bpp2 = GL_BGRA_EXT, bool upside_down = true)
{
  {
    VSX_UNUSED(mipmaps);
    VSX_UNUSED(upside_down);

    if ( size_x / 6 != size_y )
    {
      vsx_printf(L"vsx_texture::upload_ram_bitmap_cube Error: not cubemap, should be aspect 6:1");
      return;
    }

    if (bpp == 3)
    {
      vsx_printf(L"RGB cubemaps not implemented\n");
      return;
    }

    uint32_t* sides[6];

    if (bpp2 == GL_RGBA32F_ARB)
    {
      // TODO: not correctly implemented
      // split cubemap into 6 individual bitmaps

      sides[0] = (uint32_t*)malloc( sizeof(GLfloat) * (size_y * size_y) );
      sides[1] = (uint32_t*)malloc( sizeof(GLfloat) * (size_y * size_y) );
      sides[2] = (uint32_t*)malloc( sizeof(GLfloat) * (size_y * size_y) );
      sides[3] = (uint32_t*)malloc( sizeof(GLfloat) * (size_y * size_y) );
      sides[4] = (uint32_t*)malloc( sizeof(GLfloat) * (size_y * size_y) );
      sides[5] = (uint32_t*)malloc( sizeof(GLfloat) * (size_y * size_y) );

      for (size_t side_offset = 0; side_offset < 6; side_offset++)
      {
        for (size_t y = 0; y < size_y; y++)
        {
          memcpy(
            // destination
            ((GLfloat*)sides[side_offset]) + y * size_y
            ,

            // souce
            (GLfloat*)&((GLfloat*)data)[ size_x * y ] // row offset
            +
            size_y * side_offset,            // horiz offset

            sizeof(GLfloat) * size_y // count
          );
        }
      }

    } else
    {
      // split cubemap into 6 individual bitmaps

      sides[0] = (uint32_t*)malloc( sizeof(uint32_t) * (size_y * size_y) );
      sides[1] = (uint32_t*)malloc( sizeof(uint32_t) * (size_y * size_y) );
      sides[2] = (uint32_t*)malloc( sizeof(uint32_t) * (size_y * size_y) );
      sides[3] = (uint32_t*)malloc( sizeof(uint32_t) * (size_y * size_y) );
      sides[4] = (uint32_t*)malloc( sizeof(uint32_t) * (size_y * size_y) );
      sides[5] = (uint32_t*)malloc( sizeof(uint32_t) * (size_y * size_y) );

      uint32_t* source_begin = (uint32_t*)data;
      for (size_t side_offset = 0; side_offset < 6; side_offset++)
      {
        for (size_t y = 0; y < size_y; y++)
        {
          memcpy(
            // destination
            sides[side_offset] + y * size_y
            ,

            // source
            source_begin // row offset
            + size_x * y
            +
            size_y * side_offset,            // horiz offset

            sizeof(uint32_t) * size_y // count
          );
        }
      }
    }


    glEnable(texture_gl->gl_type);
    glBindTexture(texture_gl->gl_type, texture_gl->gl_id);

    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glTexParameteri(texture_gl->gl_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(texture_gl->gl_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(texture_gl->gl_type, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
    float rMaxAniso;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &rMaxAniso);
    glTexParameterf( texture_gl->gl_type, GL_TEXTURE_MAX_ANISOTROPY_EXT, rMaxAniso);


    glTexImage2D(
      GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB,  // opengl target
      0,  // mipmap level
      //GL_COMPRESSED_RGBA_ARB, // storage type
      bpp2, // storage type
      size_y, // size x
      size_y, // size y
      0,      // border 0 or 1
      bpp2,   // source data format
      GL_UNSIGNED_BYTE, // source data type
      sides[0] // pointer to data
    );

    glTexImage2D(
      GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB,  // opengl target
      0,  // mipmap level
      //GL_COMPRESSED_RGBA_ARB, // storage type
      bpp2, // storage type
      size_y, // size x
      size_y, // size y
      0,      // border 0 or 1
      bpp2,   // source data format
      GL_UNSIGNED_BYTE, // source data type
      sides[1] // pointer to data
    );

    glTexImage2D(
      GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,  // opengl target
      0,  // mipmap level
      //GL_COMPRESSED_RGBA_ARB, // storage type
      bpp2, // storage type
      size_y, // size x
      size_y, // size y
      0,      // border 0 or 1
      bpp2,   // source data format
      GL_UNSIGNED_BYTE, // source data type
      sides[2] // pointer to data
    );

    glTexImage2D(
      GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB,  // opengl target
      0,  // mipmap level
      //GL_COMPRESSED_RGBA_ARB, // storage type
      bpp2, // storage type
      size_y, // size x
      size_y, // size y
      0,      // border 0 or 1
      bpp2,   // source data format
      GL_UNSIGNED_BYTE, // source data type
      sides[3] // pointer to data
    );

    glTexImage2D(
      GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB,  // opengl target
      0,  // mipmap level
      //GL_COMPRESSED_RGBA_ARB, // storage type
      bpp2, // storage type
      size_y, // size x
      size_y, // size y
      0,      // border 0 or 1
      bpp2,   // source data format
      GL_UNSIGNED_BYTE, // source data type
      sides[4] // pointer to data
    );

    glTexImage2D(
      GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB,  // opengl target
      0,  // mipmap level
      //GL_COMPRESSED_RGBA_ARB, // storage type
      bpp2, // storage type
      size_y, // size x
      size_y, // size y
      0,      // border 0 or 1
      bpp2,   // source data format
      GL_UNSIGNED_BYTE, // source data type
      sides[5] // pointer to data
    );

    glDisable( texture_gl->gl_type );

    // free our temporary local storage
    free(sides[0]);
    free(sides[1]);
    free(sides[2]);
    free(sides[3]);
    free(sides[4]);
    free(sides[5]);

    texture_gl->uploaded_to_gl = true;
  }
}


}

#endif
