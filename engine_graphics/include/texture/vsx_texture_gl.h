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


#ifndef VSX_TEXTURE_GL_H
#define VSX_TEXTURE_GL_H

#include <debug/vsx_error.h>
#include "vsx_texture_gl_hint.h"

class vsx_texture_gl
{
public:
  vsx_texture_gl_hint hint;
  vsx_texture_data* texture_data;

  // handle and type
  unsigned int gl_id = 0;
  unsigned int gl_type = 0;

  bool uploaded_to_gl = false;

  // cache markers
  bool attached_to_cache;
  int references = 0;

  vsx_texture_gl(bool is_attached_to_cache)
    :
    attached_to_cache(is_attached_to_cache)
  {
    if (!attached_to_cache)
      texture_data = new vsx_texture_data(false);
  }

  void init_opengl_texture_1d()
  {
    if (gl_id)
      VSX_ERROR_RETURN("Trying to initialize an already initialized gl_id");

    glGenTextures(1, &gl_id);
    gl_type = GL_TEXTURE_1D;
  }

  void init_opengl_texture_2d()
  {
    if (gl_id)
      VSX_ERROR_RETURN("Trying to initialize an already initialized gl_id");

    glGenTextures(1, &gl_id);
    gl_type = GL_TEXTURE_2D;
  }

  void init_opengl_texture_cubemap()
  {
    if (gl_id)
      VSX_ERROR_RETURN("Trying to initialize an already initialized gl_id");

    glGenTextures(1, &gl_id);
    gl_type = GL_TEXTURE_CUBE_MAP;
  }

  void unload()
  {
    if (!gl_id)
      VSX_ERROR_RETURN("Trying to unload an invalid handle");

    glDeleteTextures(1,&gl_id);
    gl_id = 0;
    uploaded_to_gl = false;
  }

};

#include "texture/vsx_texture_gl_loader.h"

#endif
