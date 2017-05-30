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

#include <color/vsx_color.h>
#include <filesystem/vsx_filesystem.h>
#include "debug/vsx_error.h"
#include <FTGL/ftgl.h>
#include <string/vsx_string_helper.h>

template < typename W = char >
class text_info {
public:
  float size_x, size_y;
  vsx_string<W> string;
};

typedef enum vsx_font_outline_render_type_e
{
  inner = 1,
  outline = 2,
  inner_outline = 3
} vsx_font_outline_render_type;

#include "vsx_font_outline_holder.h"
#include "vsx_outline_font_cache.h"

template< typename W = char >
class vsx_font_outline
{
  void* font_holder;

  // text
  vsx_string<W>text;
  vsx_nw_vector< text_info<W> > lines;

  // meta
  vsx::filesystem* filesystem;
  vsx_gl_state* gl_state;

  // settings
  vsx_font_outline_render_type render_type;

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
      font_holder(0x0),
      filesystem(0x0),
      gl_state(0x0),
      render_type(inner_outline),
      align(1),
      glyph_size(24.0),
      size(1.0f),
      leading(1.0f),
      outline_thickness(1.0f),
      color(1.0, 1.0, 1.0, 0.0),
      color_outline(1.0, 1.0, 1.0, 0.0)
  {}

  ~vsx_font_outline()
  {
  }

  void set_row_spacing(float n)
  {
    leading = n;
  }

  void set_align_left()
  {
    align = 0;
  }

  void set_align_center()
  {
    align = 1;
  }

  void set_align_right()
  {
    align = 2;
  }

  void text_set(vsx_string<W> s)
  {
    text = s;
    process_lines();
  }

  void text_set_ref(vsx_string<W>& s)
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

  vsx_string<>text_get()
  {
    return text;
  }

  void filesystem_set(vsx::filesystem* fs)
  {
    filesystem = fs;
  }

  int process_lines()
  {
    if (!font_holder)
      return 0;

    vsx_nw_vector< vsx_string<W> > t_lines;
    vsx_string_helper::explode_single<W>( text, (W)0x0A, t_lines );
    lines.clear();
    for (unsigned long i = 0; i < t_lines.size(); ++i)
    {
      float x1, y1, z1, x2, y2, z2;
      lines[i].string = t_lines[i];
      FTFont* font = ((font_outline_holder*)font_holder)->get_inner();
      font->BBox(t_lines[i].c_str(), x1, y1, z1, x2, y2, z2);
      lines[i].size_x = x2 - x1;
      lines[i].size_y = y2 - y1;
    }
    return 1;
  }

  void load_font(vsx_string<>font_path, outline_font_cache* cache)
  {
    if (!filesystem)
      VSX_ERROR_RETURN("filesystem not set");

    if (font_holder)
      unload(cache);

    font_holder = cache->get_font( filesystem, font_path, render_type );
  }

  void unload(outline_font_cache* cache)
  {
    if (0x0 == font_holder)
      return;

    cache->recycle( (font_outline_holder*)font_holder);
    font_holder = 0x0;
  }

  void render_lines(void* font_inner_p, void* font_outline_p)
  {
    FTFont* font_inner = (FTFont*) font_inner_p;
    FTFont* font_outline = (FTFont*) font_outline_p;

    float ypos = 0;
    for (unsigned long i = 0; i < lines.size(); ++i)
    {
      gl_state->matrix_push();
        if (align == 0)
          gl_state->matrix_translate_f( 0, ypos, 0 );

        if (align == 1)
          gl_state->matrix_translate_f( -lines[i].size_x*0.5f,ypos, 0 );

        if (align == 2)
          gl_state->matrix_translate_f( -lines[i].size_x,ypos,0 );

        glColor4f(color.r, color.g, color.b, color.a);
        font_inner->Render(lines[i].string.c_str());

        if (render_type & outline)
        {
          glColor4f(color_outline.r, color_outline.g, color_outline.b, color_outline.a);
          font_outline->Render(lines[i].string.c_str());
        }
      gl_state->matrix_pop();
      ypos += leading;
    }
  }

  void render()
  {
    if (!font_holder)
      return;

    FTFont* font_inner = ((font_outline_holder*)font_holder)->get_inner();
    FTFont* font_outline = ((font_outline_holder*)font_holder)->get_outline();

    if (render_type & outline && !font_outline)
      VSX_ERROR_RETURN("Font Outline not initialized");

    if (gl_state == 0x0)
      gl_state = vsx_gl_state::get_instance();

    gl_state->matrix_mode (VSX_GL_MODELVIEW_MATRIX );
    gl_state->matrix_push();
    float pre_linew = gl_state->line_width_get();

      if (render_type & inner)
        glEnable(GL_TEXTURE_2D);

      gl_state->line_width_set( outline_thickness );
      render_lines(font_inner, font_outline);

      if (render_type & inner)
        glDisable(GL_TEXTURE_2D);

    gl_state->line_width_set( pre_linew );
    gl_state->matrix_pop();
  }
};


#endif

