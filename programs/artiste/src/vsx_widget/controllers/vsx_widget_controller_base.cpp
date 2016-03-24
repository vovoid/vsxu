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

#include <math.h>
#include "vsx_widget_controller_base.h"
#include "widgets/vsx_widget_popup_menu.h"

inline double f_round(float val, int p)
{
  double t=pow(10.0,p);
  return (double)
      (int)(trunc((float)((double)val*t)+((float)((double)val*t)>=0.0?0.5:-0.5)))
      /t;
}




vsx_widget_controller_base::vsx_widget_controller_base() //include first
{
	hint = 0;
  r_time = 0;
  visible=true; //is control visible?
  widget_type = VSX_WIDGET_TYPE_CONTROLLER;
  controlling = false; //control is currently controlled ny user
  isolate=false; //control is isolated from sending or recieving it's value.
  owned=false; //controll is owner by other control, thus it should behave differently
  color.set(1.0f,1.0f,1.0f,1.0f); //foreground color
  bgcolor.set(1.0f,1.0f,1.0f,1.0f); //background color
  darkshade=0.5;   //dark shade multiplier
  lightshade=0.7; //light shade multiplier
  value = 0;    //the current value that the control represents
  target_value = 0;
  smoothness=10;  //how smooth interpolation should be
  capminv = 0;
  capmaxv = 1;
  capmin = false;
  capmax = false;

  // we want the destvalue
  dest_value = true;
  constrained_x = false;
  constrained_y = false;
  set_pos(vsx_vector3<>(-sizeunit*1.2));
  set_size(vsx_vector3<>(sizeunit, sizeunit));
  drawconnection=true; //draw line between this controll and it's parent
  target_param="";
  param_spec = 0;
  support_interpolation = true;
  help_text = "- right-click to bring up menu to choose\n\
  interpolation speed.\n\
- hit 'c' on the keyboard to close\n\
  all open controllers\n";
}

vsx_widget_controller_base::~vsx_widget_controller_base()
{
  vsx_mouse_control.show_cursor();
}

void vsx_widget_controller_base::init()
{
  if (init_run) return;
  set_value(value);
  //cout <<"init controller, name=" << name << " param=" << target_param << endl;
  //cout << "param spec: " << in_param_spec << endl;
  //printf("in param spec: %s\n",in_param_spec.c_str());
  if (param_spec)
  for (std::map<vsx_string<>,vsx_string<> >::iterator it = param_spec->begin(); it != param_spec->end(); ++it)
  {
    get_in_param_spec((*it));
  }
  init_run = true;
}

void vsx_widget_controller_base::generate_menu()
{
  menu = add(new vsx_widget_popup_menu,".anchor_menu");
  menu->commands.adds(VSX_COMMAND_MENU, "Close [C]", "delete",""); //build a menu
  menu->commands.adds(VSX_COMMAND_MENU, "Close all controllers [Ctrl+C]", "delete_controllers",""); //build a menu
  menu->commands.adds(VSX_COMMAND_MENU, "-----------------------", "","");
  menu->commands.adds(VSX_COMMAND_MENU, "Smoothness;Super Ultra Smooth", "si","0.2");
  menu->commands.adds(VSX_COMMAND_MENU, "Smoothness;Ultra Smooth", "si","1");
  menu->commands.adds(VSX_COMMAND_MENU, "Smoothness;Extremely Smooth", "si","2.5");
  menu->commands.adds(VSX_COMMAND_MENU, "Smoothness;Very Smooth", "si","4");
  menu->commands.adds(VSX_COMMAND_MENU, "Smoothness;Smooth", "si","6");
  menu->commands.adds(VSX_COMMAND_MENU, "Smoothness;Standard", "si","8");
  menu->commands.adds(VSX_COMMAND_MENU, "Smoothness;Fast", "si","16");
  menu->commands.adds(VSX_COMMAND_MENU, "Smoothness;No smoothing", "si","-1");
  menu->commands.adds(VSX_COMMAND_MENU, "Show Destination Value;On", "dest_value","1");
  menu->commands.adds(VSX_COMMAND_MENU, "Show Destination Value;Off", "dest_value","0");
  menu->size.x = 0.2;
  menu->size.y = 0.5;
  //don't forget to init the menu in subclasses
}

void vsx_widget_controller_base::get_value()
{
  if (!isolate)
  {
  	//printf("command suffix: %s\n", command_suffix.c_str());
    command_q_b.add_raw("pg64 "+target_param + " " + vsx_string_helper::i2s(id)+command_suffix);
    parent->vsx_command_queue_b(this);
  }
}

void vsx_widget_controller_base::get_in_param_spec(std::pair<vsx_string<>,vsx_string<> > parampair)
{
    if (parampair.first=="param") target_param=parampair.second;
    if (parampair.first=="pos_x") pos.x=vsx_string_helper::s2f(parampair.second)*sizeunit;
    if (parampair.first=="pos_y") pos.y=vsx_string_helper::s2f(parampair.second)*sizeunit;
    if (parampair.first=="size_x") size.x=vsx_string_helper::s2f(parampair.second)*sizeunit;
    if (parampair.first=="size_y") size.y=vsx_string_helper::s2f(parampair.second)*sizeunit;
    if (parampair.first=="max") { capmaxv=vsx_string_helper::s2f(parampair.second); capmax=true; }
    if (parampair.first=="min") { capminv=vsx_string_helper::s2f(parampair.second); capmin=true; }
    if (parampair.first=="smooth") smooth(vsx_string_helper::s2f(parampair.second));
}

float vsx_widget_controller_base::snap(float newval)
{
  if (vsx_input_keyboard.pressed_shift() && !vsx_input_keyboard.pressed_ctrl() && !vsx_input_keyboard.pressed_alt())
    return f_round(newval,1);
  if (vsx_input_keyboard.pressed_ctrl() && !vsx_input_keyboard.pressed_shift() && !vsx_input_keyboard.pressed_alt())
    return f_round(newval,2);
  if (vsx_input_keyboard.pressed_alt() && !vsx_input_keyboard.pressed_ctrl() && !vsx_input_keyboard.pressed_shift())
    return f_round(newval,3);
  return newval;
}

void vsx_widget_controller_base::set_value(float newval,bool cap)
{
  send_value = presentation_value=value=target_value=newval;
  if (cap) cap_value(); //avoid recursion
}

void vsx_widget_controller_base::smooth(float newval)
{
  smoothness=newval;
}

void vsx_widget_controller_base::command_process_back_queue(vsx_command_s *t)
{
  //cout<<name<<endl;
  //t->parse();
  float tmp=value;
  if (t->cmd == "dest_value") {
    if (t->cmd_data == "1") dest_value = true;
    else dest_value = false;
  } else
  if (t->cmd == "si") {
    //printf("setting interpolation\n");
    smooth(vsx_string_helper::s2f(t->cmd_data));
  } else
  if (t->cmd == "pg64_ok" && !isolate)
    set_value(vsx_string_helper::s2f(vsx_string_helper::base64_decode(t->parts[3])));
  else
  if (t->cmd == "delete" && !owned) {
    //printf("closing...\n");
    _delete();
  } else
  if (t->cmd == "delete_controllers") {
    delete_all_by_type(widget_type);
  }
  else
  command_q_b.add(t);
  if (tmp!=value) set_value(value);
}

void vsx_widget_controller_base::event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button)//include last
{
  target_value = snap(target_value);

  if (button == 0)
  controlling = false;
  vsx_widget::event_mouse_up(distance,coords,button);
}

void vsx_widget_controller_base::before_delete() {
  if (owned) parent->_delete();
}

void vsx_widget_controller_base::event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  command_q_b.add_raw("delete");
  if (owned) {
    //parent->_delete();
    parent->vsx_command_queue_b(this);
  }else vsx_command_queue_b(this);
  parent->event_mouse_double_click(distance,coords,button);
}

void vsx_widget_controller_base::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button)//include last
{
  mouse_moves = 0;

  remPointer = vsx_input_mouse.position;

  if (!owned)
  {
    smx = 0;
    smy = 0;
  }
  vsx_widget::event_mouse_down(distance,coords,button);
}

void vsx_widget_controller_base::send_to_server()
{
  send_value = target_value;
  send_value = snap(send_value);

  if (!isolate)
  {
    if (smoothness >= 0) {
      command_q_b.add_raw("param_set_interpolate "+ vsx_string_helper::f2s(send_value)  + " " + vsx_string_helper::f2s(smoothness) + " "+target_param);
    }
    else command_q_b.add_raw("param_set "+ vsx_string_helper::f2s(send_value) + " "+target_param);
    parent->vsx_command_queue_b(this);
  } else {
    command_q_b.add_raw("update "+name);
    parent->vsx_command_queue_b(this);
  }
}

void vsx_widget_controller_base::cap_value()
{
      if (capmaxv==0) capmaxv=ALMOST_ZERO;
      if (capmax && target_value > capmaxv)
      {
        target_value = capmaxv;
      }
      if (capmin && target_value < capminv)
      {
        target_value = capminv;
      }
}

void vsx_widget_controller_base::event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords) //include last
{
  ++mouse_moves;  
  if (controlling)
  {
    if (remPointer != vsx_input_mouse.position) //don't send unnecessary messages
      // send target_value to the engine or parent widget
      send_to_server();
  }
  allow_move_x = allow_move_y = !owned;
  if (!controlling)
  vsx_widget::event_mouse_move(distance,coords);
}

void vsx_widget_controller_base::event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords)
{
  VSX_UNUSED(distance);
  VSX_UNUSED(coords);
	if (hint) ((vsx_widget_3d_hint*)hint)->popup();
}

void vsx_widget_controller_base::i_draw() //include first, don't forget to add draw_children(); to the end of inheriting method
{
  //interpolation
  float dtime;
  if (r_time == 0)
    dtime = vsx_widget_time::get_instance()->get_dtime();
  else
  {
    dtime = vsx_widget_time::get_instance()->get_time() - r_time;
    r_time = vsx_widget_time::get_instance()->get_dtime();
  }
  {
    if (value != target_value)
    {
      if (smoothness>=0) {
          float tt = dtime*smoothness;
          if (tt > 1.0f) tt = 1.0f;
          value = value*(1.0f-tt)+target_value*tt;
      } else {
        value = target_value;
      }
      cap_value();
    }
  }

  presentation_value = snap(value);

  if (fabs(presentation_value)<=ALMOST_ZERO) presentation_value=0;

  if (!visible) return;
  parentpos = parent->get_pos_p();
  if (drawconnection)
  {
  parentpos = parent->get_pos_p();
  float sx05 = size.x*0.5f;
  float sy05 = size.y*0.5f;

  if (pos.x < -sx05) {
  	glBegin(GL_TRIANGLES);
      vsx_widget_skin::get_instance()->set_color_gl_a(0, 0.8);
      glVertex3f(parentpos.x+sx05+pos.x,parentpos.y+sy05+pos.y, pos.z);
		  glVertex3f(parentpos.x+sx05+pos.x,parentpos.y-sy05+pos.y, pos.z);
      vsx_widget_skin::get_instance()->set_color_gl_a(1, 0.6);
		  glVertex3f(parentpos.x,parentpos.y, pos.z);
  	glEnd();
  } else
  if (pos.x > sx05) {
  	glBegin(GL_TRIANGLES);
      vsx_widget_skin::get_instance()->set_color_gl_a(0, 0.8);
      glVertex3f(parentpos.x-sx05+pos.x,parentpos.y+sy05+pos.y, pos.z);
      glVertex3f(parentpos.x-sx05+pos.x,parentpos.y-sy05+pos.y, pos.z);
      vsx_widget_skin::get_instance()->set_color_gl_a(1, 0.6);
      glVertex3f(parentpos.x,parentpos.y, pos.z);
  	glEnd();
  }

  if (pos.y < -sy05) {
  	glBegin(GL_TRIANGLES);
      vsx_widget_skin::get_instance()->set_color_gl_a(0, 0.8);
      glVertex3f(parentpos.x+sx05+pos.x,parentpos.y+sy05+pos.y, pos.z);
      glVertex3f(parentpos.x-sx05+pos.x,parentpos.y+sy05+pos.y, pos.z);
      vsx_widget_skin::get_instance()->set_color_gl_a(1, 0.6);
      glVertex3f(parentpos.x,parentpos.y, pos.z);
  	glEnd();
  }
  if (pos.y > sy05) {
  	glBegin(GL_TRIANGLES);
      vsx_widget_skin::get_instance()->set_color_gl_a(0, 0.8);
      glVertex3f(parentpos.x+sx05+pos.x,parentpos.y-sy05+pos.y, pos.z);
      glVertex3f(parentpos.x-sx05+pos.x,parentpos.y-sy05+pos.y, pos.z);
      vsx_widget_skin::get_instance()->set_color_gl_a(1, 0.6);
      glVertex3f(parentpos.x,parentpos.y, pos.z);
  	glEnd();
  }
  //PERFORMANCE_MODE_CHANGE if (!performance_mode)
  //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	}
	if (bgcolor.a>0)
	{
       	glBegin(GL_QUADS);
          vsx_widget_skin::get_instance()->set_color_gl(1);
        		glVertex3f(parentpos.x+pos.x-size.x/2, parentpos.y+pos.y+size.y/2,pos.z);
        		glVertex3f(parentpos.x+pos.x+size.x/2, parentpos.y+pos.y+size.y/2,pos.z);
        		glVertex3f(parentpos.x+pos.x+size.x/2, parentpos.y+pos.y-size.y/2,pos.z);
        		glVertex3f(parentpos.x+pos.x-size.x/2, parentpos.y+pos.y-size.y/2,pos.z);
      	glEnd();
  }
  if (a_focus == this) {
    glLineWidth(2.0);
    glColor3f(1,1,1);
    glBegin(GL_LINE_STRIP);
  		glVertex2f(parentpos.x+pos.x+size.x/2,parentpos.y+pos.y-size.y/2);
  		glVertex2f(parentpos.x+pos.x+size.x/2,parentpos.y+pos.y+size.y/2);
  		glVertex2f(parentpos.x+pos.x-size.x/2,parentpos.y+pos.y+size.y/2);
  		glVertex2f(parentpos.x+pos.x-size.x/2,parentpos.y+pos.y-size.y/2);
  		glVertex2f(parentpos.x+pos.x+size.x/2,parentpos.y+pos.y-size.y/2);
    glEnd();
  }
}

//#############################'''''''''##################################
//############################  3D Hint  #################################
//#############################.........##################################

  vsx_widget_3d_hint::vsx_widget_3d_hint()
  {
    color = vsx_color<>(1,1,1,1);
    color.a=0;
    secdelay=4;
  }

  vsx_widget_3d_hint::~vsx_widget_3d_hint()
  {
  }


  void vsx_widget_3d_hint::popup()
  {
    color.a=secdelay+2;
  }

  void vsx_widget_3d_hint::draw()
  {
    if (!visible) return;
    if (color.a>0) color.a -= vsx_widget_time::get_instance()->get_dtime();
    else color.a=0;
    vsx_vector3<> p = parent->get_pos_p()+pos;
    p.y -= size.y*0.5;
    font.color=color;
    font.print_center(vsx_vector3<>(p.x,p.y), title,size.y);
    font.color = vsx_color<>(1,1,1,1);
  }

