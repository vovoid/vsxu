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


class module_texture_to_bitmap : public vsx_module
{
public:

  // in
  vsx_module_param_texture* texture_in;

  // out
  vsx_module_param_bitmap* result1;

  // internal
  vsx_bitmap bitm;
  int bitm_timestamp;

  vsx_texture** texture;
  int p_updates;


  void module_info(vsx_module_info* info)
  {
    info->in_param_spec = "texture_in:texture";
    info->identifier = "texture;loaders;texture2bitmap";
    info->out_param_spec = "bitmap:bitmap";
    info->component_class = "bitmap";
    info->description = "transforms a texture into a bitmap (slow!)";
  }

  void param_set_notify(const vsx_string& name)
  {
    VSX_UNUSED(name);
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    bitm.bpp = 4;
    bitm.bformat = GL_RGBA;
    bitm.size_x = 0;
    bitm.size_y = 0;
    bitm.data = 0;
    bitm.valid = false;
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    texture_in = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture_in");

    //--------------------------------------------------------------------------------------------------

    result1 = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
    result1->set_p(bitm);
  }

  void run()
  {
    texture = texture_in->get_addr();

    if (!texture)
      return;

    (*texture)->bind();
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
        if (bitm.size_x != (unsigned int)width || bitm.size_y != (unsigned int)height)
        {

          if (bitm.data)
            delete[] (vsx_bitmap_32bt*)bitm.data;
          bitm.data = new vsx_bitmap_32bt[width*height];
          bitm.size_x = width;
          bitm.size_y = height;
        }
        glGetTexImage(GL_TEXTURE_2D,
                   0,
                   GL_RGBA,
                   GL_UNSIGNED_BYTE,
                   bitm.data);
        bitm.valid = true;
        ++bitm.timestamp;
        result1->set_p(bitm);
      }
    (*texture)->_bind();
  }

  void on_delete()
  {
    if (bitm.valid)
    {
      delete[] (vsx_bitmap_32bt*)bitm.data;
    }
  }

};

