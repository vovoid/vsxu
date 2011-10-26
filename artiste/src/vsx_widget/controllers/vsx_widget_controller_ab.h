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
#ifndef VSX_WIDGET_CONTROLLER_ARCBALL_H
#define VSX_WIDGET_CONTROLLER_ARCBALL_H


#include "arcball/arcball.h"

class vsx_widget_controller_ab : public vsx_widget_base_controller {
private:
  GLUquadricObj *quadratic;
  //vsx_vector parentpos;
  //float rot;
  vsx_vector clickpos,deltamove,remPointer,remWorld;
  float v2[3];
  float v2_int[3];
  
  Matrix4fT   Transform;
  Matrix3fT   LastRot;
  Matrix3fT   ThisRot;
  Matrix3fT   NewRot;
  //Matrix4fT   dest_quat;
  Matrix4fT   int_rot;
  Point2fT    MousePt;
  Quat4fT     dest_quat;
  Quat4fT     inter_quat;
  bool        isClicked;
  bool        isRClicked;
  bool        isDragging;
  vsx_vector  ball_size;
  vsx_widget* label;

  vsx_texture mtex;
  
public:
  ArcBall_t*   ArcBall;
  
  int  type;

  void init();
  void draw();
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords);
  void vsx_command_process_b(vsx_command_s *t);
  vsx_widget_controller_ab()
  {
  	v2[0] = v2[1] = v2[2] = 0.0f;
  }
};

class vsx_widget_controller_color : public vsx_widget_base_controller {
private:
  vsx_widget* old_k_focus;
  vsx_color color;
  float angle_dest;
  float angle;
  float color_x, color_y;
  void calculate_color();
  void send_to_server();
  float base_size;
  bool color_down, angle_down, alpha_down;
public:
  int type; // 0 = float3, 1 = float4
  void init();
  void draw();
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords);
  bool event_key_down(signed long key, bool alt, bool ctrl, bool shift);
  //bool event_key_up(signed long key, bool alt, bool ctrl, bool shift);
  void vsx_command_process_b(vsx_command_s *t);
  
  vsx_widget_controller_color();
};

class vsx_widget_controller_pad : public vsx_widget_base_controller {
  bool drawing;
  vsx_color tracer;
  vsx_vector a,b;
  vsx_widget* knob_x1;
  vsx_widget* knob_y1;
  vsx_widget* knob_x2;
  vsx_widget* knob_y2;
  float draw_area;
  vsx_avector<vsx_vector> prev_draw;
  int prev_pos;
  void send_to_server();
public:
  int ptype; // 0 = float3, 1 = float4
  void init();
  void draw();
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords);
  void vsx_command_process_b(vsx_command_s *t);
  
  vsx_widget_controller_pad();
  
};

#endif
#endif
