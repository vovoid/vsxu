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
#include <map>
#include <list>
#include <vector>
#include <math.h>
#include "vsx_math_3d.h"
#include "vsx_texture_info.h"
#include "vsx_texture.h"
#include "vsx_command.h"
#include "vsx_font.h"
// local includes
#include "vsx_widget_base.h"
#include "lib/vsx_widget_lib.h"
#include "dialogs/vsx_widget_window_statics.h"
#include "vsx_widget_desktop.h"
#include "vsx_widget_object_inspector.h"
#include "vsxfst.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include "vsx_timer.h"
#include "vsx_engine.h"
#include "lib/vsx_widget_panel.h"
#include "lib/vsx_widget_base_edit.h"
#include "vsx_widget_assistant.h"

// -------------------------------------------------------------------------------------------------------------------
//-- VSXU ASSISTANT -----------------------------------------------------------------------------------------------
#ifndef VSXU_PLAYER
//void vsxu_assistant::draw() {
  /*if (cur_focus != a_focus) {
    vsx_vector pp;
    pp = a_focus->get_pos_p();
    endpoint.x = pp.x;
    endpoint.y = pp.y;
    endsize.x = a_focus->size.x*0.6;
    endsize.y = a_focus->size.y*0.6;
  }
  //glMatrixMode(GL_MODELVIEW);
	//glPushMatrix();
	//glLoadIdentity();
	float tt = dtime*2;
  if (tt > 1) tt = 1;
	clickpoint.x = clickpoint.x*(1-tt)+endpoint.x*tt;
	clickpoint.y = clickpoint.y*(1-tt)+endpoint.y*tt;

	cursize.x = cursize.x*(1-tt)+endsize.x*tt;
	cursize.y = cursize.y*(1-tt)+endsize.y*tt;

 	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	((GrappaRenderer*)grappa)->step_freq = 1200;
  ((GrappaRenderer*)grappa)->friction = 1;
  ((GrappaRenderer*)grappa)->update(dtime,clickpoint.x+(sin(time*16))*cursize.x,clickpoint.y+(cos(time*16))*cursize.y,1.1);//fabs(sin(time*3)*0.05)+1.1);//fabs(sin(time*0.1)),fabs(cos(time*0.15)),-1.1);
  ((GrappaRenderer*)grappa)->render();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
*/
  //glPopMatrix();
//}

void vsxu_assistant::i_draw() {
  if (auto_) {
    reload = false;

    if (help_timestamp != timestamp) {
      timestamp = help_timestamp;
      reload = true;
    }
    if (a_focus != this)
    if (a_focus->widget_type != VSX_WIDGET_TYPE_2D_MENU_POPUP)
    if (a_focus != inspected)
    reload = true;
    //else reload = false;
    if (reload)
    {
//    printf("auto buuuu\n");
      inspected = a_focus;
      if (course.size())
      course.clear();
      std::vector<vsx_string> parts;
      vsx_string deli = "|||";
      if (inspected->help_text != "") {
        explode(inspected->help_text,deli,parts);
        for (unsigned int i = 0; i < parts.size(); ++i) {
          course.push_back(parts[i]);
        }
      } //else {
        //course.push_back("");
      //}
      ((vsx_widget_2d_pager*)pager)->max_pages = course.size();
      ((vsx_widget_2d_pager*)pager)->cur_page = 0;
    }
  }

	tt = dtime*10.0f*global_interpolation_speed;
  if (tt > 1) tt = 1;
	clickpoint.x = clickpoint.x*(1-tt)+size_multiplier*tt;

  size.x = 0.3*clickpoint.x*(screen_aspect);
  size.y = 0.5*clickpoint.x;
  //printf("pos.x: %f\n",pos.x);
  //size.y-0.09f*clickpoint.x;
  alpha = (clickpoint.x-0.2)*2;
  set_pos(vsx_vector((screen_aspect-size.x),-0.03));

  pager->set_pos(vsx_vector(0.065f*clickpoint.x,clickpoint.x*0.14f));
  pager->set_size(vsx_vector(0.1f*clickpoint.x,0.03f*clickpoint.x));
  //pager->size.x = 0.1*clickpoint.x;
  //pager->size.y = 0.030*clickpoint.x;

//  float screen_aspect = 1.0f/this->screen_aspect;
  texture.bind();
  glColor3f(1,1,1);
  draw_box_tex(pos, size.x, size.y);
 	/*
   glColor4f(0.9,0.9,1,1);
 	glBegin(GL_QUADS);
     	glTexCoord2f(0, 0);
  			glVertex3f(pos.x, pos.y-size.y, 0);
     	glTexCoord2f(0, 1);
  			glVertex3f(pos.x, pos.y, 0);
     	glTexCoord2f(1, 1);
  			glVertex3f((pos.x+size.x), pos.y, 0);
     	glTexCoord2f(1, 0);
   			glVertex3f((pos.x+size.x), pos.y-size.y, 0);
	glEnd();*/
	texture._bind();
  if (alpha > 0.01) {
    if (course.size()) {
      text = course[((vsx_widget_2d_pager*)pager)->cur_page];
      //else text = "";
      myf.color.a = alpha;
      myf.print(vsx_vector((pos.x+size.x*0.06)*screen_aspect,pos.y+size.y-size.y*0.12),text,0.012*clickpoint.x);
    }
  }
  target_pos = pos;
  target_size = size;
}

void vsxu_assistant::reinit() {
  texture.load_png(PLATFORM_SHARED_FILES+"gfx"+DIRECTORY_SEPARATOR+"luna.png");
  vsx_widget::reinit();
}

void vsxu_assistant::init()
{
  temp_size = -1.0f;
  if (!init_run) vsx_widget::init();
  render_type = VSX_WIDGET_RENDER_2D;
  coord_type = VSX_WIDGET_COORD_CORNER;
  topmost = true;
  myf.mode_2d = true;
  texture.load_png(PLATFORM_SHARED_FILES+"gfx"+DIRECTORY_SEPARATOR+"luna.png");
  if (configuration.find("assistant_size") != configuration.end()) {
    size_multiplier = s2f(configuration["assistant_size"]);
  } else
  size_multiplier = 1.0f;

  pager = add(new vsx_widget_2d_pager,"pages");
  pager->init();
  title = "Luna <3";
  inspected = 0;

  menu = add(new vsx_widget_popup_menu,".obtx_menu");
  menu->commands.adds(VSX_COMMAND_MENU, "Toggle size {tab}", "mini","");
  menu->commands.adds(VSX_COMMAND_MENU, "Automatic Mode", "auto","");
  menu->commands.adds(VSX_COMMAND_MENU, "Courses;Introduction course", "load","course_intro");
  menu->size.x = 0.2;
  menu->init();

  vsx_command_s* c;
  vsx_command_list cla;
  cla.load_from_file(vsx_get_data_path()+"help_settings.conf",true,4);
  auto_ = false;
  while ( (c = cla.pop()) ) {
    if (c->cmd == "auto") {
//      printf("auto is true\n");
      auto_ = true;
    }
  }
  if (!auto_) {
    command_q_b.add_raw("load course_intro");
    vsx_command_queue_b(this);
  }
  target_pos = pos;
  target_size = size;
//  lesson_id = 0;
}

void vsxu_assistant::toggle_size() {
  if (size_multiplier == 1.3f) {
    size_multiplier = 1.0f;
  } else
  if (size_multiplier == 1.0f) {
    size_multiplier = 0.8f;
  } else
  /*if (size_multiplier == 0.9f) {
    size_multiplier = 0.8f;
  } else*/
  if (size_multiplier == 0.8f) {
    size_multiplier = 0.2f;
  } else
  if (size_multiplier == 0.2f)
  size_multiplier = 1.3f;
  configuration["assistant_size"] = f2s(size_multiplier);
  ((vsx_widget_desktop*)root)->save_configuration();
}

void vsxu_assistant::temp_hide() {
  if (temp_size == -1.0f) {
    temp_size = size_multiplier;
    size_multiplier = 0.2f;
  }
}

void vsxu_assistant::temp_show() {
  if (temp_size != -1.0f) {
    size_multiplier = temp_size;
    temp_size = -1.0f;
  }
}

void vsxu_assistant::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button) {
  if (button == 0)
  {
    ((vsx_widget_2d_pager*)pager)->increase();
  } else
  vsx_widget::event_mouse_down(distance,coords,button);
}

void vsxu_assistant::vsx_command_process_b(vsx_command_s *t) {
  if (t->cmd == "mini") {
    toggle_size();
    //size_multiplier = 0.2;
  }
  else
  if (t->cmd == "auto") {
    if (!auto_) {
      inspected = 0;
      vsx_command_list cla;
      cla.add_raw("auto");
      cla.save_to_file(vsx_get_data_path()+"help_settings.conf");
      auto_ = true;
      course.clear();
      ((vsx_widget_2d_pager*)pager)->max_pages = 0;//course.size();
    }
  }
  else
  if (t->cmd == "load") {
    auto_ = false;
    vsx_command_s* c;
    vsx_command_list cl;
    cl.load_from_file(PLATFORM_SHARED_FILES+"doc/"+t->cmd_data+".txt",false,4);
    vsx_string newstr;
    course.clear();
    while ((c = cl.pop())) {
      if (c->raw == "|||") {
        course.push_back(newstr);
        newstr = "";
      } else {
        newstr = newstr + c->raw+"\n";
      }
    }
    course.push_back(newstr);
    ((vsx_widget_2d_pager*)pager)->max_pages = course.size();
  }
}

vsxu_assistant::vsxu_assistant() {
  timestamp = 0;
  reload = false;
}
#endif
