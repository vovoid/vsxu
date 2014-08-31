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

#ifndef VSX_FONT_OUTLINE_H
#define VSX_FONT_OUTLINE_H

#include "vsx_color.h"
#include "vsxfst.h"
#include "debug/vsx_error.h"

typedef struct
{
  float size_x, size_y;
  vsx_string string;
} text_info;

class vsx_font_outline {

  // font
  vsx_string cur_font;

  void* ftfont;
  void* ftfont2;

  // binary data
  char* fdata;

  // text
  vsx_string text;
  vsx_avector<text_info> lines;

  // meta
  vsxf* filesystem;
  vsx_gl_state* gl_state;

  // settings
  int render_type;
  int align;
  float glyph_size;
  float size;
  float leading;
  float outline_thickness;
  vsx_color<> color;
  vsx_color<> color_outline;

public:

  vsx_font_outline()
    :
      ftfont(0x0),
      ftfont2(0x0),
      fdata(0x0),
      filesystem(0x0),
      gl_state(0x0),
      render_type(1), // poly + outline
      align(1),
      glyph_size(24.0),
      size(1.0f),
      leading(1.0f),
      outline_thickness(1.0f),
      color(1.0, 1.0, 1.0, 0.0),
      color_outline(1.0, 1.0, 1.0, 0.0)
  {}


  void text_set(vsx_string& s)
  {
    text = s;
    process_lines();
  }

  void color_set(vsx_color<> c)
  {
    color = c;
  }

  void color_outline_set(vsx_color<> c)
  {
    color_outline = c;
  }

  void outline_thickness_set(float n)
  {
    outline_thickness = n;
  }

  vsx_string text_get()
  {
    return text;
  }

  void filesystem_set(vsxf* fs)
  {
    filesystem = fs;
  }

  int process_lines();
  void load_font(vsx_string font_path);
  void unload();
  void render();
};


#endif

