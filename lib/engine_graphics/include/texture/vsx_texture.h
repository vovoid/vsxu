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

#include <vsx_platform.h>
#include <engine_graphics_dllimport.h>
#include <string/vsx_string.h>
#include <bitmap/vsx_bitmap.h>
#include <math/vsx_matrix.h>
#include <tools/vsx_req.h>

#include "gl/vsx_texture_gl.h"
#include "transform/vsx_texture_transform_base.h"
#include <texture/gl/vsx_texture_gl_cache.h>

template <class T = vsx_texture_gl>
class vsx_texture
{

public:

  // transformation object
  vsx_texture_transform_base* transform_obj = 0x0;
  T* texture = 0x0;


  void set_transform(vsx_texture_transform_base* new_transform_obj)
  {
    transform_obj = new_transform_obj;
  }

  vsx_texture_transform_base* get_transform()
  {
    return transform_obj;
  }

  // use this to load/unload the texture from vram
  void upload_gl()
  {
    req(texture);
    req(!texture->uploaded_to_gl);
    req(texture->bitmap);
    req(texture->bitmap->data_ready.load());

    texture->unload();

    if (texture->bitmap->hint & vsx_bitmap::cubemap_split_6_1_hint || texture->bitmap->hint & vsx_bitmap::cubemap_load_files_hint)
      if (texture->bitmap->sides_count_get() == 6)
    {
      texture->init_opengl_texture_cubemap();
      vsx_texture_gl_loader::upload_cube(texture);
      return;
    }

    texture->init_opengl_texture_2d();
    vsx_texture_gl_loader::upload_2d(texture);
  }

  void unload_gl()
  {
    if (!texture)
      VSX_ERROR_RETURN("texture_gl invalid");

    if (texture->attached_to_cache)
      VSX_ERROR_RETURN("Trying to unload a texture which is not local");

    texture->unload();
  }

  // use this to bind the texture.
  bool bind()
  {
    if (!texture)
      return false;

    upload_gl();

    if (!texture->gl_id)
      return false;

    if (texture->gl_type == GL_TEXTURE_2D_MULTISAMPLE)
    {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D,texture->gl_id);
      return true;
    }

    glEnable(texture->gl_type);
    glBindTexture(texture->gl_type, texture->gl_id);
    return true;
  }

  // use this when you're done with the texture
  void _bind()
  {
    if (!texture)
      return;

    if (!texture->gl_id)
      return;

    if (texture->gl_type == GL_TEXTURE_2D_MULTISAMPLE)
    {
      glDisable(GL_TEXTURE_2D);
      return;
    }

    glDisable(texture->gl_type);
  }


  inline vsx_texture<T>& operator=(vsx_texture<T>&& other)
  {
    texture = other.texture;
    transform_obj = other.transform_obj;
    other.texture = 0x0;
    other.transform_obj = 0x0;
    return *this;
  }

  vsx_texture(vsx_texture<T>&& other)
  {
    texture = other.texture;
    transform_obj = other.transform_obj;
    other.texture = 0x0;
    other.transform_obj = 0x0;
  }


  vsx_texture(bool attached_to_cache = false)
  {
    if (!attached_to_cache)
      texture = new T(false);
  }

  ~vsx_texture()
  {
    if (texture && !texture->attached_to_cache)
    {
      delete texture;
      return;
    }

//    if (texture && texture->attached_to_cache)
//      vsx_texture_gl_cache::get_instance()->destroy( texture );
  }
};

#include "transform/vsx_texture_transform_helper.h"
#include "vsx_texture_loader.h"
