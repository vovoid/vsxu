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

#include "vsx_gl_global.h"
#include <iomanip>
#include <map>
#include <list>
#include <vector>
#include <math.h>
#include "vsx_math_3d.h"
#include "vsx_texture_info.h"
#include "vsx_texture.h"
#include "vsx_command.h"
#include "vsx_font.h"
#include "vsxfst.h"
#include "vsx_mouse.h"
#include "vsx_param.h"
#include "vsx_module.h"
// local includes
#include "vsx_widget_base.h"
//#include "vsx_widget_anchor.h"
//#include "vsx_widget_server.h"
//#include "vsx_widget_connector.h"
//#include "vsx_widget_comp.h"
#include "lib/vsx_widget_lib.h"
#include "vsx_timer.h"
#include "vsxfst.h"
#include "vsx_widget_base_controller.h"
//#include "vsx_widget_controller.h"
#include "lib/vsx_widget_panel.h"
#include "lib/vsx_widget_base_edit.h"


using namespace std;

//const float ALMOST_ZERO=0.00001;
//#############################'''''''''''''''##################################
//############################ Base controller #################################
//#############################...............##################################


vsx_widget_base_controller::vsx_widget_base_controller() //include first
{
//  printf("creating controller\n");
	hint = 0;
  r_time = 0;
  //cout << "creating controller\n";
  visible=true; //is control visible?
  widget_type = VSX_WIDGET_TYPE_CONTROLLER;
  controlling = false; //control is currently controlled ny user
  isolate=false; //control is isolated from sending or recieving it's value.
  owned=false; //controll is owner by other control, thus it should behave differently
  color.set(1.0f,1.0f,1.0f,1.0f); //foreground color
  bgcolor.set(1.0f,1.0f,1.0f,1.0f); //background color
  darkshade=0.5;   //dark shade multiplier
  lightshade=0.7; //light shade multiplier
  value=0;    //the current value that the control represents
  smoothness=10;  //how smooth interpolation should be
//  capvalue=false; //keep value between max and min
//  min=-1; //max...
//  max=1; //...and min
  capminv = 0;
  capmaxv = 1;
  capmin = false;
  capmax = false;
  bSnap=false;

  // we want the destvalue
  dest_value = true;
  constrained_x = false;
  constrained_y = false;
  set_pos(vsx_vector(-sizeunit*1.2));
  set_size(vsx_vector(sizeunit, sizeunit));
  drawconnection=true; //draw line between this controll and it's parent
  target_param="";
  param_spec = 0;
  support_interpolation = true;
  help_text = "- right-click to bring up menu to choose\n\
  interpolation speed.\n\
- hit 'c' on the keyboard to close\n\
  all open controllers\n";
}

vsx_widget_base_controller::~vsx_widget_base_controller()
{
  mouse.show_cursor();
}

void vsx_widget_base_controller::init()
{
  if (init_run) return;
  set_value(value);
  //cout <<"init controller, name=" << name << " param=" << target_param << endl;
  //cout << "param spec: " << in_param_spec << endl;
  //printf("in param spec: %s\n",in_param_spec.c_str());
  if (param_spec)
  for (map<vsx_string,vsx_string>::iterator it = param_spec->begin(); it != param_spec->end(); ++it)
  {
    get_in_param_spec((*it));
  }
  init_run = true;
}

void vsx_widget_base_controller::generate_menu()
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

bool vsx_widget_base_controller::key_down(signed long key, bool alt, bool ctrl, bool shift) {
  //printf("key is %d\n",(int)key);
  //if (k_focus) {
    if (key == 'c') delete_();
  //}
  return true;
}

void vsx_widget_base_controller::get_value()
{
  if (!isolate)
  {
  	//printf("command suffix: %s\n", command_suffix.c_str());
    command_q_b.add_raw("pg64 "+target_param + " " + i2s(id)+command_suffix);
    parent->vsx_command_queue_b(this);
  }
}

void vsx_widget_base_controller::get_in_param_spec(pair<vsx_string,vsx_string> parampair)
{
    if (parampair.first=="param") target_param=parampair.second;
    if (parampair.first=="pos_x") pos.x=s2f(parampair.second)*sizeunit;
    if (parampair.first=="pos_y") pos.y=s2f(parampair.second)*sizeunit;
    if (parampair.first=="size_x") size.x=s2f(parampair.second)*sizeunit;
    if (parampair.first=="size_y") size.y=s2f(parampair.second)*sizeunit;
    if (parampair.first=="max") { capmaxv=s2f(parampair.second); capmax=true; }
    if (parampair.first=="min") { capminv=s2f(parampair.second); capmin=true; }
    if (parampair.first=="smooth") smooth(s2f(parampair.second));
}

float vsx_widget_base_controller::snap(float newval)
{
  if (!bSnap) return newval;
  if (shift) return f_round(newval,1);
  if (ctrl) return f_round(newval,2);
  if (alt) return f_round(newval,3);
  return newval;
}

void vsx_widget_base_controller::set_value(float newval,bool cap)
{
  send_value = presentation_value=value=target_value=newval;
  if (cap) cap_value(); //avoid recursion
}

void vsx_widget_base_controller::smooth(float newval)
{
  smoothness=newval;
}

void vsx_widget_base_controller::vsx_command_process_b(vsx_command_s *t)
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
    smooth(s2f(t->cmd_data));
  } else
  if (t->cmd == "pg64_ok" && !isolate) set_value(s2f(base64_decode(t->parts[3])));
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

void vsx_widget_base_controller::event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button)//include last
{
  target_value = snap(target_value);
  bSnap=false;//(shift|alt|ctrl);
  //if (bSnap)
  //send_to_server();
  if (button == 0)
  if (mouse_moves < 5 && last_clicked == this) { //;;(distance.center-mouse_down_pos.center).length() < 0.01f) {

  }
  controlling = false;
  vsx_widget::event_mouse_up(distance,coords,button);
}

void vsx_widget_base_controller::before_delete() {
  if (owned) parent->_delete();
}

void vsx_widget_base_controller::event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  command_q_b.add_raw("delete");
  if (owned) {
    //parent->_delete();
    parent->vsx_command_queue_b(this);
  }else vsx_command_queue_b(this);
  parent->event_mouse_double_click(distance,coords,button);
}

void vsx_widget_base_controller::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button)//include last
{
  mouse_moves = 0;
  bSnap=(shift|alt|ctrl);
  remPointer=mouse.position;
//  world = world - pos;
//  if (!controlling)
//  {
//    world=pos+world+parent->pos;
//    world.z = pos.z;

    //if (parent->type != VSX_WIDGET_TYPE_ANCHOR)
        //parent->event_mouse_down(world,screen,button);
//  }
  if (!owned)
  {
    smx = 0;
    smy = 0;
  }
  vsx_widget::event_mouse_down(distance,coords,button);
}

void vsx_widget_base_controller::send_to_server() {
  send_value = target_value;
  if (bSnap) send_value = snap(send_value);

//  if (!bSnap || old_snap_value != target_value)
  {
    //printf("trying to send %f\n",send_value);
    if (!isolate)
    {
      if (smoothness >= 0) {
        command_q_b.add_raw("param_set_interpolate "+ f2s(send_value)  + " " + f2s(smoothness) + " "+target_param);
      }
      else command_q_b.add_raw("param_set "+ f2s(send_value) + " "+target_param);
      parent->vsx_command_queue_b(this);
    } else {
      command_q_b.add_raw("update "+name);
      parent->vsx_command_queue_b(this);
    }
  }
}

void vsx_widget_base_controller::cap_value()
{
      if (capmaxv==0) capmaxv=ALMOST_ZERO;
      if (capmax && target_value > capmaxv) {
        //printf("capmax hit!\n");
        target_value = capmaxv;//set_value(capmaxv,false);
      }
      if (capmin && target_value < capminv) {
        target_value = capminv;//set_value(capminv,false);
        //printf("capmin hit!\n");
      }
}

void vsx_widget_base_controller::event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords) //include last
{
  ++mouse_moves;
  bSnap=(shift|alt|ctrl);
  if (controlling)
  {
      if (remPointer!=mouse.position) //don't send unnecessary messages
        // send target_value to the engine or parent widget
        send_to_server();
  } //else
/*  if (!owned)
  {
    if (smx)
    {
      vsx_vector uu = distance.center;
      uu.x -= smx;
      uu.y -= smy;
      uu.z = 0;
      move_d(uu);
    }
    smx = distance.center.x;
    smy = distance.center.y;
  }*/
//!!!  else

  //if (!owned)
  allow_move_x = allow_move_y = !owned;
  if (!controlling)
  vsx_widget::event_mouse_move(distance,coords);
}

void vsx_widget_base_controller::event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords)
{
	if (hint) ((vsx_widget_3d_hint*)hint)->popup();

  bSnap=(shift|alt|ctrl);
}

void vsx_widget_base_controller::i_draw() //include first, don't forget to add draw_children(); to the end of inheriting method
{
  //interpolation
  float dtime;
  if (r_time == 0) dtime = this->dtime; else
  {
    dtime = time - r_time;
    r_time = time;
  }
  if (!bSnap) {
    if (value!=target_value)
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
  } else {
    value = target_value;
    cap_value();
  }


  presentation_value = snap(value);

  //
  //else presentation_value=value;

  if (fabs(presentation_value)<=ALMOST_ZERO) presentation_value=0;

  if (!visible) return;
  parentpos = parent->get_pos_p();
  if (drawconnection)
  {
  parentpos = parent->get_pos_p();
  float sx05 = size.x*0.5f;
  float sy05 = size.y*0.5f;

    /*glColor3f(1,1,1);
    glLineWidth(2);
  	glBegin(GL_LINES);
 	   	  glVertex3f(parentpos.x,parentpos.y,parent->pos.z);
 	   	  glVertex3f(parentpos.x+pos.x,parentpos.y+pos.y,pos.z);
  	glEnd();*/
	//PERFORMANCE_MODE_CHANGE if (!performance_mode)
  //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
  if (pos.x < -sx05) {
  	glBegin(GL_TRIANGLES);
    glColor4f(skin_color[0].r,skin_color[0].g,skin_color[0].b,0.8f*skin_color[0].a);
		  glVertex3f(parentpos.x+sx05+pos.x,parentpos.y+sy05+pos.y, pos.z);
		  glVertex3f(parentpos.x+sx05+pos.x,parentpos.y-sy05+pos.y, pos.z);
  glColor4f(skin_color[1].r,skin_color[1].g,skin_color[1].b,0.6*skin_color[1].a);
		  glVertex3f(parentpos.x,parentpos.y, pos.z);
  	glEnd();
  } else
  if (pos.x > sx05) {
  	glBegin(GL_TRIANGLES);
    glColor4f(skin_color[0].r,skin_color[0].g,skin_color[0].b,0.8*skin_color[0].a);
		  glVertex3f(parentpos.x-sx05+pos.x,parentpos.y+sy05+pos.y, pos.z);
		  glVertex3f(parentpos.x-sx05+pos.x,parentpos.y-sy05+pos.y, pos.z);
  glColor4f(skin_color[1].r,skin_color[1].g,skin_color[1].b,0.6*skin_color[1].a);
		  glVertex3f(parentpos.x,parentpos.y, pos.z);
  	glEnd();
  }

  if (pos.y < -sy05) {
  	glBegin(GL_TRIANGLES);
    glColor4f(skin_color[0].r,skin_color[0].g,skin_color[0].b,0.8f*skin_color[0].a);
		  glVertex3f(parentpos.x+sx05+pos.x,parentpos.y+sy05+pos.y, pos.z);
		  glVertex3f(parentpos.x-sx05+pos.x,parentpos.y+sy05+pos.y, pos.z);
  glColor4f(skin_color[1].r,skin_color[1].g,skin_color[1].b,0.6*skin_color[1].a);
		  glVertex3f(parentpos.x,parentpos.y, pos.z);
  	glEnd();
  }
  if (pos.y > sy05) {
  	glBegin(GL_TRIANGLES);
    glColor4f(skin_color[0].r,skin_color[0].g,skin_color[0].b,0.8*skin_color[0].a);
		  glVertex3f(parentpos.x+sx05+pos.x,parentpos.y-sy05+pos.y, pos.z);
		  glVertex3f(parentpos.x-sx05+pos.x,parentpos.y-sy05+pos.y, pos.z);
  glColor4f(skin_color[1].r,skin_color[1].g,skin_color[1].b,0.6*skin_color[1].a);
		  glVertex3f(parentpos.x,parentpos.y, pos.z);
  	glEnd();
  }
  //PERFORMANCE_MODE_CHANGE if (!performance_mode)
  //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	}
	if (bgcolor.a>0)
	{
       	glBegin(GL_QUADS);
       	  skin_color[1].gl_color();
         	  //glColor4f(darkshade*bgcolor.r,darkshade*bgcolor.g,darkshade*bgcolor.b,bgcolor.a);
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
//    bgcolor.a=0;
  	color = vsx_color__(1,1,1,1);
    color.a=0;
    secdelay=4;
  }

  vsx_widget_3d_hint::~vsx_widget_3d_hint()
  {
//  	printf("hint destructor\n");
  }


  void vsx_widget_3d_hint::popup()
  {
    color.a=secdelay+2;
  }

  void vsx_widget_3d_hint::draw()
  {
    if (!visible) return;
    if (color.a>0) color.a-=dtime;
    else color.a=0;
    vsx_vector p = parent->get_pos_p()+pos;
    p.y -= size.y*0.5;
    myf.color=color;
    //print_center(vsx_vector p, const vsx_string& str, float size);
    //if (color.a > 0) printf("drawing %s",title.c_str());
    myf.print_center(vsx_vector(p.x,p.y), title,size.y);
    myf.color = vsx_color__(1,1,1,1);
    //vsx_widget_3d_label::draw();
  }

