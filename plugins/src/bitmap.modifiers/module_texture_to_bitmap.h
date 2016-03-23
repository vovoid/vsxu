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

#include <bitmap/vsx_bitmap.h>

class module_texture_to_bitmap : public vsx_module
{
public:

  // in
  vsx_module_param_texture* texture_in;

  // out
  vsx_module_param_bitmap* bitmap_out;

  // internal
  vsx_bitmap bitmap;
  int bitm_timestamp;

  vsx_texture<>* texture;
  int p_updates;


  void module_info(vsx_module_specification* info)
  {
    info->in_param_spec = "texture_in:texture";
    info->identifier = "texture;loaders;texture2bitmap";
    info->out_param_spec = "bitmap:bitmap";
    info->component_class = "bitmap";
    info->description = "transforms a texture into a bitmap (slow!)";
  }

  void param_set_notify(const vsx_string<>& name)
  {
    VSX_UNUSED(name);
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    texture_in = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture_in");
    bitmap_out = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
  }

  void run()
  {
    texture = texture_in->get();

    if (!texture)
      return;

    texture->bind();
      GLint components;
      glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_COMPONENTS,&components);
      GLint pack;
      glGetIntegerv(GL_PACK_ALIGNMENT,&pack);
      if (pack == 4)
      {
        GLint width;
        GLint height;
        glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&height);
        if (bitmap.width != (unsigned int)width || bitmap.height != (unsigned int)height)
        {
          bitmap.data_free();
          bitmap.data_set( malloc( sizeof(uint32_t) * width * height) );
          bitmap.width = width;
          bitmap.height = height;
        }
        glGetTexImage(GL_TEXTURE_2D,
                   0,
                   GL_RGBA,
                   GL_UNSIGNED_BYTE,
                   bitmap.data_get()
        );
        ++bitmap.timestamp;
        bitmap_out->set(&bitmap);
      }
    texture->_bind();
  }
};

