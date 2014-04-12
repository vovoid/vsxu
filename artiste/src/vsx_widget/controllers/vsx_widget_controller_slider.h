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

#ifndef VSX_WIDGET_CONTROLLER_SLIDER_H
#define VSX_WIDGET_CONTROLLER_SLIDER_H


#include "vsx_widget_controller_base.h"


class vsx_widget_controller_slider : public vsx_widget_controller_base
{
public:
  float amp, ofs;
  vsx_vector<> clickpos,deltamove,remPointer,remWorld;
  vsx_color<> marker_color;
  float handlesize;
  vsx_widget* hint;
  vsx_widget* editor;
  void init();
  void draw();
  void cap_value();
  int inside_xy_l(vsx_vector<> &test, vsx_vector<> &global);
  void command_process_back_queue(vsx_command_s *t);
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords);

  double get_y_pos();
  vsx_widget_controller_slider();
};

#endif
