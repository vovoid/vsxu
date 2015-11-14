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

class vsx_texture_gl
{
public:
  unsigned int gl_id;
  unsigned int gl_type;

  bool uploaded_to_gl;

  bool attached_to_cache; // not part of cache
  int references; // references in cache

  vsx_texture_gl(bool is_attached_to_cache)
    :
    gl_id(0),
    gl_type(0),
    uploaded_to_gl(false),
    attached_to_cache(is_attached_to_cache),
    references(0)
  {
  }

  void init_opengl_texture_1d()
  {
    glGenTextures(1, &gl_id);
    gl_type = GL_TEXTURE_1D;
  }

  void init_opengl_texture_2d()
  {
    glGenTextures(1, &gl_id);
    gl_type = GL_TEXTURE_2D;
  }

  void init_opengl_texture_cubemap()
  {
    glGenTextures(1, &gl_id);
    gl_type = GL_TEXTURE_CUBE_MAP;
  }

  void unload()
  {
    glDeleteTextures(1,&gl_id);

    gl_id = 0;
    uploaded_to_gl = false;
  }

};


#endif
