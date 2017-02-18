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

#include "vsx_widget_controller_knob.h"
#include "widgets/vsx_widget_base_edit.h"


vsx_widget_controller_knob::vsx_widget_controller_knob()
{
  generate_menu();
  menu->init();

  color.set(85.0f/255.0f,170.0f/255.0f,1,1);
  capminv = -1;
  capmaxv = 1;
  capmin = false;
  capmax = false;
  multiplier=1;
  help_text += "- Hold ctrl/alt/shift to snap values";
}

void vsx_widget_controller_knob::init()
{
  if (init_run) return;
  vsx_widget_controller_base::init();
  get_value();
  editor = add(new vsx_widget_base_edit,name+".label");
  editor->init();
  editor->render_type = render_3d;
  editor->set_size(vsx_vector3<>(size.x,size.y*0.23f));
  editor->set_pos(vsx_vector3<>(0,size.y*0.5f+size.y*0.25f-editor->size.y*0.6f));
  editor->set_font_size(0.214286 * size.y); // sizeunit = 0.014f
  ((vsx_widget_base_edit*)editor)->allowed_chars = "0123456789.-";
  ((vsx_widget_base_edit*)editor)->command_prefix = "u";
  ((vsx_widget_base_edit*)editor)->size_from_parent = true;
  ((vsx_widget_base_edit*)editor)->single_row = true;

  hint=add(new vsx_widget_3d_hint,name+".hint");
  hint->size.y=size.y*0.15;
  hint->size.x=size.x;
  ((vsx_widget_3d_hint*)hint)->title=target_param;
  ((vsx_widget_3d_hint*)hint)->init();
}

void vsx_widget_controller_knob::on_delete() {
  vsx_mouse_control.show_cursor();
}

void vsx_widget_controller_knob::event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  if (controlling && button == 0)
  {
    vsx_mouse_control.show_cursor();
    vsx_mouse_control.set_cursor_pos(remPointer.x,remPointer.y);
  }
  vsx_widget_controller_base::event_mouse_up(distance, coords, button);
}

void vsx_widget_controller_knob::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  remWorld=distance.center;
  vsx_vector3<> a;
  a.x=distance.center.x/size.x;
  a.y=distance.center.y/size.y;
  if (a.length() <0.5 && button == 0)
  {
    controlling = true;
    vsx_mouse_control.hide_cursor();
  }
  vsx_widget_controller_base::event_mouse_down(distance, coords, button);
}

void vsx_widget_controller_knob::event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords)
{
  if (vsx_input_mouse.position != remPointer && controlling)
  {
    deltamove = (distance.center-remWorld)/size;

    // x moves slower than Y
    if (fabs(deltamove.x) > fabs(deltamove.y))
      deltamove.y=0.0f;
    else
      deltamove.x=0.0f;

    target_value += (((deltamove.y)+((deltamove.x)/15.0f)) * 0.1 * multiplier);
    cap_value();
    vsx_widget_controller_base::event_mouse_move(distance, coords);
    vsx_mouse_control.set_cursor_pos(remPointer.x,remPointer.y);
  } else
  vsx_widget_controller_base::event_mouse_move(distance, coords);
}

void vsx_widget_controller_knob::command_process_back_queue(vsx_command_s *t)
{
  if (t->cmd == "u") {
    target_value = vsx_string_helper::s2f(t->cmd_data);
    cap_value();
    send_to_server();
    k_focus = this;
  } else vsx_widget_controller_base::command_process_back_queue(t);
}

void vsx_widget_controller_knob::draw()
{
  if (!visible) return;

  vsx_widget_controller_base::draw();

  presentation_value /= multiplier;
  if (k_focus != editor)
  ((vsx_widget_base_edit*)editor)->set_string(vsx_string_helper::f2s(presentation_value,15));
  float angle=(fmod(presentation_value,1)*PI)*2;
  float target_angle=(fmod(target_value,1)*PI)*2;
  glBegin(GL_TRIANGLE_FAN);
    glColor4f(darkshade*color.r,darkshade*color.g,darkshade*color.b,1);
    glVertex3f((parentpos.x+pos.x),(parentpos.y+pos.y),pos.z);
    for (float t=0;t<2*PI;t+=PI/6.0f)
    {
      float c=1-fabs(sin(t+angle+0.21*PI));
      glColor4f(c*color.r,c*color.g,c*color.b,1);
      glVertex3f((parentpos.x+pos.x)+(sin(t+angle)*(size.x*0.49f)),(parentpos.y+pos.y)+(cos(t+angle)*(size.y*0.49f)),pos.z);
    }
  glEnd();
  float skew = 0.05;
  glBegin(GL_TRIANGLES);
    glColor4f(1,1,1,1);
    glVertex3f((parentpos.x+pos.x)+0.6*(sin(angle)*(size.x/2)),(parentpos.y+pos.y)+0.6*(cos(angle-skew)*((size.y)/2)),pos.z);
    glVertex3f((parentpos.x+pos.x)+(sin(angle-skew)*(size.x/2)),(parentpos.y+pos.y)+(cos(angle-skew)*((size.y)/2)),pos.z);
    glVertex3f((parentpos.x+pos.x)+(sin(angle+skew)*(size.x/2)),(parentpos.y+pos.y)+(cos(angle+skew)*((size.y)/2)),pos.z);
    if (dest_value) {
      glColor4f(1,1,1,0.4);
      skew = 0.04;
      glVertex3f((parentpos.x+pos.x)+0.1*(sin(target_angle)*(size.x/2)),(parentpos.y+pos.y)+0.1*(cos(target_angle-skew)*((size.y)/2)),pos.z);
      glVertex3f((parentpos.x+pos.x)+(sin(target_angle-skew)*(size.x/2)),(parentpos.y+pos.y)+(cos(target_angle-skew)*((size.y)/2)),pos.z);
      glVertex3f((parentpos.x+pos.x)+(sin(target_angle+skew)*(size.x/2)),(parentpos.y+pos.y)+(cos(target_angle+skew)*((size.y)/2)),pos.z);
    }
  glEnd();
  draw_children();
}
