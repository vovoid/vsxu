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


#ifndef VSX_WIDGET_CONTROLLER_PAD_H
#define VSX_WIDGET_CONTROLLER_PAD_H

#include <math/vector/vsx_vector4.h>
#include "vsx_widget_controller_base.h"


class vsx_widget_controller_pad : public vsx_widget_controller_base
{
  bool drawing;
  vsx_vector4<> tracer;
  vsx_vector3<> a,b;
  vsx_widget* knob_x1;
  vsx_widget* knob_y1;
  vsx_widget* knob_x2;
  vsx_widget* knob_y2;
  float draw_area;
  vsx_nw_vector<vsx_vector3<> > prev_draw;
  int prev_pos;
  void send_to_server();
public:
  int ptype; // 0 = float3, 1 = float4
  void init();
  void draw();
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords);
  void command_process_back_queue(vsx_command_s *t);

  vsx_widget_controller_pad();

};


#endif
