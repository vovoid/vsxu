/**
* Project: VSXu: Realtime modular visual programming language, music/audio visualizer.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "../vsx_widget_base.h"
#include "window/vsx_widget_window.h"
#include "lib/vsx_widget_button.h"
#include <gl_helper.h>

vsx_widget_window::vsx_widget_window()
{
	button_close = 0;
}

void vsx_widget_window::init() {
  render_type = VSX_WIDGET_RENDER_2D;
  coord_type = VSX_WIDGET_COORD_CORNER;
  pos.y = 0.7f;
  set_size(vsx_vector<>(0.3f,0.3f));
  topmost = true;
  title = "window";
  constrained_x = false;
  constrained_y = false;
  visible = 1;
  target_pos = pos;
  target_size = size;
  support_interpolation = true;
  button_close = add(new vsx_widget_button,"bc1");
  init_children();
  init_run = true;


  button_close->title = "x";
  button_close->commands.adds(4,"close","close","bc1");
}

void vsx_widget_window::i_draw() {
	if (button_close)
	{
		((vsx_widget_button*)button_close)->border = 0.0001;
    button_close->set_pos(vsx_vector<>(size.x-font_size*0.4f,size.y-font_size*0.5f-0.5f*dragborder));
    button_close->set_size(vsx_vector<>(font_size*0.4f,font_size*0.8f-dragborder));
	}

  if (visible) {
    font.color.a = color.a;
  glColor4f(skin_colors[1].r,skin_colors[1].g,skin_colors[1].b,skin_colors[1].a);
  draw_box(pos,size.x,size.y);
  glColor4f(skin_colors[2].r,skin_colors[2].g,skin_colors[2].b,skin_colors[2].a);
  draw_box(pos+vsx_vector<>(0.0f,size.y-font_size),size.x,font_size);

  // border
  glColor4f(skin_colors[0].r,skin_colors[0].g,skin_colors[0].b,skin_colors[0].a);

  // left
  draw_box(pos+vsx_vector<>(0,dragborder),dragborder,size.y-dragborder-dragborder);

  // right
  draw_box(pos+vsx_vector<>(size.x-dragborder,dragborder),dragborder,size.y-dragborder-dragborder);

  // bottom
  draw_box(pos,size.x,dragborder);

  // top
  draw_box(pos+vsx_vector<>(0.0f,size.y-dragborder),size.x,dragborder);
  glColor4f(skin_colors[3].r,skin_colors[3].g,skin_colors[3].b,skin_colors[3].a);
  font.print(vsx_vector<>((pos.x+font_size*0.1)*screen_aspect,pos.y+size.y-font_size*0.85),title,font_size*0.6);
	}
}
