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


#ifndef VSX_TEXTURE_H
#define VSX_TEXTURE_H
#include <vsx_platform.h>
#include <engine_graphics_dllimport.h>
#include <string/vsx_string.h>
#include <vsxg.h>
#include <vsx_bitmap.h>
#include <vsx_matrix.h>

#include "vsx_texture_data.h"
#include "vsx_texture_gl.h"
#include "vsx_texture_transform_base.h"

class vsx_texture
{

public:

  vsx_string<> filename;

  // transformation object
  vsx_texture_transform_base* transform_obj;
  vsx_texture_data* texture_data;
  vsx_texture_gl* texture_gl;


  void set_transform(vsx_texture_transform_base* new_transform_obj)
  {
    transform_obj = new_transform_obj;
  }

  vsx_texture_transform_base* get_transform()
  {
    return transform_obj;
  }

  inline bool is_valid()
  {
    if (!texture_gl)
      return false;

    if (!texture_gl->gl_id)
      return false;

    return true;
  }

  // use this to load/unload the texture from vram
  VSX_ENGINE_GRAPHICS_DLLIMPORT void upload_gl();
  VSX_ENGINE_GRAPHICS_DLLIMPORT void unload_gl();

  // use this to bind the texture.
  VSX_ENGINE_GRAPHICS_DLLIMPORT bool bind();

  // use this when you're done with the texture
  VSX_ENGINE_GRAPHICS_DLLIMPORT void _bind();

  vsx_texture(bool attached_to_cache = false)
    :
      transform_obj(0x0),
      texture_data(0x0),
      texture_gl(0x0)
  {
    if (!attached_to_cache)
    {
      texture_gl = new vsx_texture_gl(true);
      texture_data = new vsx_texture_data(0, true);
    }
  }

  ~vsx_texture()
  {
    if (texture_data && !texture_data->attached_to_cache)
      delete texture_data;

    if (texture_gl && !texture_gl->attached_to_cache)
      delete texture_gl;
  }
};

#include "vsx_texture_transform_helper.h"
#include "vsx_texture_data_loader_helper.h"

#endif
