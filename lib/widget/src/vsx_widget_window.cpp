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

#include <vsx_gl_global.h>
#include "vsx_widget.h"
#include "vsx_widget_window.h"
#include <widgets/vsx_widget_button.h>
#include <gl_helper.h>

vsx_widget_window::vsx_widget_window()
{
	button_close = 0;
}

void vsx_widget_window::init() {
  render_type = render_2d;
  coord_type = VSX_WIDGET_COORD_CORNER;
  widget_type = VSX_WIDGET_TYPE_WINDOW;
  pos.y = 0.7f;
  set_size(vsx_vector3<>(0.3f,0.3f));
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
		((vsx_widget_button*)button_close)->border = 0.0001f;
    button_close->set_pos(vsx_vector3<>(size.x-font_size*0.4f,size.y-font_size*0.5f-0.5f*(float)dragborder));
    button_close->set_size(vsx_vector3<>(font_size*0.4f,font_size*0.8f - (float)dragborder));
	}

  req(visible > 0.0f);
  font.color.a = color.a;


  vsx_widget_skin::get_instance()->set_color_gl(1);
  draw_box(pos,size.x,size.y);
  vsx_widget_skin::get_instance()->set_color_gl(2);
  draw_box(pos+vsx_vector3<>(0.0f,size.y-font_size),size.x,font_size);

  // border
  vsx_widget_skin::get_instance()->set_color_gl(0);

  // left
  draw_box(pos+vsx_vector3<>(0, (float)dragborder), (float)dragborder, size.y - (float)dragborder - (float)dragborder);

  // right
  draw_box(pos+vsx_vector3<>(size.x- (float)dragborder, (float)dragborder), (float)dragborder,size.y- (float)dragborder- (float)dragborder);

  // bottom
  draw_box(pos,size.x, (float)dragborder);

  // top
  draw_box(pos+vsx_vector3<>(0.0f,size.y- (float)dragborder),size.x, (float)dragborder);

  vsx_widget_skin::get_instance()->set_color_gl(3);

  font.print(vsx_vector3<>((pos.x+font_size*0.1f)*screen_aspect,pos.y+size.y-font_size*0.85f),title,font_size*0.6f);
}
