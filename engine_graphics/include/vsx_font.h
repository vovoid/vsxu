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


#ifndef VSX_FONT_H
#define VSX_FONT_H

#include <vsx_platform.h>
#include <engine_graphics_dllimport.h>

#include <texture/vsx_texture.h>
#include <vsx_color.h>

#include <vsx_platform.h>

class vsx_font_info {
public:
  int type; // 0 = texture (old) 1 = new (FtGL)
  vsx_texture<>* texture;  // pointer to either vsx_texture or ftgl font
  vsx_string<>name;
  void* ftfont;
  void* ftfont_outline;
  vsx_font_info()
  :
  type(0),
  texture(0x0),
  ftfont(0x0),
  ftfont_outline(0x0)
  {}

  ~vsx_font_info()
  {
  }
};  

class vsx_font {
  vsx_vector3<> ep;
  void reinit(vsx_font_info* f_info, vsx_string<>font, vsx_filesystem::filesystem* filesystem);
  bool list_built;
  GLuint dlist;


  float dx, dy, dz;
  vsx_string<>base_path;
  float ch, cw, size_s;
  float ddx;
  char* stc;
  char* colc;
  float sx,sy,ex,ey;

  void init_vars()
  {
    list_built = false;
    color.r = 1;
    color.g = 1;
    color.b = 1;
    color.a = 1;
    align = 1.0f;
  }
public:
  VSX_ENGINE_GRAPHICS_DLLIMPORT void load(vsx_string<>font, vsx_filesystem::filesystem* filesystem);
  VSX_ENGINE_GRAPHICS_DLLIMPORT void unload();

  vsx_nw_vector< vsx_color<> > syntax_colors;
  // is in 2d mode?
  // keeping track of our font
  vsx_font_info my_font_info;
  float outline_transparency;
  // is it translucent or not?
  vsx_color<> color;
  float align;
  
  
  VSX_ENGINE_GRAPHICS_DLLIMPORT vsx_vector3<> print(vsx_vector3<> p, const vsx_string<>& str, const float size, const vsx_string<>colors = "");
  VSX_ENGINE_GRAPHICS_DLLIMPORT vsx_vector3<> print(vsx_vector3<> p, const vsx_string<>& str, const vsx_string<>& font, float size, const vsx_string<>colors = "");
  VSX_ENGINE_GRAPHICS_DLLIMPORT vsx_vector3<> print_center(vsx_vector3<> p, const vsx_string<>& str, float size);
  VSX_ENGINE_GRAPHICS_DLLIMPORT vsx_vector3<> print_right(vsx_vector3<> p, const vsx_string<>& str, float size);
  VSX_ENGINE_GRAPHICS_DLLIMPORT vsx_vector3<> get_size(const vsx_string<>& str, float size);
  
  vsx_font(vsx_string<>path)
  {
    base_path = path;
    init_vars();
  }

  vsx_font()
  {
    init_vars();
  }
};

#endif
