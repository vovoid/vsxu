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

#ifndef VSX_WIDGET_CONNECTOR_H
#define VSX_WIDGET_CONNECTOR_H

// VSX_WIDGET_CONNECTOR ************************************************************************************************
// VSX_WIDGET_CONNECTOR ************************************************************************************************
// VSX_WIDGET_CONNECTOR ************************************************************************************************
// VSX_WIDGET_CONNECTOR ************************************************************************************************


class vsx_widget_connector_bezier : public vsx_widget {
  bool macro_child;
  bool transparent;
  vsx_vector3<> pv;
  double sx,sy,ex,ey; // internal coordinates
  double old_sx, old_sy, old_ex, old_ey;
  vsx_vector3<> cached_spline[25];
  std::unique_ptr<vsx_texture<>> mtex_blob;
  bool color_initialized;
  vsx_color<> color;
  
public:
  static float dim_alpha;
  float dim_my_alpha;
  vsx_vector3<> real_pos; // storing the value of the position of the drawing
                       // so that the anchor can order them in the order they
                       // appear visually. Updated on each frame in the draw func.
  bool alias_conn; // is this an alias connection?
  bool open;
  int order;
  static bool receiving_focus;
  vsx_widget *destination;
  
  void command_process_back_queue(vsx_command_s *t);
  
  void show_children() {}
  void hide_children() {}

  void before_delete();
  void on_delete();
  void init();
  int inside_xy_l(vsx_vector3<> &test, vsx_vector3<> &global);
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords);
  bool event_key_down(uint16_t key);
  void draw();
  vsx_widget_connector_bezier();
};

#endif
