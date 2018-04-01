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
#include <color/vsx_color.h>
#include <math/vector/vsx_vector2.h>
#include <math/vector/vsx_vector3.h>

#include <vsx_platform.h>

//#define VSX_FONT_ "\x01\x00\x00\x00\xff"
#define VSX_FONT_C_TEX "\x01"
#define VSX_FONT_C_OUTLINE "\x01"
#define VSX_FONT_C_BLACK "\x01\x01\x01\xff"
#define VSX_FONT_C_BLUE "\x01\x01\xaa\xff"
#define VSX_FONT_C_GREEN "\x01\xaa\x01\xff"
#define VSX_FONT_C_CYAN "\x01\xaa\xaa\xff"
#define VSX_FONT_C_RED "\xaa\x01\x01\xff"
#define VSX_FONT_C_MAGENTA "\xaa\x01\xaa\xff"
#define VSX_FONT_C_BROWN "\xaa\x55\x01\xff"
#define VSX_FONT_C_GRAY "\xaa\xaa\xaa\xff"
#define VSX_FONT_C_DARK_GRAY "\x55\x55\x55\xff"
#define VSX_FONT_C_BRIGHT_BLUE "\x55\x55\xff\xff"
#define VSX_FONT_C_BRIGHT_GREEN "\x55\xff\x55\xff"
#define VSX_FONT_C_BRIGHT_CYAN "\x55\xff\xff\xff"
#define VSX_FONT_C_BRIGHT_RED "\xff\x55\x55\xff"
#define VSX_FONT_C_BRIGHT_MAGENTA "\xff\x55\xff\xff"
#define VSX_FONT_C_YELLOW "\xff\xff\x55\xff"
#define VSX_FONT_C_WHITE "\xff\xff\xff\xff"

class vsx_font_info {
public:
  int type = 0; // 0 = texture (old) 1 = new (FtGL)
  std::unique_ptr<vsx_texture<>> texture;
  vsx_string<>name;
  void* ftfont = 0x0;
  void* ftfont_outline = 0x0;
};  

class vsx_font {
  vsx_vector3<> ep;
  void reinit(vsx_font_info* f_info, vsx_string<>font, vsx::filesystem* filesystem);
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
  VSX_ENGINE_GRAPHICS_DLLIMPORT void load(vsx_string<>font, vsx::filesystem* filesystem);
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

  vsx_vector3<> print(vsx_vector2f p, const vsx_string<>& str, const float size, const vsx_string<>colors = "")
  {
    return print(vsx_vector3f(p.x, p.y), str, size, colors);
  }

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

  ~vsx_font()
  {

  }
};

#endif
