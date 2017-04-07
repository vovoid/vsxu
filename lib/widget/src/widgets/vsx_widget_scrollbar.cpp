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

#include "vsx_gl_global.h"
#include <map>
#include <list>
#include <vector>
#include <command/vsx_command.h>
#include <texture/vsx_texture.h>
#include "vsx_font.h"
#include "vsx_widget.h"
#include <widgets/vsx_widget_scrollbar.h>
#include <gl_helper.h>


vsx_widget_scrollbar::vsx_widget_scrollbar()
  :
    scroll_handle_size(0.5),
    cur_pos_click_down(0),
    cur_pos(0),
    value(0),
    window_size(0.5),
    scroll_max(1),
    control_value(0x0),
    shz(0),
    scroll_type(VSX_WIDGET_SCROLLBAR_TYPE_HORIZONTAL)
{
  coord_type = VSX_WIDGET_COORD_CORNER;
}

void vsx_widget_scrollbar::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  VSX_UNUSED(coords);
  VSX_UNUSED(button);
  click_down = distance.corner;
  cur_pos_click_down = cur_pos;
}  

void vsx_widget_scrollbar::event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords)
{
  VSX_UNUSED(coords);
    
  if (scroll_type == VSX_WIDGET_SCROLLBAR_TYPE_HORIZONTAL)
  {
    cur_pos = distance.corner.x + cur_pos_click_down-click_down.x;
    if (cur_pos < 0) cur_pos = 0;
    if (cur_pos > (1-shz)*size.x) cur_pos = (1-shz)*size.x;
    if ( fabsf((1-shz) * size.x) > 0.0f)
    value = ((cur_pos/((1-shz)*size.x))*(float)scroll_max);
    else value = 0;
  }

  if (scroll_type == VSX_WIDGET_SCROLLBAR_TYPE_VERTICAL)
  {
    cur_pos = (-distance.corner.y + cur_pos_click_down+click_down.y);

    if (cur_pos < 0.0f)
      cur_pos = 0.0f;

    if (cur_pos > (1-shz)*size.y)
      cur_pos = (1-shz)*size.y;

    value = cur_pos / ( ( 1 - shz ) * size.y) * scroll_max;
  }

  if (control_value) *control_value = value;
}

void vsx_widget_scrollbar::i_draw()
{
  req(visible > 0.0f);

  scroll_handle_size = window_size;

  if (control_value)
    value = *control_value;

  if (scroll_handle_size > scroll_max)
  {
    scroll_max = scroll_handle_size;
  }

  if (size.y < 0)
    return;

  if (size.x < 0)
    return;

  vsx_vector3<> p = parent->get_pos_p()+pos;
  p.z = pos.z;
  shz = scroll_handle_size/(scroll_max);

  // draw background
  vsx_widget_skin::get_instance()->set_color_gl(0);

  draw_box(p,size.x,size.y);

  // draw handle
  glColor4f(1,1,1,1);

  if (scroll_type == VSX_WIDGET_SCROLLBAR_TYPE_HORIZONTAL)
  {
    // horizontal
    cur_pos = (size.x-size.x*(shz))*(value/scroll_max);
    p.x += cur_pos;
    draw_box(p,size.x*shz,size.y);
    return;
  }

  if (scroll_type == VSX_WIDGET_SCROLLBAR_TYPE_VERTICAL)
  {
    // vertical
    cur_pos = (size.y-size.y*(shz))*(value/scroll_max);
    p.y = -cur_pos+p.y+size.y;
    draw_box(p,size.x,-size.y*shz);
  }
}
