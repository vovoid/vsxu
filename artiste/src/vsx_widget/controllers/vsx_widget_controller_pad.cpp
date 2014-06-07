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

#include "vsx_widget_controller_pad.h"
#include "vsx_widget_controller_knob.h"
#include <gl_helper.h>

vsx_widget_controller_pad::vsx_widget_controller_pad()
{
  widget_type = 0;
  a.x = 0;
  a.y = 0;

  b.x = 1;
  b.y = 1;

  widget_type = VSX_WIDGET_TYPE_CONTROLLER;
  coord_type = VSX_WIDGET_COORD_CENTER;
  set_size(vsx_vector3<>(0.1,0.1));
  set_pos(vsx_vector3<>(-0.06));

  generate_menu();
  menu->init();
  drawing = false;
}

void vsx_widget_controller_pad::init()
{
  prev_pos = 0;
  draw_area = size.x*0.5-sizeunit*1.3*0.5f;
  knob_x1 = add(new vsx_widget_controller_knob,"x1");
  knob_x1->set_pos(vsx_vector3<>(-draw_area-sizeunit*0.25,0));
  ((vsx_widget_controller_knob*)knob_x1)->target_param = "x1";
  ((vsx_widget_controller_knob*)knob_x1)->bgcolor.a=0;
  ((vsx_widget_controller_knob*)knob_x1)->owned=true;
  ((vsx_widget_controller_knob*)knob_x1)->drawconnection=false;
  ((vsx_widget_controller_knob*)knob_x1)->isolate=true;
  knob_x1->set_size(vsx_vector3<>(sizeunit*0.5,sizeunit*0.5));
  knob_x1->init();

  knob_y1 = add(new vsx_widget_controller_knob,"y1");
  knob_y1->set_pos(vsx_vector3<>(0,draw_area+sizeunit*0.25));
  ((vsx_widget_controller_knob*)knob_y1)->bgcolor.a=0;
  ((vsx_widget_controller_knob*)knob_y1)->owned=true;
  ((vsx_widget_controller_knob*)knob_y1)->drawconnection=false;
  ((vsx_widget_controller_knob*)knob_y1)->isolate=true;
  ((vsx_widget_controller_knob*)knob_y1)->target_param = "y1";
  knob_y1->set_size(vsx_vector3<>(sizeunit*0.5,sizeunit*0.5));
  knob_y1->init();

  knob_x2 = add(new vsx_widget_controller_knob,"x2");
  knob_x2->set_pos(vsx_vector3<>(draw_area+sizeunit*0.25,0));
  ((vsx_widget_controller_knob*)knob_x2)->target_param = "x2";
  ((vsx_widget_controller_knob*)knob_x2)->bgcolor.a=0;
  ((vsx_widget_controller_knob*)knob_x2)->owned=true;
  ((vsx_widget_controller_knob*)knob_x2)->drawconnection=false;
  ((vsx_widget_controller_knob*)knob_x2)->isolate=true;
  ((vsx_widget_controller_knob*)knob_x2)->target_value = 1.0;
  ((vsx_widget_controller_knob*)knob_x2)->value = 1.0;
  knob_x2->set_size(vsx_vector3<>(sizeunit*0.5,sizeunit*0.5));
  knob_x2->init();

  knob_y2 = add(new vsx_widget_controller_knob,"y2");
  knob_y2->set_pos(vsx_vector3<>(0,-draw_area-sizeunit*0.38));
  ((vsx_widget_controller_knob*)knob_y2)->bgcolor.a=0;
  ((vsx_widget_controller_knob*)knob_y2)->owned=true;
  ((vsx_widget_controller_knob*)knob_y2)->drawconnection=false;
  ((vsx_widget_controller_knob*)knob_y2)->isolate=true;
  ((vsx_widget_controller_knob*)knob_y2)->target_param = "y2";
  ((vsx_widget_controller_knob*)knob_y2)->value = 1.0;
  ((vsx_widget_controller_knob*)knob_y2)->target_value = 1.0;
  knob_y2->set_size(vsx_vector3<>(sizeunit*0.5,sizeunit*0.5));
  knob_y2->init();

}

void vsx_widget_controller_pad::draw()
{
  if (!visible) return;
  parentpos = parent->get_pos_p();
  vsx_widget_controller_base::draw();
  glColor4f(1.0f,1.0f,1.0f,0.2f);
  draw_box_c(parentpos+pos, draw_area,draw_area);
  long start_prevs = prev_pos-300;

  if (start_prevs < 0) start_prevs += 300;
  if (start_prevs >= (long)prev_draw.size()) start_prevs = 0;

  glLineWidth(1.0f);
  glBegin(GL_LINE_STRIP);
  bool run = true;
  unsigned long i = start_prevs;
  unsigned long j = 0;
  while (run)
  {
    glColor4f(1,1,1,prev_draw[i].z);
    glVertex2f(prev_draw[i].x+parentpos.x+pos.x,prev_draw[i].y+parentpos.y+pos.y);
    i++; j++;
    if (i >= prev_draw.size()) run = false;
    if (i >= 300) i = 0;
    if (j >= 300) run = false;
  }
  glEnd();
}

void vsx_widget_controller_pad::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  if (
      button == 0
      && fabs(distance.center.x) < draw_area
      && fabs(distance.center.y) < draw_area
      )
  {
    drawing = true;
    event_mouse_move(distance,coords);
  }
  else
  drawing = false;
  vsx_widget_controller_base::event_mouse_down(distance,coords,button);
}

void vsx_widget_controller_pad::event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  drawing = false;
  if (button != 0)
  vsx_widget_controller_base::event_mouse_up(distance,coords,button);
}

void vsx_widget_controller_pad::event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords)
{
  if (drawing) {
    float xx = ((distance.center.x+draw_area)/(draw_area*2));
    if (xx < 0.0f) xx = 0.0f;
    if (xx > 1.0f) xx = 1.0f;
    tracer.x = ((vsx_widget_controller_knob*)knob_x1)->value + xx * (((vsx_widget_controller_knob*)knob_x2)->value - ((vsx_widget_controller_knob*)knob_x1)->value);
    float yy = (1-(distance.center.y+draw_area)/(draw_area*2));
    if (yy < 0.0f) yy = 0.0f;
    if (yy > 1.0f) yy = 1.0f;
    tracer.y = ((vsx_widget_controller_knob*)knob_y1)->value + yy * (((vsx_widget_controller_knob*)knob_y2)->value - ((vsx_widget_controller_knob*)knob_y1)->value);
    prev_draw[prev_pos].x = distance.center.x;
    prev_draw[prev_pos].y = distance.center.y;
    if (prev_draw[prev_pos].x < -draw_area) prev_draw[prev_pos].x = -draw_area;
    if (prev_draw[prev_pos].y < -draw_area) prev_draw[prev_pos].y = -draw_area;
    if (prev_draw[prev_pos].x > draw_area) prev_draw[prev_pos].x = draw_area;
    if (prev_draw[prev_pos].y > draw_area) prev_draw[prev_pos].y = draw_area;
    prev_draw[prev_pos].z = 2.0f;
    prev_pos++;
    if (prev_pos > 300) prev_pos = 0; // cyclic buffer
    for (unsigned long i = 0; i < prev_draw.size(); i++)
    {
      prev_draw[i].z -= 2.0f/300.0f;
      if (prev_draw[i].z < 0.0f) prev_draw[i].z = 0.0f;
    }

    send_to_server();
  }
  else vsx_widget_controller_base::event_mouse_move(distance,coords);
}


void vsx_widget_controller_pad::command_process_back_queue(vsx_command_s *t)
{
  if (t->cmd == "param_set_interpolate") {
    printf("param_set_interp\n");
    if (t->parts[3] == "x1") {
      printf("setting new x1\n");
      a.x = vsx_string_helper::s2f(t->parts[1]);
    }
    if (t->parts[3] == "y1") a.y = vsx_string_helper::s2f(t->parts[1]);
    if (t->parts[3] == "x2") b.x = vsx_string_helper::s2f(t->parts[1]);
    if (t->parts[3] == "y2") b.y = vsx_string_helper::s2f(t->parts[1]);
  }  vsx_widget_controller_base::command_process_back_queue(t);
}


void vsx_widget_controller_pad::send_to_server()
{
  if (ptype == 0) {
    if (smoothness>=0) command_q_b.add_raw("param_set_interpolate "+ vsx_string_helper::f2s(tracer.x)+","+vsx_string_helper::f2s(tracer.y)+","+vsx_string_helper::f2s(tracer.z)  + " " + vsx_string_helper::f2s(smoothness) + " "+target_param);
    else command_q_b.add_raw("param_set "+ vsx_string_helper::f2s(tracer.x)+","+vsx_string_helper::f2s(tracer.y)+","+vsx_string_helper::f2s(tracer.z) + " "+target_param);
  } else {
    if (smoothness>=0) command_q_b.add_raw("param_set_interpolate "+ vsx_string_helper::f2s(tracer.x)+","+vsx_string_helper::f2s(tracer.y)+","+vsx_string_helper::f2s(tracer.z)+","+vsx_string_helper::f2s(tracer.w)  + " " + vsx_string_helper::f2s(smoothness) + " "+target_param);
    else command_q_b.add_raw("param_set "+ vsx_string_helper::f2s(tracer.x)+","+vsx_string_helper::f2s(tracer.y)+","+vsx_string_helper::f2s(tracer.z)+","+vsx_string_helper::f2s(tracer.w) + " "+target_param);
  }
  parent->vsx_command_queue_b(this);
}
