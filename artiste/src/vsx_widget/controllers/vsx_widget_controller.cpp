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

#ifndef VSX_NO_CLIENT
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
#include "vsx_widget_controller.h"
#include "lib/vsx_widget_panel.h"
#include "lib/vsx_widget_base_edit.h"


//#############################''''''##################################
//############################  Knob  #################################
//#############################......##################################

vsx_widget_knob::vsx_widget_knob()
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

void vsx_widget_knob::init()
{
  if (init_run) return;
  vsx_widget_base_controller::init();
  get_value();
  editor = add(new vsx_widget_base_edit,name+".label");
  editor->init();
  editor->render_type = VSX_WIDGET_RENDER_3D;
  editor->set_size(vsx_vector(size.x,size.y*0.23f));
  editor->set_pos(vsx_vector(0,size.y*0.5f+size.y*0.25f-editor->size.y*0.6f));
  editor->set_font_size(0.214286 * size.y); // sizeunit = 0.014f
  ((vsx_widget_base_edit*)editor)->allowed_chars = "0123456789.-";
  ((vsx_widget_base_edit*)editor)->command_prefix = "u";
  ((vsx_widget_base_edit*)editor)->size_from_parent = true;
  ((vsx_widget_base_edit*)editor)->single_row = true;

  hint=add(new vsx_widget_3d_hint,name+".hint");
  hint->size.y=size.y*0.15;
  hint->size.x=size.x;
  //((vsx_widget_3d_hint*)hint)->numchars=8;
  ((vsx_widget_3d_hint*)hint)->title=target_param;
  ((vsx_widget_3d_hint*)hint)->init();
}

void vsx_widget_knob::on_delete() {
  mouse.show_cursor();
}

  //void vsx_widget_knob::get_in_param_spec(pair<vsx_string,vsx_string> parampair)
  //{
//    vsx_widget_base_controller::get_in_param_spec(parampair);
//    if (parampair.first=="hint") ((vsx_widget_3d_hint*)hint)->title=parampair.second;
//  }

  void vsx_widget_knob::event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button)
  {
    bSnap=(shift|alt|ctrl);
    if (controlling && button == 0)
    {
      mouse.show_cursor();
      mouse.set_cursor_pos(remPointer.x,remPointer.y);
    }
    vsx_widget_base_controller::event_mouse_up(distance, coords, button);
  }

  void vsx_widget_knob::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button)
  {
    bSnap=(shift|alt|ctrl);
    remWorld=distance.center;
    //!glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
    vsx_vector a;
    a.x=distance.center.x/size.x;
    a.y=distance.center.y/size.y;
    if (a.length() <0.5 && button == 0)
    {
      controlling = true;
      mouse.hide_cursor();
    }
    vsx_widget_base_controller::event_mouse_down(distance, coords, button);
  }

  void vsx_widget_knob::event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords)
  {
    bSnap=(shift|alt|ctrl);
    if (mouse.position != remPointer && controlling)
    {
      deltamove=(distance.center-remWorld)/size;
      if (fabs(deltamove.x)>fabs(deltamove.y)) deltamove.y=0.0f; else deltamove.x=0.0f;
      target_value += (((deltamove.y)+((deltamove.x)/15.0f))*0.5*multiplier);
      cap_value();
      vsx_widget_base_controller::event_mouse_move(distance, coords);
      //mouse_moves++;
      mouse.set_cursor_pos(remPointer.x,remPointer.y);
    } else
    vsx_widget_base_controller::event_mouse_move(distance, coords);
  }

  void vsx_widget_knob::vsx_command_process_b(vsx_command_s *t) {
    if (t->cmd == "u") {
      target_value = s2f(t->cmd_data);
      cap_value();
      send_to_server();
      k_focus = this;
    } else vsx_widget_base_controller::vsx_command_process_b(t);
  }

  void vsx_widget_knob::draw()
  {
    if (!visible) return;

    // deal with the eventual label text updates
    /*if (((vsx_widget_3d_label*)label)->changed)
    {
      set_value(s2f(((vsx_widget_3d_label*)label)->title));
      cap_value();
      // send to engine this new value
      send_to_server();
      ((vsx_widget_3d_label*)label)->changed=false;
    }*/

    vsx_widget_base_controller::draw();

    presentation_value /= multiplier;
    if (k_focus != editor)
    ((vsx_widget_base_edit*)editor)->set_string(f2s(presentation_value,15));
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


//##########################'''''''''''''''''###############################
//######################### Slider controller  ##############################
//##########################.................###############################


  vsx_widget_slider::vsx_widget_slider()
  {
    generate_menu();
    menu->init();
    color.set(85.0f/255.0f,170.0f/255.0f,1,1);
    set_size(vsx_vector(sizeunit, sizeunit* 4.0f));
    handlesize=0.5*sizeunit;
    value=0.0f;target_value=0.0f;
    capminv=0;
    capmaxv=1;
    capmin = false;
    capmax = false;
    amp=1;
    ofs=0;
//    cap = false;
  }

  void vsx_widget_slider::init()
  {
    if (init_run) return;
    vsx_widget_base_controller::init();
    get_value();
    //hint=add(new vsx_widget_3d_hint,name+".hint");
    //hint->size.y = sizeunit*0.25;
    //hint->size.x = sizeunit;
    //((vsx_widget_3d_hint*)hint)->numchars=8;
    //((vsx_widget_3d_hint*)hint)->title=target_param;
    //((vsx_widget_3d_hint*)hint)->init();

    editor = add(new vsx_widget_base_edit,name+".edit");
    editor->init();
    editor->render_type = VSX_WIDGET_RENDER_3D;
    editor->set_size(vsx_vector(sizeunit,sizeunit*0.23f));
    editor->set_pos(vsx_vector(0,size.y*0.5f+sizeunit*0.125f));
    editor->set_font_size(0.003);

    /*label=add(new vsx_widget_3d_label,name+".label");
    label->size.y=sizeunit*0.25;
    label->size.x=sizeunit;
    ((vsx_widget_3d_label*)label)->editable=true;
    ((vsx_widget_3d_label*)label)->numchars=8;
    ((vsx_widget_3d_label*)label)->title=value;
    ((vsx_widget_3d_label*)label)->init();*/
  }

  void vsx_widget_slider::cap_value()
  {
    if (capmax)
    //if (target_value > ofs+amp)
    if (ofs > capmaxv) {
      //amp = capmaxv-ofs;
      ofs = capmaxv;
    }
    if (capmin)
    if (ofs < capminv) ofs = capminv;
    if (amp<ALMOST_ZERO) amp=ALMOST_ZERO;
    if (target_value > amp+ofs) target_value = amp+ofs;//set_value(amp+ofs,false);
    if (target_value < ofs) target_value = ofs;//set_value(ofs,false);
    vsx_widget_base_controller::cap_value();
  }

int vsx_widget_slider::inside_xy_l(vsx_vector &test, vsx_vector &global)
{
  float ypos = getYPos();
  if (coord_type == VSX_WIDGET_COORD_CENTER) {
    if (
       (test.x > global.x-target_size.x*0.5f) &&
       (test.x < global.x+target_size.x*0.5f) &&
       (test.y > global.y+ypos-handlesize/2) &&
       (test.y < global.y+ypos+handlesize/2)
    ) {
       return coord_type;
    }
  }
  return 0;
}

void vsx_widget_slider::vsx_command_process_b(vsx_command_s *t) {
  if (owned && t->cmd == "si") { vsx_widget::vsx_command_process_b(t); }
  else
  if (t->cmd == "u") {
    target_value = s2f(t->cmd_data);
    cap_value();
    send_to_server();
    k_focus = this;
  } else vsx_widget_base_controller::vsx_command_process_b(t);
}

  void vsx_widget_slider::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button)
  {
    controlling = true;
    bSnap=(shift|alt|ctrl);
    vsx_widget_base_controller::event_mouse_down(distance, coords, button);
  }
  void vsx_widget_slider::event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords)
  {
    VSX_UNUSED(coords);

    bSnap = ( shift | alt | ctrl );
    if (controlling)
    {
      target_value = (-(((distance.corner.y-handlesize/2)/(size.y-handlesize))-1) * (amp))+ofs; //amp-ofs
      cap_value();
    }
    send_to_server();
    ++mouse_moves;
    //vsx_widget_base_controller::event_mouse_move(distance, coords);
  }

  double vsx_widget_slider::getYPos()
  {
    return (((presentation_value-ofs)/(amp))*(size.y-handlesize))-size.y/2.0f+handlesize/2;//amp-ofs
//    return (((presentation_value-ofs)/(amp))*(size.y-handlesize)+(handlesize/2)-size.y/2);//amp-ofs
  }

  void vsx_widget_slider::draw()
  {
    if (!visible) return;

    // deal with the eventual label text updates
    /*if (((vsx_widget_3d_label*)label)->changed)
    {
      set_value(s2f(((vsx_widget_3d_label*)label)->title));
      //cout << value << endl;
      cap_value();
      // send to engine this new value
      send_to_server();
      ((vsx_widget_3d_label*)label)->changed=false;
    }*/

    vsx_widget_base_controller::draw();
    glLineWidth(1.0f);
    float ypos=parentpos.y+pos.y+getYPos();
    //((vsx_widget_3d_label*)label)->pos.y=size.y/2+((vsx_widget_3d_label*)label)->size.y/2;
    ((vsx_widget_base_edit*)editor)->set_string(f2s(presentation_value,15));
    //((vsx_widget_3d_hint*)hint)->pos.y=getYPos();//((presentation_value-min)/(max-min))*(size.y-handlesize)+0.5*handlesize-size.y/2;
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

//#############################'''''''''##################################
//############################  channel    ###############################
//#############################.........##################################

vsx_widget_channel::vsx_widget_channel()
{
  generate_menu();
  menu->init();
  target_size.y = size.y=sizeunit*6.75;
}

void vsx_widget_channel::init()
{
  if (init_run) return;
  vsx_widget_base_controller::init();
  get_value();
  amp = add(new vsx_widget_knob,"amp");
  amp->set_pos(vsx_vector(0,pos.y+size.y/2-amp->size.y*0.75));
  //amp->pos.y=pos.y+size.y/2-amp->size.y*0.75;
  //amp->pos.x=0;
  ((vsx_widget_knob*)amp)->bgcolor.a=0;
  ((vsx_widget_knob*)amp)->owned=true;
  ((vsx_widget_knob*)amp)->drawconnection=false;
  ((vsx_widget_knob*)amp)->isolate=true;
  ((vsx_widget_knob*)amp)->init();
  ((vsx_widget_knob*)amp)->capmin = true;
  ((vsx_widget_knob*)amp)->capminv = ALMOST_ZERO;
  //((vsx_widget_3d_hint*)((vsx_widget_knob*)amp)->hint)->title="Amp";

  offset = add(new vsx_widget_knob,"offset");
  offset->set_pos(vsx_vector(0,amp->pos.y-offset->size.y-offset->size.y*0.25));
  //offset->pos.y=amp->pos.y-offset->size.y-offset->size.y*0.25;
  //offset->pos.x=0;
  ((vsx_widget_knob*)offset)->bgcolor.a=0;
  ((vsx_widget_knob*)offset)->owned=true;
  ((vsx_widget_knob*)offset)->drawconnection=false;
  ((vsx_widget_knob*)offset)->isolate=true;
  ((vsx_widget_knob*)offset)->init();
  //((vsx_widget_3d_hint*)((vsx_widget_knob*)offset)->hint)->title="Ofs";

  slider = add(new vsx_widget_slider,"slider");
  slider->set_pos(vsx_vector(0,offset->pos.y-(offset->size.y/2)-(slider->size.y/2)-(sizeunit/4)));
  //slider->pos.y=offset->pos.y-(offset->size.y/2)-(slider->size.y/2)-(sizeunit/4);
  //slider->pos.x=0;
  ((vsx_widget_slider*)slider)->bgcolor.a=0;
  ((vsx_widget_slider*)slider)->owned=true;
  ((vsx_widget_slider*)slider)->drawconnection=false;
  ((vsx_widget_slider*)slider)->isolate=true;
  ((vsx_widget_slider*)slider)->value=value;
  ((vsx_widget_slider*)slider)->init();
  ((vsx_widget_slider*)slider)->capmin = capmin;
  ((vsx_widget_slider*)slider)->capmax = capmax;
  ((vsx_widget_slider*)slider)->capminv = capminv;
  ((vsx_widget_slider*)slider)->capmaxv = capmaxv;
  //((vsx_widget_3d_hint*)((vsx_widget_slider*)slider)->hint)->title=target_param;

  double x=((vsx_widget_slider*)slider)->amp,i=((vsx_widget_slider*)slider)->ofs;
  ((vsx_widget_knob*)offset)->set_value(i);
  ((vsx_widget_knob*)amp)->set_value(x-i);
  smooth(smoothness);

  if (!isolate) command_q_b.add_raw("param_get "+target_param + " " + i2s(slider->id));
}

void vsx_widget_channel::smooth(float newval)
{
  smoothness=newval;
  if (init_run) {
    ((vsx_widget_knob*)amp)->smooth(newval);
    ((vsx_widget_knob*)offset)->smooth(newval);
    ((vsx_widget_knob*)slider)->smooth(newval);
  }
}

void vsx_widget_channel::vsx_command_process_b(vsx_command_s *t)
{
  if (t->cmd=="pg64_ok" && !isolate) {
    set_value(s2f(base64_decode(t->parts[3])));
    //((vsx_widget_slider*)slider)->set_value(s2f(t->parts[3]));
    //value = target_value = ((vsx_widget_slider*)slider)->target_value;
    if (target_value > ((vsx_widget_slider*)slider)->amp)
    {
      ((vsx_widget_knob*)amp)->target_value = ((vsx_widget_slider*)slider)->amp = target_value*2;
    }
    if (target_value < ((vsx_widget_slider*)slider)->ofs) {
      ((vsx_widget_knob*)offset)->value = ((vsx_widget_knob*)offset)->target_value = ((vsx_widget_slider*)slider)->ofs = target_value*2;
      ((vsx_widget_knob*)amp)->value = ((vsx_widget_knob*)amp)->target_value = ((vsx_widget_slider*)slider)->amp = ((vsx_widget_knob*)amp)->target_value-target_value*2;
    }
    ((vsx_widget_slider*)slider)->set_value(value);
  } else
  if (t->cmd == "set_value_i") {
    //target_value = value;
    //value = target_value = ((vsx_widget_slider*)slider)->target_value;
    if (value > ((vsx_widget_slider*)slider)->amp)
    {
      ((vsx_widget_knob*)amp)->target_value = ((vsx_widget_slider*)slider)->amp = target_value*2;
    }
    if (value < ((vsx_widget_slider*)slider)->ofs) {
      ((vsx_widget_knob*)offset)->value = ((vsx_widget_knob*)offset)->target_value = ((vsx_widget_slider*)slider)->ofs = target_value*2;
      ((vsx_widget_knob*)amp)->value = ((vsx_widget_knob*)amp)->target_value = ((vsx_widget_slider*)slider)->amp = ((vsx_widget_knob*)amp)->target_value-target_value*2;
    }
    ((vsx_widget_slider*)slider)->set_value(value);
//    set_value(((vsx_widget_slider*)slider)->value);
  } else
  if (t->cmd == "update") {
    //printf("update %s\n",t->parts[1].c_str());
    if (t->parts[1] == "amp") {
      ((vsx_widget_slider*)slider)->amp=((vsx_widget_knob*)amp)->send_value;//+((vsx_widget_knob*)offset)->target_value;
      ((vsx_widget_slider*)slider)->ofs=((vsx_widget_knob*)offset)->send_value;
      ((vsx_widget_slider*)slider)->cap_value();
      ((vsx_widget_slider*)slider)->value = ((vsx_widget_slider*)slider)->target_value;
    	if (((vsx_widget_slider*)slider)->value != ((vsx_widget_slider*)slider)->target_value)
    	{
        set_value(((vsx_widget_slider*)slider)->target_value);
        if (((vsx_widget_knob*)amp)->smoothness>=0) command_q_b.add_raw("param_set_interpolate "+ f2s(((vsx_widget_slider*)slider)->target_value)  + " " + f2s(((vsx_widget_knob*)amp)->smoothness) + " "+target_param);
        else command_q_b.add_raw("param_set "+ f2s(((vsx_widget_slider*)slider)->target_value) + " "+target_param);
        parent->vsx_command_queue_b(this);
      }
    } else
    if (t->parts[1] == "offset") {
      ((vsx_widget_slider*)slider)->amp=((vsx_widget_knob*)amp)->send_value;//+((vsx_widget_knob*)offset)->target_value;
      ((vsx_widget_slider*)slider)->ofs=((vsx_widget_knob*)offset)->send_value;
      ((vsx_widget_slider*)slider)->cap_value();
      //printf("slider ofs: %f -- ofs ofs: %f\n",((vsx_widget_slider*)slider)->ofs,((vsx_widget_knob*)offset)->send_value);
      ((vsx_widget_knob*)offset)->target_value = ((vsx_widget_slider*)slider)->ofs;
      ((vsx_widget_slider*)slider)->value = ((vsx_widget_slider*)slider)->target_value;
    	if (value != ((vsx_widget_slider*)slider)->target_value)
    	{
        //target_value = value = ((vsx_widget_slider*)slider)->value;
        set_value(((vsx_widget_slider*)slider)->target_value);
        if (((vsx_widget_knob*)offset)->smoothness>=0) command_q_b.add_raw("param_set_interpolate "+ f2s(((vsx_widget_slider*)slider)->target_value)  + " " + f2s(((vsx_widget_knob*)offset)->smoothness) + " "+target_param);
        else command_q_b.add_raw("param_set "+ f2s(((vsx_widget_slider*)slider)->target_value) + " "+target_param);
        parent->vsx_command_queue_b(this);
      }
    } else
    if (t->parts[1] == "slider") {
      //printf("slider update\n");
      //printf("smoothing: %f\n",smoothness);
    	if (value!=((vsx_widget_slider*)slider)->target_value)
    	{
        //target_value = value = ((vsx_widget_slider*)slider)->value;
        //printf("set value: %f\n",((vsx_widget_slider*)slider)->target_value);
        set_value(((vsx_widget_slider*)slider)->target_value);
        smoothness = ((vsx_widget_slider*)slider)->smoothness;
        if (smoothness>=0) command_q_b.add_raw("param_set_interpolate "+ f2s(((vsx_widget_slider*)slider)->target_value)  + " " + f2s(smoothness) + " "+target_param);
        else command_q_b.add_raw("param_set "+ f2s(((vsx_widget_slider*)slider)->target_value) + " "+target_param);
        parent->vsx_command_queue_b(this);
      }
    }
    ((vsx_widget_slider*)slider)->cap_value();
    return;
  } else
  vsx_widget_base_controller::vsx_command_process_b(t);
}

void vsx_widget_channel::draw()
{
  if (!visible) return;
  vsx_widget_base_controller::draw();
  draw_children();
}

//#############################'''''''##################################
//############################  multi  #################################
//############################ channel #################################
//#############################.......##################################


vsx_widget_mixer::vsx_widget_mixer()
{
  generate_menu();
  menu->init();
  nummixers=4;
}

void vsx_widget_mixer::init()
{
  if (init_run) return;
  vsx_widget_base_controller::init();
  set_size(vsx_vector(sizeunit*nummixers,sizeunit*6.75));
  target_pos.x = (pos.x -= size.x*0.5);

  for (int t=0;t<nummixers;++t)
  {
    vsx_widget* newmixer=add(new vsx_widget_channel,name+".channel"+i2s(nummixers-t));
    newmixer->set_pos(vsx_vector(-size.x*0.5+sizeunit*0.5 + (float)t*sizeunit));
    //newmixer->pos.x = -size.x*0.5+sizeunit*0.5;
    //newmixer->pos.x+=(float)t*sizeunit;
    ((vsx_widget_channel*)newmixer)->owned=true;
    ((vsx_widget_channel*)newmixer)->drawconnection=false;
    ((vsx_widget_channel*)newmixer)->isolate=true;
    ((vsx_widget_channel*)newmixer)->init();
    //((vsx_widget_3d_hint*)((vsx_widget_slider*)((vsx_widget_channel*)newmixer)->slider)->hint)->title=target_param+'['+i2s(t)+']';
    mixers.push_back(newmixer);
  }
  std::vector<vsx_string> parts;
  vsx_string deli = ";";
  explode(capmaxv_s,deli,parts);
  for (unsigned int i = 0; i < parts.size(); ++i) {
    if (parts[i] != "x" && parts[i] != "") {
      ((vsx_widget_slider*)((vsx_widget_channel*)mixers[i])->slider)->capmax = true;
      ((vsx_widget_slider*)((vsx_widget_channel*)mixers[i])->slider)->capmaxv = s2f(parts[i]);
    }
  }
  parts.clear();
  explode(capminv_s,deli,parts);
  for (unsigned int i = 0; i < parts.size(); ++i) {
    if (parts[i] != "x" && parts[i] != "") {
      ((vsx_widget_slider*)((vsx_widget_channel*)mixers[i])->slider)->capmin = true;
      ((vsx_widget_slider*)((vsx_widget_channel*)mixers[i])->slider)->capminv = s2f(parts[i]);
    }
  }
  smooth(smoothness);
  get_value();
}

void vsx_widget_mixer::smooth(float newval)
{
  for (unsigned long i=0;i<mixers.size(); ++i)
    ((vsx_widget_channel*)mixers[i])->smooth(newval);
}

void vsx_widget_mixer::get_in_param_spec(std::pair<vsx_string,vsx_string> parampair)
{
  //printf("grombleton\n");
  if (parampair.first=="max") { capmaxv_s=parampair.second; capmax=true; } else
  if (parampair.first=="min") { capminv_s=parampair.second; capmin=true; } else
  vsx_widget_base_controller::get_in_param_spec(parampair);
}


void vsx_widget_mixer::draw()
{
  if (!visible) return;
  vsx_widget_base_controller::draw();
  //draw_children();
}

void vsx_widget_mixer::vsx_command_process_b(vsx_command_s *t)
{
  if (t->cmd == "pg64_ok") {
    //printf("param_get_ok\n");
    std::vector<vsx_string> parts;
    vsx_string deli = ",";
    t->parts[3] = base64_decode(t->parts[3]);
    explode(t->parts[3],deli, parts);
    for (unsigned long i = 0; i < parts.size(); ++i) {
      //printf("setting value\n");
      ((vsx_widget_channel*)mixers[i])->set_value(s2f(parts[i]));
      command_q_b.add_raw("set_value_i");
      ((vsx_widget_channel*)mixers[i])->vsx_command_queue_b(this);
    }
  } else
  if (t->cmd == "param_set_interpolate" || t->cmd == "param_set")
  {
    vsx_string cmd="";
    for (unsigned long i=0;i<mixers.size();++i)
    {
      cmd+=f2s(((vsx_widget_channel*)mixers[i])->target_value);
      if (i<mixers.size()-1) cmd+=",";
    }
//    cout <<cmd << endl;
    command_q_b.add_raw(t->cmd + " " +cmd+ " " + t->parts[t->parts.size()-1]);
    parent->vsx_command_queue_b(this);
    return;
  } else
  vsx_widget_base_controller::vsx_command_process_b(t);
}


//#############################'''''''''##################################
//############################ 3D label  #################################
//#############################.........##################################

/*  vsx_widget_3d_label::vsx_widget_3d_label()
  {
    pos.x=0;
    type = VSX_WIDGET_TYPE_3D_LABEL;
    halign = 0;
    numchars=0;
    color.set(1,1,1,1);
    cursorpos=0;
    editable=false;
    editing=false;
    changed=false;
    allowedchars="-1234567890.e";
  }

  vsx_widget_3d_label::~vsx_widget_3d_label() {
    //printf("vsx_widget_3d_label::~vsx_widget_3d_label()\n");
  }

  bool vsx_widget_3d_label::inside_xyd(vsx_vector world, vsx_vector screen)
  {
    return editable;//!!! && vsx_widget::inside_xyd(world, screen);
  }

  void vsx_widget_3d_label::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button)
  {
//    vsx_widget_3d_grid_comp::event_mouse_down(world,screen,button);
    if (button == GLUT_RIGHT_BUTTON && editable)
    {
      k_focus=this;
      a_focus=this;
      editing=true;
      editvalue=title;
      cursorpos=editvalue.size();
      changed=false;
    }
    else
    parent->event_mouse_down(distance,coords,button);
  }

  bool vsx_widget_3d_label::event_key_down(signed long key, bool alt, bool ctrl, bool shift)
  {
    if (!editable || !editing) return true;
    switch(key)
    {
    // arrow left
    case -100:
      --cursorpos;
      if (cursorpos<0) cursorpos=0;
      break;
    // arrow right
    case -102:
      ++cursorpos;
      if (cursorpos>editvalue.size()) cursorpos=editvalue.size();
      break;
    case 8:
        --cursorpos;
        if (cursorpos<0) cursorpos=0;
    case 127:
        editvalue.erase(cursorpos,1);// = str_replace(cursorpos,1,"");
      break;
    case 13:
        title=editvalue;
        changed=true;
        editing=false;
      break;
    default:
      //string in(1,(char)key);
      if (allowedchars.find(vsx_string((char)key)) != -1 && editvalue.size() < numchars+2)
      {
        editvalue.insert(cursorpos,(char)key);
        ++cursorpos;
      }
      break;
    }
    return false;
  }

  void vsx_widget_3d_label::draw()
  {
    if (editing && !((k_focus==this) && (a_focus==this)))
    {
        title=editvalue;
        changed=true;
        editing=false;
    }
    if (!visible) return;
  	if (bgcolor.a > 0)
  	{
      vsx_vector parentpos;
      parentpos = parent->get_pos_p();
     	glBegin(GL_QUADS);
       	  glColor4f(bgcolor.r,bgcolor.g,bgcolor.b,bgcolor.a);
      		glVertex3f(parentpos.x+pos.x-size.x/2, parentpos.y+pos.y+size.y/2,pos.z);
      		glVertex3f(parentpos.x+pos.x+size.x/2, parentpos.y+pos.y+size.y/2,pos.z);
      		glVertex3f(parentpos.x+pos.x+size.x/2, parentpos.y+pos.y-size.y/2,pos.z);
      		glVertex3f(parentpos.x+pos.x-size.x/2, parentpos.y+pos.y-size.y/2,pos.z);
    	glEnd();
    }
  	if (color.a>0)
  	{
      myf.color=color;
      vsx_vector p = parent->get_pos_p()+pos;
      vsx_string t="";
      if (editable && editing) t=editvalue.substr(0,numchars+2);
      else t=title.substr(0,numchars+2);
      cursor = "";
      for (int i = 0; i < cursorpos; ++i) cursor.push_back(' ');
      cursor.push_back('_');
      if (editable && editing) t+=" ";

      switch (halign) {
//        case -1:
//          myf.print(vsx_vector(p.x,p.y-size.y*0.5,parent->pos.z), t,"ascii",size.y);
  //        if (editable && editing)
//          myf.print(vsx_vector(p.x,p.y-size.y*0.5,parent->pos.z), cursor,"ascii",size.y);
  //      break;
        case 0:
          if (editable && editing) {
            myf.print(vsx_vector(p.x-size.x*0.5,p.y-size.y*0.5), t,"ascii",size.y);
            myf.print(vsx_vector(p.x-size.x*0.5,p.y-size.y*0.5), cursor,"ascii",size.y);
          } else {
            myf.print_center(vsx_vector(p.x,p.y-size.y*0.5), t,"ascii",size.y);
          }
        break;
  //7      case 1:
  //      break;
      }
    }
  }
*/
//#############################'''''''''##################################
//############################  dialog   #################################
//#############################.........##################################


void vsx_widget_dialog::draw() //include first, don't forget to add draw_children(); to the end of inheriting method
{
  vsx_widget_base_controller::draw();
  draw_children();
}

void vsx_widget_dialog::init()
{
  if (init_run) return;
  vsx_widget_base_controller::init();
  init_children();
}

void vsx_widget_dialog::get_in_param_spec(std::pair<vsx_string,vsx_string> parampair)
{
    vsx_widget_base_controller::get_in_param_spec(parampair);
    if (parampair.first.substr(0,8)=="control_")
    {
      vsx_widget* newwidget;
      vsx_string type=parampair.first.substr(8);
      if (type=="knob")
      newwidget=add(new vsx_widget_knob,name+".knob");
      else if (type=="slider")
      newwidget=add(new vsx_widget_slider,name+".slider");

      ((vsx_widget_base_controller*)newwidget)->owned=true;
      ((vsx_widget_base_controller*)newwidget)->drawconnection=false;
      ((vsx_widget_base_controller*)newwidget)->in_param_spec=parampair.second;
      ((vsx_widget_base_controller*)newwidget)->init();
    }
}
#endif
