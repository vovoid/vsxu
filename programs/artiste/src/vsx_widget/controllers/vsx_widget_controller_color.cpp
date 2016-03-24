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

#include "vsx_widget_controller_color.h"

vsx_widget_controller_color::vsx_widget_controller_color()
{
  angle = angle_dest = 0;
  color_x = color_y = 1;
  type = 0;
}

void vsx_widget_controller_color::init()
{
  type = VSX_WIDGET_TYPE_CONTROLLER;
  base_size = 0.8;
  alpha_down = angle_down = color_down = false;
  set_size(vsx_vector3<>(0.03,0.03));
  set_pos(vsx_vector3<>(-0.02));
  bgcolor.r = 0;
  bgcolor.g = 0;
  bgcolor.b = 0;
  get_value();
  generate_menu();
  menu->init();
}

void vsx_widget_controller_color::calculate_color()
{
  color.hsv(angle_dest,color_x,color_y);
}

void vsx_widget_controller_color::send_to_server()
{
  if (type == 0)
  {
    if (smoothness>=0)
      command_q_b.add_raw("param_set_interpolate "+ vsx_string_helper::f2s(color.r)+","+vsx_string_helper::f2s(color.g)+","+vsx_string_helper::f2s(color.b)  + " " + vsx_string_helper::f2s(smoothness) + " "+target_param);
    else
      command_q_b.add_raw("param_set "+ vsx_string_helper::f2s(color.r)+","+vsx_string_helper::f2s(color.g)+","+vsx_string_helper::f2s(color.b) + " "+target_param);
  }
  else
  {
    if (smoothness>=0)
      command_q_b.add_raw("param_set_interpolate "+ vsx_string_helper::f2s(color.r)+","+vsx_string_helper::f2s(color.g)+","+vsx_string_helper::f2s(color.b)+","+vsx_string_helper::f2s(color.a)  + " " + vsx_string_helper::f2s(smoothness) + " "+target_param);
    else
      command_q_b.add_raw("param_set "+ vsx_string_helper::f2s(color.r)+","+vsx_string_helper::f2s(color.g)+","+vsx_string_helper::f2s(color.b)+","+vsx_string_helper::f2s(color.a) + " "+target_param);
  }
  parent->vsx_command_queue_b(this);
}

void vsx_widget_controller_color::command_process_back_queue(vsx_command_s *t) {
  // 0=pg64_ok [1=module_name] [2=param] MC4yMTI4OTE5OTU5MDY4Mjk4MzM5OCwwLjAwMDAwMDAwMDAwMDAwOTI5NjU3LDAuOTk5OTk5ODgwNzkwNzEwNDQ5MjIsMS4wMDAwMDAwMDAwMDAwMDAwMDAwMA== 2054
  if (t->cmd == "pg64_ok")
  {
    vsx_nw_vector <vsx_string<> > parts;
    vsx_string<>deli = ",";
    t->parts[3] = vsx_string_helper::base64_decode(t->parts[3]);
    vsx_string_helper::explode(t->parts[3],deli, parts);

    // RGB
    if (parts.size() == 3)
    {
      vsx_color<> t;
      color.r = vsx_string_helper::s2f(parts[0]);
      color.g = vsx_string_helper::s2f(parts[1]);
      color.b = vsx_string_helper::s2f(parts[2]);
      color.get_hsv(&t);
      angle_dest = t.h;
      color_x = t.s;
      color_y = t.v;
    }

    // RGBA
    if (parts.size() == 4)
    {
      vsx_color<> t;
      color.r = vsx_string_helper::s2f(parts[0]);
      color.g = vsx_string_helper::s2f(parts[1]);
      color.b = vsx_string_helper::s2f(parts[2]);

      color.get_hsv(&t);
      angle_dest = t.h;
      color_x = t.s;
      color_y = t.v;
      color.a = vsx_string_helper::s2f(parts[3]);
    }

    set_value(vsx_string_helper::s2f(t->parts[3]));

  } else vsx_widget_controller_base::command_process_back_queue(t);
}

void vsx_widget_controller_color::draw()
{
  if (!visible) return;
  parentpos = parent->get_pos_p();
  vsx_widget_controller_base::draw();

  glPushMatrix();
  glTranslatef(parentpos.x+pos.x, parentpos.y+pos.y, pos.z);
  float sx = size.x*0.5;
  float sy = size.y*0.5;
  float gridc = 70.0;
  float gi = 1.0/gridc;

  float ss = size.x*base_size;
  float wheel_h = size.x*(1-base_size);

  float cur_angle = angle = angle_dest;
  cur_angle += 0.5;
  if (cur_angle >= 1.0) cur_angle -=1.0;
  for (float i = 0; i < 1.0; i+=gi) {
      glBegin(GL_QUADS);
        vsx_color<> aa;
        aa.hsv(cur_angle,1,1); glColor3f(aa.r, aa.g, aa.b);
        glVertex3f(-sx+i*ss,sy,0);
        glVertex3f(-sx+i*ss,sy-wheel_h,0);
        cur_angle += gi;
        if (cur_angle >= 1.0) cur_angle -=1.0;

        aa.hsv(cur_angle,1,1); glColor3f(aa.r, aa.g, aa.b);
        glVertex3f(-sx+(i+gi)*ss,sy-wheel_h,0);
        glVertex3f(-sx+(i+gi)*ss,sy,0);
      glEnd();
  }
  glColor4f(0,0,0,1);
  glBegin(GL_LINE_STRIP);
    glVertex3f(-sx+0.5*ss,sy,0);
    glVertex3f(-sx+0.5*ss,sy-wheel_h*0.2,0);
  glEnd();

  glColor4f(1,1,1,1);
  glBegin(GL_LINE_STRIP);
    glVertex3f(-sx+0.5*ss,sy-wheel_h*0.8,0);
    glVertex3f(-sx+0.5*ss,sy-wheel_h,0);
  glEnd();

  cur_angle -= 0.5;
  if (cur_angle < 0.0) cur_angle +=1.0;

  gridc = 10.0;
  gi = 1.0 / gridc;

  for (float i = 0; i < 1.0; i+=gi) {
    for (float j = 0; j < 1.0; j+=gi) {
      glBegin(GL_QUADS);
        vsx_color<> aa;
        aa.hsv(cur_angle,i,j); glColor3f(aa.r, aa.g, aa.b);
        glVertex3f(-sx+i*ss,-sy+j*ss,0);

        aa.hsv(cur_angle,i+gi,j); glColor3f(aa.r, aa.g, aa.b);
        glVertex3f(-sx+(i+gi)*ss,-sy+j*ss,0);

        aa.hsv(cur_angle,i+gi,j+gi); glColor3f(aa.r, aa.g, aa.b);
        glVertex3f(-sx+(i+gi)*ss,-sy+(j+gi)*ss,0);

        aa.hsv(cur_angle,i,j+gi); glColor3f(aa.r, aa.g, aa.b);
        glVertex3f(-sx+i*ss,-sy+(j+gi)*ss,0);
      glEnd();
    }
  }


  glColor4f(color.r, color.g, color.b, color.a);
  glBegin(GL_QUADS);
    glVertex3f(sx-0.2*size.x,sy-0.2*size.y,0);
    glVertex3f(sx,sy-0.2*size.y,0);
    glVertex3f(sx,sy,0);
    glVertex3f(sx-0.2*size.x,sy,0);
  //glEnd();

  glColor4f(color.r, color.g, color.b, 1.0);
  //glBegin(GL_QUADS);
    glVertex3f(sx-0.2*size.x,sy-0.2*size.y,0);
    glVertex3f(sx,sy-0.2*size.y,0);
    glColor4f(color.r, color.g, color.b, 0);
    glVertex3f(sx,-sy,0);
    glVertex3f(sx-0.2*size.x,-sy,0);
  glEnd();

  // color_x
  glColor4f(1,1,1,0.7);
  glBegin(GL_LINE_STRIP);
    glVertex3f(-sx+ss*color_x,-sy,0);
    glVertex3f(-sx+ss*color_x,-sy+ss,0);
  glEnd();

  // color_y
  glBegin(GL_LINE_STRIP);
    glVertex3f(-sx,-sy+ss*color_y,0);
    glVertex3f(-sx+ss,-sy+ss*color_y,0);
  glEnd();

  // alpha
  glBegin(GL_LINE_STRIP);
    glVertex3f(-sx+ss,-sy+ss*color.a,0);
    glVertex3f(sx,-sy+ss*color.a,0);
  glEnd();

  glPopMatrix();
}

void vsx_widget_controller_color::event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords)
{
  if (color_down)
  {
    color_x = (distance.center.x+size.x/2)/(size.x*base_size);
    if (color_x < 0) color_x = 0;
    if (color_x > 1) color_x = 1;

    color_y = (distance.center.y+size.y/2)/(size.y*base_size);
    if (color_y < 0) color_y = 0;
    if (color_y > 1) color_y = 1;
    calculate_color();
    send_to_server();
    return;
  }

  if (angle_down)
  {
    if (vsx_input_mouse.position != remPointer)
    {
      angle_dest -= (distance.center.x-remWorld.x)*20;
      while (angle_dest > 1) angle_dest -= 1.0;
      while (angle_dest < 0) angle_dest += 1.0;
      vsx_mouse_control.set_cursor_pos(remPointer.x,remPointer.y);
      calculate_color();
      send_to_server();
    }
    return;
  }

  if (alpha_down)
  {
    color.a = (distance.center.y+size.y/2)/(size.y*base_size);
    if (color.a < 0) color.a = 0;
    if (color.a > 1) color.a = 1;
    calculate_color();
    send_to_server();
    return;
  }
  vsx_widget_controller_base::event_mouse_move(distance,coords);
}

void vsx_widget_controller_color::event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button) {
  if (angle_down || alpha_down) vsx_application_control::get_instance()->mouse.show_cursor();
  alpha_down = angle_down = color_down = false;
  vsx_widget::event_mouse_up(distance,coords,button);
}

void vsx_widget_controller_color::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button) {
  //clickpos.x += size.x/2;
  //clickpos.y += size.y/2;
  color_down = angle_down = alpha_down = false;
  //world = world-pos;
  remWorld = distance.center;//-get_pos_p();
/*  a_focus = this;
  m_focus = this;
  k_focus = this;*/
  //vsx_vector p = parent->get_pos_p();
  //p += pos;
  //printf("wx : %f\n",world.x);
  if (button == 0) {
    if (((distance.center.x > -size.x/2) && (distance.center.x < -size.x/2+base_size*size.x))
    && ((distance.center.y > -size.y/2) && (distance.center.y < -size.y/2+base_size*size.y)))
    {
      color_down = true;
    }
    else
    if (((distance.center.x > -size.x/2) && (distance.center.x < -size.x/2+base_size*size.x))
    && ((distance.center.y > -size.y/2+base_size*size.y) && (distance.center.y < size.y/2)))
    {
      angle_down = true;
      remPointer = vsx_input_mouse.position;
      vsx_mouse_control.hide_cursor();
    } else
    if (((distance.center.x > -size.x/2+base_size*size.x) && (distance.center.x < size.x/2))
    && ((distance.center.y > -size.y/2) && (distance.center.y < -size.y/2+base_size*size.y)))
    {
      alpha_down = true;
    }
  }
  if (color_down) event_mouse_move(distance,coords);
  if (alpha_down) event_mouse_move(distance,coords);
//  vsx_widget_3d_grid_comp::event_mouse_down(world,screen,button);
  vsx_widget_controller_base::event_mouse_down(distance,coords,button);
}

void vsx_widget_controller_color::event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  VSX_UNUSED(distance);
  VSX_UNUSED(coords);
  VSX_UNUSED(button);
  _delete();
}

bool vsx_widget_controller_color::event_key_down(uint16_t key)
{
  switch (key) {
    case 'f':
      angle_dest+= 0.01;
      if (angle_dest > 1.0) angle_dest -= 1.0;
      if (angle_dest < 0.0) angle_dest += 1.0;
      calculate_color();
      send_to_server();
      return false;
    break;
    case 's':
      angle_dest-= 0.01;
      if (angle_dest > 1.0) angle_dest -= 1.0;
      if (angle_dest < 0.0) angle_dest += 1.0;
      calculate_color();
      send_to_server();
      return false;
    break;
  }
  return true;
}
