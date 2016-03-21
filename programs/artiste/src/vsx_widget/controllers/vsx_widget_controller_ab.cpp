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

#include <math/vector/vsx_vector3.h>
#include <math.h>
#include <vsx_gl_global.h>
#include <texture/vsx_texture.h>
#include "vsx_widget_controller_ab.h"
#include "vsx_widget_controller_knob.h"

void vsx_widget_controller_ab::init()
{
  tuple_type = VSX_WIDGET_TYPE_CONTROLLER;
  tuple_type = 3; // float3 datatype
  get_value();
  quadratic = gluNewQuadric();										    // Create A Pointer To The Quadric Object
  gluQuadricNormals(quadratic, GLU_SMOOTH);						// Create Smooth Normals
  gluQuadricTexture(quadratic, GL_TRUE);							// Create Texture Coords
  vsx_widget::init();
  constrained_x = false;
  constrained_y = false;
  set_pos(vsx_vector3<>(-sizeunit*1.2));
  set_size(vsx_vector3<>(0.02f, 0.02f));

  ball_size.y = ball_size.x = size.x/2;

  dest_quat.s.X = 0;
  dest_quat.s.Y = 1;
  dest_quat.s.Z = 0;
  dest_quat.s.W = 0;

  Transform.M[0] = 1.0f;
  Transform.M[1] = 0.0f;
  Transform.M[2] = 0.0f;
  Transform.M[3] = 0.0f;

  Transform.M[4] = 0.0f;
  Transform.M[5] = 1.0f;
  Transform.M[6] = 0.0f;
  Transform.M[7] = 0.0f;

  Transform.M[8] = 0.0f;
  Transform.M[9] = 0.0f;
  Transform.M[10] = 1.0f;
  Transform.M[11] = 0.0f;

  Transform.M[12] = 0.0f;
  Transform.M[13] = 0.0f;
  Transform.M[14] = 0.0f;
  Transform.M[15] = 1.0f;


  LastRot.M[0] = 1.0f;
  LastRot.M[1] = 0.0f;
  LastRot.M[2] = 0.0f;

  LastRot.M[3] = 0.0f;
  LastRot.M[4] = 1.0f;
  LastRot.M[5] = 0.0f;

  LastRot.M[6] = 0.0f;
  LastRot.M[7] = 0.0f;
  LastRot.M[8] = 1.0f;


  ThisRot.M[0] = 1.0f;
  ThisRot.M[1] = 0.0f;
  ThisRot.M[2] = 0.0f;

  ThisRot.M[3] = 0.0f;
  ThisRot.M[4] = 1.0f;
  ThisRot.M[5] = 0.0f;

  ThisRot.M[6] = 0.0f;
  ThisRot.M[7] = 0.0f;
  ThisRot.M[8] = 1.0f;


  int_rot.M[0] = 1.0f;
  int_rot.M[1] = 0.0f;
  int_rot.M[2] = 0.0f;
  int_rot.M[3] = 0.0f;

  int_rot.M[4] = 0.0f;
  int_rot.M[5] = 1.0f;
  int_rot.M[6] = 0.0f;
  int_rot.M[7] = 0.0f;

  int_rot.M[8] = 0.0f;
  int_rot.M[9] = 0.0f;
  int_rot.M[10] = 1.0f;
  int_rot.M[11] = 0.0f;

  int_rot.M[12] = 0.0f;
  int_rot.M[13] = 0.0f;
  int_rot.M[14] = 0.0f;
  int_rot.M[15] = 1.0f;

  mtex = vsx_texture_loader::load(
    vsx_widget_skin::get_instance()->skin_path_get() + "controllers/sphere.png",
    vsx::filesystem::get_instance(),
    true, // threaded
    vsx_bitmap::flip_vertical_hint,
    vsx_texture_gl::linear_interpolate_hint
  );

  generate_menu();
  menu->init();
  ArcBall = new ArcBall_t(0,0);

  // set the arcball in usable mode
  MousePt.s.Y = MousePt.s.X = 0;
  Quat4fT     ThisQuat;
  ArcBall->drag(&MousePt, &ThisQuat);						// Update End Vector And Get Rotation As Quaternion

  Matrix3fSetRotationFromQuat4f(&ThisRot, &ThisQuat);		// Convert Quaternion Into Matrix3fT
  Matrix3fMulMatrix3f(&ThisRot, &LastRot);				// Accumulate Last Rotation Into This One
  dest_quat.s.X = 0;
  dest_quat.s.Y = 0;
  dest_quat.s.Z = 1;
  dest_quat.s.W = 1;
  LastRot = ThisRot;
}


void vsx_widget_controller_ab::set_tuple_type( int new_value )
{
  tuple_type = new_value;
}

void vsx_widget_controller_ab::draw()
{
  if (!visible) return;
  parentpos = parent->get_pos_p();

  if (smoothness>=0)
  {
    float tt = vsx_widget_time::get_instance()->get_dtime() * smoothness;
    if (tt > 1) tt = 1;
    Quat4fT temp_quat;
    QuatSlerp(&inter_quat, &dest_quat,tt, &temp_quat);
    inter_quat = temp_quat;

    Matrix3fSetRotationFromQuat4f(&NewRot, &inter_quat);
  }
  else
  {
    Matrix3fSetRotationFromQuat4f(&NewRot, &dest_quat);
    v2_int[0] = v2[0];
    v2_int[1] = v2[1];
    v2_int[2] = v2[2];
    int_rot = Transform;
  }

  Matrix4fSetRotationFromMatrix3f(&Transform, &NewRot);
  int_rot = Transform;

  glColor3f(1,1,1);



  glLineWidth(2);
  glBegin(GL_LINES);
    glColor4f(1,1,1,0.5);
    glVertex3f(parentpos.x,parentpos.y,pos.z);
    glColor4f(1,1,1,0);
    glVertex3f(parentpos.x+pos.x,parentpos.y+pos.y,pos.z);
  glEnd();



  vsx_widget_controller_base::draw();


  glPushMatrix();
    glTranslatef(parentpos.x+pos.x, parentpos.y+pos.y, pos.z);
    glMultMatrixf(int_rot.M);

    glLineWidth(3);
    glBegin(GL_LINE_STRIP);
      glColor4f(1.0,1.0,1.0,1.0);
          glVertex3f(0,-size.x/2*0.87,0);
      glColor4f(1.0,1.0,1.0,0.0);
          glVertex3f(0,0,0);
      glColor4f(1.0,1.0,1.0,1.0);
          glVertex3f(0,size.x/2*0.87,0);
    glEnd();

    glBegin(GL_LINE_STRIP);
      glColor4f(1.0,0.5,1.0,1.0);
          glVertex3f(-size.x/2*0.87,0,0);
      glColor4f(1.0,1.0,1.0,0.0);
          glVertex3f(0,0,0);
      glColor4f(1.0,0.5,1.0,1.0);
          glVertex3f(size.x/2*0.87,0,0);
    glEnd();

    glBegin(GL_LINE_STRIP);
      glColor4f(1.0,0.5,1.0,1.0);
          glVertex3f(0,0,-size.x/2*0.87);
      glColor4f(1.0,1.0,1.0,0.0);
          glVertex3f(0,0,0);
      glColor4f(1.0,0.5,1.0,1.0);
          glVertex3f(0,0,size.x/2*0.87);
    glEnd();

    glLineWidth(1);

    if (v2[2] <= 0) {
      glColor4f(1.0f,1.0f,1.0f,0.7f);
      glCullFace(GL_FRONT);
      glEnable(GL_CULL_FACE);
      mtex->bind();
        gluSphere(quadratic,size.x/2*0.8,10,10);
      mtex->_bind();
      glDisable(GL_CULL_FACE);
    }

    glPushMatrix();
    glColor4f(1.0f,1.0f,1.0f,1.0f);
    glTranslatef(0,size.x/2*0.9,0);
    gluSphere(quadratic,size.x/2*0.09,3,3);
    glPopMatrix();

  // after line

    if (v2[2] > 0) {
      glColor4f(1.0f,1.0f,1.0f,0.7f);
      glCullFace(GL_FRONT);
      glEnable(GL_CULL_FACE);
      mtex->bind();
        gluSphere(quadratic,size.x/2*0.8,10,10);
      mtex->_bind();
      glDisable(GL_CULL_FACE);
    }
  glPopMatrix();
}

void vsx_widget_controller_ab::event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button) {

    if (controlling && button == 0)
      vsx_mouse_control.set_cursor_pos(remPointer.x, remPointer.y);
    vsx_mouse_control.show_cursor();
    LastRot = ThisRot;
    controlling = false;
    vsx_widget::event_mouse_up(distance,coords,button);
}

void vsx_widget_controller_ab::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button) {
  if (button == 0) {
    MousePt.s.X = distance.center.x/size.x;
    MousePt.s.Y = distance.center.y/size.y;
    m_focus = this;
    a_focus = this;
    k_focus = this;
    ArcBall->click(&MousePt);
    if (sqrt(MousePt.s.X*MousePt.s.X + MousePt.s.Y*MousePt.s.Y) < 0.4f)
    {
      controlling = true;
      remPointer = vsx_input_mouse.position;
      vsx_mouse_control.hide_cursor();
    }
  }

  vsx_widget::event_mouse_down(distance,coords,button);
}

void vsx_widget_controller_ab::event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords)
{
  if (vsx_input_mouse.position != remPointer && controlling)
  {
    Point2fT    oldMousePt = MousePt;

    Quat4fT ThisQuat;
    MousePt.s.X = distance.center.x/size.x;
    MousePt.s.Y = distance.center.y/size.y;


    ArcBall->drag(&MousePt, &ThisQuat);
    MousePt = oldMousePt;

    Matrix3fSetRotationFromQuat4f(&ThisRot, &ThisQuat);
    Matrix3fMulMatrix3f(&ThisRot, &LastRot);

    float v1[3];
    v1[0] = 0;
    v1[1] = 1;
    v1[2] = 0;

    mat_vec_mult3x3(&ThisRot.M[0], &v1[0], &v2[0]);
    v_norm(&v2[0]);


    QuatFromMat(&ThisRot, dest_quat );

    if (tuple_type == 3) {
      if (smoothness>=0) command_q_b.add_raw("param_set_interpolate "+ vsx_string_helper::f2s(v2[0])+","+vsx_string_helper::f2s(v2[1])+","+vsx_string_helper::f2s(v2[2])  + " " + vsx_string_helper::f2s(smoothness) + " "+target_param);
      else command_q_b.add_raw("param_set "+ vsx_string_helper::f2s(v2[0])+","+vsx_string_helper::f2s(v2[1])+","+vsx_string_helper::f2s(v2[2]) + " "+target_param);
    } else
    {
      if (smoothness>=0) command_q_b.add_raw("param_set_interpolate "+ vsx_string_helper::f2s(dest_quat.s.X)+","+vsx_string_helper::f2s(dest_quat.s.Y)+","+vsx_string_helper::f2s(dest_quat.s.Z)+","+vsx_string_helper::f2s(dest_quat.s.W)  + " " + vsx_string_helper::f2s(smoothness) + " "+target_param);
      else command_q_b.add_raw("param_set "+ vsx_string_helper::f2s(dest_quat.s.X)+","+vsx_string_helper::f2s(dest_quat.s.Y)+","+vsx_string_helper::f2s(dest_quat.s.Z)+","+vsx_string_helper::f2s(dest_quat.s.W)  + " " + target_param);
    }
    parent->vsx_command_queue_b(this);

    LastRot = ThisRot;

    vsx_mouse_control.set_cursor_pos(remPointer.x,remPointer.y);
  } else
  if (!controlling)
  vsx_widget_controller_base::event_mouse_move(distance, coords);
}

void vsx_widget_controller_ab::event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  VSX_UNUSED(distance);
  VSX_UNUSED(coords);
  VSX_UNUSED(button);

  vsx_mouse_control.show_cursor();
  _delete();
}

void vsx_widget_controller_ab::command_process_back_queue(vsx_command_s *t) {
  if (t->cmd == "pg64_ok")
  {
    vsx_nw_vector <vsx_string<> > parts;
    vsx_string<> deli = ",";
    t->parts[3] = vsx_string_helper::base64_decode(t->parts[3]);
    vsx_string_helper::explode(t->parts[3],deli, parts);
    if (parts.size() == 4) {
      dest_quat.s.X = inter_quat.s.X = vsx_string_helper::s2f(parts[0]);
      dest_quat.s.Y = inter_quat.s.Y = vsx_string_helper::s2f(parts[1]);
      dest_quat.s.Z = inter_quat.s.Z = vsx_string_helper::s2f(parts[2]);
      dest_quat.s.W = inter_quat.s.W = vsx_string_helper::s2f(parts[3]);
      Matrix3fSetRotationFromQuat4f(&LastRot, &dest_quat);
    }
  } else vsx_widget_controller_base::command_process_back_queue(t);
}
