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

#ifndef VSX_WIDGET_CONSOLE_H
#define VSX_WIDGET_CONSOLE_H

// VSX_WIDGET_CONSOLE **************************************************************************************************
// VSX_WIDGET_CONSOLE **************************************************************************************************
// VSX_WIDGET_CONSOLE **************************************************************************************************
// VSX_WIDGET_CONSOLE **************************************************************************************************


class vsx_widget_2d_console : public vsx_widget {
public:
  double rows;
  //vsx_font myf;
  vsx_string<>h;
  double texty;
  bool up;
  double fontsize;
  double tx;
  double ypos;
  double yposs;
  double ythrust;
  double height;
  double htime;
  vsx_widget *editor;
  vsx_widget *destination;
  void init();
  void command_process_back_queue(vsx_command_s *t);
  int inside_xy_l(vsx_vector3<> &test, vsx_vector3<> &global);
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void draw_2d();
  void set_destination(vsx_widget* dest);
};

#endif
