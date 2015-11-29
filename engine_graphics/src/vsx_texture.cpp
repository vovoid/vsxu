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


#include <vsxfst.h>
#include <vsx_gl_global.h>
#include <texture/vsx_texture.h>
#ifndef VSX_TEXTURE_NO_GLPNG
  #include <vsxg.h>
  #include <stdlib.h>
  #ifdef _WIN32
    #include <pthread.h>
  #else
    #include <pthread.h>
  #endif
#endif
#include <debug/vsx_error.h>
#include <vsx_gl_state.h>

void vsx_texture::upload_gl()
{
  if (!texture_gl)
    return;

  // already uploaded
  if (texture_gl->uploaded_to_gl)
    return;

  // Data uploaded elsewhere
  if (!texture_gl->texture_data)
    return;

  if (! __sync_fetch_and_add(&(texture_gl->texture_data->data_ready), 0))
    return;

  texture_gl->unload();

  if (texture_gl->texture_data->hint.split_cubemap)
  {
    texture_gl->init_opengl_texture_cubemap();
    vsx_texture_gl_loader::upload_cube(texture_gl);
    return;
  }

  texture_gl->init_opengl_texture_2d();
  vsx_texture_gl_loader::upload_2d(texture_gl);
}

void vsx_texture::unload_gl()
{
  if (!texture_gl)
    VSX_ERROR_RETURN("texture_gl invalid");

  if (texture_gl->attached_to_cache)
    VSX_ERROR_RETURN("Trying to unload a texture which is not local");

  texture_gl->unload();
}


bool vsx_texture::bind()
{
  if (!texture_gl)
    return false;

  upload_gl();

  if (!texture_gl->gl_id)
    return false;

  if (texture_gl->gl_type == GL_TEXTURE_2D_MULTISAMPLE)
  {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texture_gl->gl_id);
    return true;
  }

  glEnable(texture_gl->gl_type);
  glBindTexture(texture_gl->gl_type, texture_gl->gl_id);
  return true;
}


void vsx_texture::_bind()
{
  if (!texture_gl)
    return;

  if (!texture_gl->gl_id)
    return;

  if (texture_gl->gl_type == GL_TEXTURE_2D_MULTISAMPLE)
  {
    glDisable(GL_TEXTURE_2D);
    return;
  }

  glDisable(texture_gl->gl_type);
}


