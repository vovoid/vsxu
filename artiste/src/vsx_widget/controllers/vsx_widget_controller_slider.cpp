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
  set_size(vsx_vector<>(sizeunit, sizeunit* 4.0f));
  handlesize=0.5*sizeunit;
  value=0.0f;target_value=0.0f;
  capminv=0;
  capmaxv=1;
  capmin = false;
  capmax = false;
  amp=1;
  ofs=0;
}

void vsx_widget_controller_slider::init()
{
  if (init_run) return;
  vsx_widget_controller_base::init();
  get_value();

  editor = add(new vsx_widget_base_edit,name+".edit");
  editor->init();
  editor->render_type = render_3d;
  editor->set_size(vsx_vector<>(sizeunit,sizeunit*0.23f));
  editor->set_pos(vsx_vector<>(0,size.y*0.5f+sizeunit*0.125f));
  editor->set_font_size(0.003);
}

void vsx_widget_controller_slider::cap_value()
{
  if (capmax)
  if (ofs > capmaxv) {
    ofs = capmaxv;
  }
  if (capmin)
  if (ofs < capminv) ofs = capminv;
  if (amp<ALMOST_ZERO) amp=ALMOST_ZERO;
  if (target_value > amp+ofs) target_value = amp+ofs;
  if (target_value < ofs) target_value = ofs;
  vsx_widget_controller_base::cap_value();
}

int vsx_widget_controller_slider::inside_xy_l(vsx_vector<> &test, vsx_vector<> &global)
{
  float ypos = get_y_pos();
  if (coord_type == VSX_WIDGET_COORD_CENTER)
  {
    if (
       (test.x > global.x-target_size.x*0.5f) &&
       (test.x < global.x+target_size.x*0.5f) &&
       (test.y > global.y+ypos-handlesize/2) &&
       (test.y < global.y+ypos+handlesize/2)
    )
    {
       return coord_type;
    }
  }
  return 0;
}

void vsx_widget_controller_slider::command_process_back_queue(vsx_command_s *t)
{
  if (owned && t->cmd == "si") { vsx_widget::command_process_back_queue(t); }
  else
  if (t->cmd == "u") {
    target_value = s2f(t->cmd_data);
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
    target_value = (-(((distance.corner.y-handlesize/2)/(size.y-handlesize))-1) * (amp))+ofs; //amp-ofs
    cap_value();
  }
  send_to_server();
  ++mouse_moves;
}

double vsx_widget_controller_slider::get_y_pos()
{
  return (((presentation_value-ofs)/(amp))*(size.y-handlesize))-size.y/2.0f+handlesize/2;//amp-ofs
}

void vsx_widget_controller_slider::draw()
{
  if (!visible) return;

  vsx_widget_controller_base::draw();
  glLineWidth(1.0f);
  float ypos=parentpos.y+pos.y+get_y_pos();
  ((vsx_widget_base_edit*)editor)->set_string(f2s(presentation_value,15));
  glBegin(GL_LINES);
    glColor4f(1,1,1,0.2);
    float ymin=parentpos.y+pos.y-size.y/2+handlesize/2,
          ymax=parentpos.y+pos.y+size.y/2-handlesize/2,
          stepfactor=(ymax-ymin)/20;
    int c=0;
    for (float yy=ymax;yy>=ymin-(stepfactor/2);yy-=stepfactor)
    {
      float ww=size.x*0.25;
      if (c%2==1) ww=size.x*0.4;
      if (c%10==0) ww=size.x*0.1;
      ++c;
      glVertex3f(parentpos.x+pos.x+size.x/2, yy,pos.z);
      glVertex3f(parentpos.x+pos.x+ww, yy,pos.z);
      glVertex3f(parentpos.x+pos.x-size.x/2, yy,pos.z);
      glVertex3f(parentpos.x+pos.x-ww, yy,pos.z);
    }
    glColor4f(0,0,0,1);
    glVertex3f(parentpos.x+pos.x, parentpos.y+pos.y+size.y/2-handlesize/2,pos.z);
    glVertex3f(parentpos.x+pos.x, parentpos.y+pos.y-size.y/2+handlesize/2,pos.z);
  glEnd();
  glBegin(GL_TRIANGLE_FAN);
    glColor4f(lightshade*color.r,lightshade*color.g,lightshade*color.b,color.a);
    glVertex3f(parentpos.x+pos.x+size.x/3, ypos+handlesize*0.5,pos.z);//top right
    glColor4f(darkshade*color.r,darkshade*color.g,darkshade*color.b,color.a);
    glVertex3f(parentpos.x+pos.x+size.x/3, ypos,pos.z);//right center
    glColor4f(lightshade*color.r,lightshade*color.g,lightshade*color.b,color.a);
    glVertex3f(parentpos.x+pos.x+size.x/3,ypos-handlesize*0.5,pos.z); //bottom right
    glVertex3f(parentpos.x+pos.x-size.x/3,ypos-handlesize*0.5,pos.z); //bottom left
    glColor4f(darkshade*color.r,darkshade*color.g,darkshade*color.b,color.a);
    glVertex3f(parentpos.x+pos.x-size.x/3, ypos,pos.z); //left center
    glColor4f(lightshade*color.r,lightshade*color.g,lightshade*color.b,color.a);
    glVertex3f(parentpos.x+pos.x-size.x/3,ypos+handlesize*0.5,pos.z); //top left
    glVertex3f(parentpos.x+pos.x+size.x/3, ypos+handlesize*0.5,pos.z);//top right
  glEnd();
  glBegin(GL_LINES);
    glColor4f(0,0,0,1);
    glVertex3f(parentpos.x+pos.x+size.x/3, ypos,pos.z);
    glVertex3f(parentpos.x+pos.x-size.x/3, ypos,pos.z);
  glEnd();
  draw_children();
}

