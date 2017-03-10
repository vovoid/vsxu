/**
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Lesser General Public License (LGPL)
*
* VSXu Engine is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU Lesser General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


#pragma once

#include <debug/vsx_error.h>

#ifndef VSX_NO_GL
  #include <vsx_gl_global.h>
#endif

class vsx_texture_gl
{
public:
  // loader hint
  enum loader_hint {
      no_hint = 0,
      generate_mipmaps_hint = 1,
      linear_interpolate_hint = 2,
      mipmap_linear_interpolate_hint = 4,
      anisotropic_filtering_hint = 8
    };

  uint64_t bitmap_loader_hint = 0;
  uint64_t hint = 0;

  vsx_bitmap* bitmap = 0x0;

  // handle and type
  unsigned int gl_id = 0;
  unsigned int gl_type = 0;

  bool uploaded_to_gl = false;
  size_t mip_map_levels_uploaded = 0;

  // cache markers
  bool attached_to_cache;
  int references = 0;

  vsx_texture_gl(bool is_attached_to_cache)
    :
    attached_to_cache(is_attached_to_cache)
  {
    req(!is_attached_to_cache);
    bitmap = new vsx_bitmap();
  }

  ~vsx_texture_gl()
  {
    unload();
    req(bitmap);
    req(!bitmap->attached_to_cache);
    delete bitmap;
  }

  void copy_from(vsx_texture_gl* other)
  {
    this->hint = other->hint;
    this->bitmap = other->bitmap;
    if (other->gl_id)
      this->gl_id = other->gl_id;
    this->gl_type = other->gl_type;

    this->gl_type = other->gl_type;
  }

  void init_opengl_texture_1d()
  {
    #ifndef VSX_NO_GL
    if (gl_id)
      VSX_ERROR_RETURN("Trying to re-initialize gl texture");
    glGenTextures(1, &gl_id);
    gl_type = GL_TEXTURE_1D;
    #endif
  }

  void init_opengl_texture_2d()
  {
    #ifndef VSX_NO_GL
    if (gl_id)
      VSX_ERROR_RETURN("Trying to re-initialize gl texture");

    glGenTextures(1, &gl_id);
    gl_type = GL_TEXTURE_2D;
    #endif
  }

  void init_opengl_texture_cubemap()
  {
    #ifndef VSX_NO_GL
    if (gl_id)
      VSX_ERROR_RETURN("Trying to re-initialize gl texture");

    glGenTextures(1, &gl_id);
    gl_type = GL_TEXTURE_CUBE_MAP;
    #endif
  }

  void unload()
  {
    #ifndef VSX_NO_GL
    if (!gl_id)
      return;

    glDeleteTextures(1,&gl_id);
    gl_id = 0;
    uploaded_to_gl = false;
    #endif
  }

};

#include "vsx_texture_gl_loader.h"
