/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies Copyright (C) 2003-2011
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
  vsx_string h;
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
  void vsx_command_process_b(vsx_command_s *t);
  int inside_xy_l(vsx_vector &test, vsx_vector &global);
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void draw_2d();
  void set_destination(vsx_widget* dest);
};

#endif
