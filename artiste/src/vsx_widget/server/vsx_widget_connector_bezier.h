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

#ifndef VSX_NO_CLIENT
#ifndef VSX_WIDGET_CONNECTOR_H
#define VSX_WIDGET_CONNECTOR_H

// VSX_WIDGET_CONNECTOR ************************************************************************************************
// VSX_WIDGET_CONNECTOR ************************************************************************************************
// VSX_WIDGET_CONNECTOR ************************************************************************************************
// VSX_WIDGET_CONNECTOR ************************************************************************************************


class vsx_widget_connector_bezier : public vsx_widget {
  bool macro_child;
  bool transparent;
  vsx_vector pv;
  double sx,sy,ex,ey; // internal coordinates
  double old_sx, old_sy, old_ex, old_ey;
  vsx_vector cached_spline[25];
  vsx_texture mtex_blob;
  bool color_initialized;
  vsx_color color;
  
public:
  static float dim_alpha;
  float dim_my_alpha;
  vsx_vector real_pos; // storing the value of the position of the drawing
                       // so that the anchor can order them in the order they
                       // appear visually. Updated on each frame in the draw func.
  bool alias_conn; // is this an alias connection?
  bool open;
  int order;
  static bool receiving_focus;
//  double smx, smy; // coordinate where mouse has been clicked
//  double mdx, mdy; // delta mouse move
  vsx_widget *destination;
  
  void vsx_command_process_b(vsx_command_s *t);
  
  void show_children() {};
  void hide_children() {};

  void before_delete();
  void on_delete();
  void init();
  int inside_xy_l(vsx_vector &test, vsx_vector &global);
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords);
  bool event_key_down(signed long key, bool alt, bool ctrl, bool shift);
  void draw();
  vsx_widget_connector_bezier();
};



#endif
#endif
