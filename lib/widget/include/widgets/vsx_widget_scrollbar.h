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

#pragma once

#include <vsx_widget.h>

#define VSX_WIDGET_SCROLLBAR_TYPE_HORIZONTAL 0
#define VSX_WIDGET_SCROLLBAR_TYPE_VERTICAL 1


class WIDGET_DLLIMPORT vsx_widget_scrollbar : public vsx_widget
{
  float scroll_handle_size;
  float cur_pos_click_down;
  float cur_pos;
  float value;
  float window_size;
  float scroll_max; // scroll from 0 to what?
  float* control_value;
  float shz;
  int scroll_type; // 0 = horizontal, 1 = vertical
  vsx_vector2f click_down;

public:

  void set_scroll_type( int n )
  {
    scroll_type = n;
  }

  void set_control_value( float* n)
  {
    control_value = n;
  }

  void set_window_size( float n )
  {
    window_size = n;
  }

  vsx_widget_scrollbar();

  void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords);
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);

  void i_draw();

};
