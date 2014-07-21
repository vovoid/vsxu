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

#ifndef VSX_WIDGET_PANEL_H
#define VSX_WIDGET_PANEL_H

#include <vsx_widget.h>

class WIDGET_DLLIMPORT vsx_widget_panel : public vsx_widget {
public:
  bool size_from_parent;
  bool pos_from_parent;

  void calc_size();
  virtual int inside_xy_l(vsx_vector3<> &test, vsx_vector3<> &global);
  vsx_vector3<> calc_pos();
  void base_draw();
  virtual void i_draw() { base_draw(); }
  vsx_widget_panel();
};

#define VSX_WIDGET_SPLIT_PANEL_VERT 0 
#define VSX_WIDGET_SPLIT_PANEL_HORIZ 1 

class WIDGET_DLLIMPORT vsx_widget_split_panel : public vsx_widget_panel {
  // 2 x base_editor
public:
  float split_pos; // 0 -> 1
  float splitter_size;
  vsx_vector3<> size_max;
  vsx_widget_panel* one;
  vsx_widget_panel* two;
  int orientation;
  float sy;
  void i_draw();
  void set_border(float border);

  void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords);
  
  void event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords);

  int inside_xy_l(vsx_vector3<> &test, vsx_vector3<> &global);

  vsx_widget_split_panel();

};





#endif
