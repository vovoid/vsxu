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

#include "vsx_widget_controller_slider.h"
#include "widgets/vsx_widget_base_edit.h"

vsx_widget_controller_slider::vsx_widget_controller_slider()
{
  generate_menu();
  menu->init();
  color.set(85.0f/255.0f,170.0f/255.0f,1,1);
  set_size(vsx_vector3<>(sizeunit, sizeunit* 4.0f));
  handle_size = 0.5 * sizeunit;
  orientation = 0; // vertical
  value=0.0f;
  target_value=0.0f;
  capminv=0;
  capmaxv=1;
  capmin = false;
  capmax = false;
  amp=1;
  ofs=0;
}

void vsx_widget_controller_slider::init()
{
  if (init_run)
    return;

  vsx_widget_controller_base::init();

  get_value();

  editor = add(new vsx_widget_base_edit,name+".edit");
  editor->init();
  editor->render_type = render_3d;
  editor->set_size(vsx_vector3<>(sizeunit,sizeunit*0.23f));
  editor->set_pos(vsx_vector3<>(0, size.y * 0.5f + sizeunit*0.125f));
  editor->set_font_size(0.003);
}

void vsx_widget_controller_slider::set_horizontal()
{
  orientation = 1;

  set_size(vsx_vector3<>(sizeunit * 4.0f, sizeunit));

  editor->set_size(vsx_vector3<>(sizeunit, sizeunit*0.23f));
  editor->set_pos(vsx_vector3<>(size.x * 0.5 + sizeunit * 0.125, 0));
}

void vsx_widget_controller_slider::set_vertical()
{
  orientation = 0;

  set_size(vsx_vector3<>(sizeunit, sizeunit* 4.0f));

  editor->set_size(vsx_vector3<>(sizeunit,sizeunit*0.23f));
  editor->set_pos(vsx_vector3<>(0, size.y * 0.5f + sizeunit*0.125f));
}

void vsx_widget_controller_slider::cap_value()
{
  if (capmax)
    if (ofs > capmaxv)
      ofs = capmaxv;

  if (capmin)
    if (ofs < capminv)
      ofs = capminv;

  if (amp<ALMOST_ZERO)
    amp=ALMOST_ZERO;

  if (target_value > amp+ofs)
    target_value = amp+ofs;

  if (target_value < ofs)
    target_value = ofs;

  vsx_widget_controller_base::cap_value();
}

int vsx_widget_controller_slider::inside_xy_l(vsx_vector3<> &test, vsx_vector3<> &global)
{
  float ypos = get_y_pos();
  if (coord_type == VSX_WIDGET_COORD_CENTER)
    if (
       (test.x > global.x - target_size.x*0.5f) &&
       (test.x < global.x + target_size.x*0.5f) &&
       (test.y > global.y + ypos-handle_size/2) &&
       (test.y < global.y + ypos+handle_size/2)
    )
       return coord_type;
  return 0;
}

void vsx_widget_controller_slider::command_process_back_queue(vsx_command_s *t)
{
  if (owned && t->cmd == "si") { vsx_widget::command_process_back_queue(t); }
  else
  if (t->cmd == "u") {
    target_value = vsx_string_helper::s2f(t->cmd_data);
    cap_value();
    send_to_server();
    k_focus = this;
  } else vsx_widget_controller_base::command_process_back_queue(t);
}

void vsx_widget_controller_slider::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  controlling = true;
  vsx_widget_controller_base::event_mouse_down(distance, coords, button);
}

void vsx_widget_controller_slider::event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords)
{
  VSX_UNUSED(coords);

  if (controlling)
  {
    if (orientation == 0)
      target_value = (-(((distance.corner.y-handle_size/2)/(size.y-handle_size))-1) * (amp))+ofs;
    if (orientation == 1)
      target_value = (-(((distance.corner.x-handle_size/2)/(size.x-handle_size))-1) * (amp))+ofs;
    cap_value();
  }
  send_to_server();
  ++mouse_moves;
}

double vsx_widget_controller_slider::get_y_pos()
{
  if (orientation == 0)
    return (((presentation_value-ofs)/(amp))*(size.y-handle_size))-size.y/2.0f + handle_size/2;

  if (orientation == 1)
    return (((presentation_value-ofs)/(amp))*(size.x-handle_size))-size.x/2.0f + handle_size/2;

  return 0.0;
}

float vsx_widget_controller_slider::amp_get()
{
  return amp;
}

void vsx_widget_controller_slider::amp_set(float n)
{
  amp = n;
}


float vsx_widget_controller_slider::ofs_get()
{
  return ofs;
}


void vsx_widget_controller_slider::ofs_set(float n)
{
  ofs = n;
}


void vsx_widget_controller_slider::draw_vertical()
{
  float ypos = parentpos.y + pos.y + get_y_pos();
  ((vsx_widget_base_edit*)editor)->set_string(vsx_string_helper::f2s(presentation_value,15));
  glBegin(GL_LINES);
    glColor4f(1,1,1,0.2);
    float ymin = parentpos.y + pos.y - size.y/2 + handle_size/2,
          ymax = parentpos.y + pos.y + size.y/2 - handle_size/2,
          stepfactor = (ymax - ymin)/20;
    int c=0;
    for (float yy=ymax; yy >= ymin - (stepfactor/2); yy -= stepfactor)
    {
      float ww = size.x*0.25;
      if (c%2 == 1)
        ww=size.x*0.4;

      if (c%10 == 0)
        ww=size.x*0.1;

      ++c;
      glVertex3f(parentpos.x+pos.x+size.x/2, yy,pos.z);
      glVertex3f(parentpos.x+pos.x+ww, yy,pos.z);
      glVertex3f(parentpos.x+pos.x-size.x/2, yy,pos.z);
      glVertex3f(parentpos.x+pos.x-ww, yy,pos.z);
    }
    glColor4f(0,0,0,1);
    glVertex3f(parentpos.x+pos.x, parentpos.y+pos.y+size.y/2-handle_size/2,pos.z);
    glVertex3f(parentpos.x+pos.x, parentpos.y+pos.y-size.y/2+handle_size/2,pos.z);
  glEnd();


  glBegin(GL_TRIANGLE_FAN);
    glColor4f(lightshade*color.r,lightshade*color.g,lightshade*color.b,color.a);
    glVertex3f(parentpos.x+pos.x+size.x/3, ypos+handle_size*0.5,pos.z);//top right
    glColor4f(darkshade*color.r,darkshade*color.g,darkshade*color.b,color.a);
    glVertex3f(parentpos.x+pos.x+size.x/3, ypos,pos.z);//right center
    glColor4f(lightshade*color.r,lightshade*color.g,lightshade*color.b,color.a);
    glVertex3f(parentpos.x+pos.x+size.x/3,ypos-handle_size*0.5,pos.z); //bottom right
    glVertex3f(parentpos.x+pos.x-size.x/3,ypos-handle_size*0.5,pos.z); //bottom left
    glColor4f(darkshade*color.r,darkshade*color.g,darkshade*color.b,color.a);
    glVertex3f(parentpos.x+pos.x-size.x/3, ypos,pos.z); //left center
    glColor4f(lightshade*color.r,lightshade*color.g,lightshade*color.b,color.a);
    glVertex3f(parentpos.x+pos.x-size.x/3,ypos+handle_size*0.5,pos.z); //top left
    glVertex3f(parentpos.x+pos.x+size.x/3, ypos+handle_size*0.5,pos.z);//top right
  glEnd();


  glBegin(GL_LINES);
    glColor4f(0,0,0,1);
    glVertex3f(parentpos.x+pos.x+size.x/3, ypos,pos.z);
    glVertex3f(parentpos.x+pos.x-size.x/3, ypos,pos.z);
  glEnd();
}

void vsx_widget_controller_slider::draw_horizontal()
{
  float ypos = parentpos.y + pos.y + get_y_pos();
  ((vsx_widget_base_edit*)editor)->set_string(vsx_string_helper::f2s(presentation_value,15));
  glBegin(GL_LINES);
    glColor4f(1,1,1,0.2);
    float ymin = parentpos.y + pos.y - size.y/2 + handle_size/2,
          ymax = parentpos.y + pos.y + size.y/2 - handle_size/2,
          stepfactor = (ymax - ymin)/20;
    int c=0;
    for (float yy=ymax; yy >= ymin - (stepfactor/2); yy -= stepfactor)
    {
      float ww = size.x*0.25;
      if (c%2 == 1)
        ww=size.x*0.4;

      if (c%10 == 0)
        ww=size.x*0.1;

      ++c;
      glVertex3f(parentpos.x+pos.x+size.x/2, yy,pos.z);
      glVertex3f(parentpos.x+pos.x+ww, yy,pos.z);
      glVertex3f(parentpos.x+pos.x-size.x/2, yy,pos.z);
      glVertex3f(parentpos.x+pos.x-ww, yy,pos.z);
    }
    glColor4f(0,0,0,1);
    glVertex3f(parentpos.x+pos.x, parentpos.y+pos.y+size.y/2-handle_size/2,pos.z);
    glVertex3f(parentpos.x+pos.x, parentpos.y+pos.y-size.y/2+handle_size/2,pos.z);
  glEnd();


  glBegin(GL_TRIANGLE_FAN);
    glColor4f(lightshade*color.r,lightshade*color.g,lightshade*color.b,color.a);
    glVertex3f(parentpos.x+pos.x+size.x/3, ypos+handle_size*0.5,pos.z);//top right
    glColor4f(darkshade*color.r,darkshade*color.g,darkshade*color.b,color.a);
    glVertex3f(parentpos.x+pos.x+size.x/3, ypos,pos.z);//right center
    glColor4f(lightshade*color.r,lightshade*color.g,lightshade*color.b,color.a);
    glVertex3f(parentpos.x+pos.x+size.x/3,ypos-handle_size*0.5,pos.z); //bottom right
    glVertex3f(parentpos.x+pos.x-size.x/3,ypos-handle_size*0.5,pos.z); //bottom left
    glColor4f(darkshade*color.r,darkshade*color.g,darkshade*color.b,color.a);
    glVertex3f(parentpos.x+pos.x-size.x/3, ypos,pos.z); //left center
    glColor4f(lightshade*color.r,lightshade*color.g,lightshade*color.b,color.a);
    glVertex3f(parentpos.x+pos.x-size.x/3,ypos+handle_size*0.5,pos.z); //top left
    glVertex3f(parentpos.x+pos.x+size.x/3, ypos+handle_size*0.5,pos.z);//top right
  glEnd();


  glBegin(GL_LINES);
    glColor4f(0,0,0,1);
    glVertex3f(parentpos.x+pos.x+size.x/3, ypos,pos.z);
    glVertex3f(parentpos.x+pos.x-size.x/3, ypos,pos.z);
  glEnd();
}

void vsx_widget_controller_slider::draw()
{
  if (!visible)
    return;

  vsx_widget_controller_base::draw();
  glLineWidth(1.0f);

  if (orientation == 0)
    draw_vertical();

  if (orientation == 1)
    draw_horizontal();

  draw_children();
}

