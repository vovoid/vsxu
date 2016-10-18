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

#ifndef VSX_WIDGET_CONTROLLER_ARCBALL_H
#define VSX_WIDGET_CONTROLLER_ARCBALL_H

#include "vsx_widget_controller_base.h"
#include "arcball/arcball.h"

class vsx_widget_controller_ab : public vsx_widget_controller_base
{
private:
  GLUquadricObj *quadratic;
  vsx_vector2f clickpos;
  vsx_vector2f deltamove;
  vsx_vector2f remPointer;
  vsx_vector2f remWorld;
  float v2[3] = {0,0,0};
  float v2_int[3] = {0, 0, 0};
  
  Matrix4fT   Transform;
  Matrix3fT   LastRot;
  Matrix3fT   ThisRot;
  Matrix3fT   NewRot;
  Matrix4fT   int_rot;
  Point2fT    MousePt;
  Quat4fT     dest_quat;
  Quat4fT     inter_quat;
  bool        isClicked;
  bool        isRClicked;
  bool        isDragging;
  vsx_vector3<>  ball_size;
  vsx_widget* label;

  std::unique_ptr<vsx_texture<>> mtex;
  ArcBall_t*   ArcBall;

  int  tuple_type; // 3 = float3, 4 = float4

public:

  void set_tuple_type( int new_value );
  void init();
  void draw();
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords);
  void command_process_back_queue(vsx_command_s *t);

};



#endif
