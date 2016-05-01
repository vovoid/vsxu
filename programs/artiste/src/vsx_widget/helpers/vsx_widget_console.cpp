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

#include <map>
#include <list>
#include <vector>
#include <math.h>
#include "vsx_gl_global.h"
#include <texture/vsx_texture.h>
#include <command/vsx_command.h>
#include <command/vsx_command_list.h>
#include <command/vsx_command_client_server.h>
#include "vsx_font.h"
#include "vsx_param.h"
#include <module/vsx_module.h>

// local includes
#include "vsx_widget.h"
#include "vsx_widget_window.h"
#include "vsx_widget_object_inspector.h" 
#include "widgets/vsx_widget_panel.h"
#include "widgets/vsx_widget_base_edit.h"


#include "vsx_widget_console.h"

// VSX_WIDGET_CONSOLE **************************************************************************************************
// VSX_WIDGET_CONSOLE **************************************************************************************************  
// VSX_WIDGET_CONSOLE **************************************************************************************************
// VSX_WIDGET_CONSOLE **************************************************************************************************

  void vsx_widget_2d_console::init() {
    up = false;
    render_type = render_2d;
    coord_type = VSX_WIDGET_COORD_CORNER;
    topmost = true;
    fontsize = 0.02;
    ypos = 0;
    yposs = 0.025;
    //pos.x = 0;
    //size.x = 1;
    support_interpolation = false;
    rows = 15;
    //size.y = fontsize*rows+0.01;
    tx = 0;
    //pos.y = yposs;
    set_pos(vsx_vector3<>(0,yposs));
    set_size(vsx_vector3<>(1,fontsize*rows+0.01));
    ythrust = 0;
    htime = sqrt(size.y-yposs);
    vsx_widget_base_edit* edit = (vsx_widget_base_edit*)add(new vsx_widget_base_edit,".e");

//    edit->set_pos(vsx_vector(0, -size.y-fontsize+0.003)); 
    edit->set_pos(vsx_vector3<>(size.x*0.5f, -size.y+fontsize*0.5f));
    edit->size_from_parent = true;
    edit->set_size(vsx_vector3<>(1.0f, fontsize));
    edit->single_row = true;
    edit->set_font_size(fontsize);
    edit->init();
    edit->set_string("");
    edit->caret_goto_end();
    
    editor = edit;
    

    //editor = add(new vsx_widget_2d_edit,name+".edit");
    //editor->pos.y = -(size.y - fontsize)+0.003;
    //editor->pos.x = 0;
    //editor->size.x = 1;
    //editor->size.y = fontsize;
    destination = a_focus;
    h = ">>";
    h[0] = 16;
    h[1] = 16;
    destination = a_focus;
  }
  
  int vsx_widget_2d_console::inside_xy_l(vsx_vector3<> &test, vsx_vector3<> &global) {
    if (coord_type == VSX_WIDGET_COORD_CORNER) {
      if ( test.y < global.y && 
          test.y > global.y-size.y) 
      {
        return 2;
      }
    }
    //if (screen.y < pos.y) return true; else return false;
    return 0;
  }
  
  void vsx_widget_2d_console::command_process_back_queue(vsx_command_s *t) {
    cmd_parent = destination;
    ((vsx_widget_base_edit*)editor)->set_string("");
    ((vsx_widget_base_edit*)editor)->caret_goto_end();
    command_q_b.add(vsx_command_parse<vsx_command_s>(t->raw));
  }

  void vsx_widget_2d_console::event_mouse_down(vsx_widget_distance distance, vsx_widget_coords coords, int button)
  {
    VSX_UNUSED(distance);
    VSX_UNUSED(coords);

    if ((button == 0) && (tx > 0)) k_focus = editor;
    parent->front(this);
    if (ythrust == 0) {
      if ((button == 2) && (tx > 0 ))  ythrust = -1;
      if (tx <= 0 ) ythrust = 1;
    } else {
      if (ythrust < 0) ythrust = 1; else
      if ((button == 2) &&(ythrust > 0)) ythrust = -1;
    }
  }

  void vsx_widget_2d_console::draw_2d() {
    if (a_focus != destination) {
      if (a_focus->widget_type == VSX_WIDGET_TYPE_COMPONENT || a_focus->widget_type == VSX_WIDGET_TYPE_SERVER) {
        destination = a_focus;
      } else
      {
      	if (a_focus != editor)
      	destination = 0;
      }
    }
    
  	if (ythrust > 0) {
      tx += vsx_widget_time::get_instance()->get_dtime() * 1.5f;
    	if (tx > htime) { ythrust = 0; tx = htime; topmost = true; 
      k_focus = editor;
      }
    	ypos = htime*htime-(htime-tx)*(htime-tx);
  	}
  	if (ythrust < 0) {
      tx -= vsx_widget_time::get_instance()->get_dtime() * 1.5f;
    	if (tx < 0) { ythrust = 0; tx = 0; topmost = true; k_focus = root;}
    	ypos = htime*htime-(htime-tx)*(htime-tx);
  	}
  	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  	glColor4f(1,1,1,1);
  	target_pos.y = pos.y = yposs+ypos;
  	glBegin(GL_QUADS);
      vsx_widget_skin::get_instance()->set_color_gl(7);
        glVertex3f(0, pos.y-size.y, 0);
      vsx_widget_skin::get_instance()->set_color_gl(8);
  			glVertex3f(0, pos.y, 0);
  			glVertex3f(1.0f*screen_aspect, pos.y, 0);
      vsx_widget_skin::get_instance()->set_color_gl(7);
   			glVertex3f(1.0f*screen_aspect, pos.y-size.y, 0);
  	glEnd();
    vsx_widget_skin::get_instance()->set_color_gl(10);
    texty = pos.y-size.y+fontsize+fontsize/4;
    if (tx != 0) {
      double c = 0;
      if (destination)
      for (int x = destination->log_cmd.size()-1; ((x >= 0) && (c < rows-1)); x--) {
        font.print(vsx_vector3<>(0,texty+fontsize*c*0.9),destination->log_cmd[x],fontsize);
        ++c;
      }
    }
    vsx_widget_skin::get_instance()->set_color_gl(9);
    if (destination)
    font.print(vsx_vector3<>(0,pos.y-0.025),h+"console:"+destination->name,0.025);
    glLineWidth(1.0);
   	glBegin(GL_LINE_STRIP);
   	  glVertex3f(0,pos.y-0.025,0);
      vsx_widget_skin::get_instance()->set_color_gl(11);
   	  glVertex3f(0.41*screen_aspect,pos.y-0.025,0);
      vsx_widget_skin::get_instance()->set_color_gl(12);
   	  glVertex3f(screen_aspect,pos.y-0.025,0);
   	  
 	  glEnd();

    draw_children_2d();
  }

	void vsx_widget_2d_console::set_destination(vsx_widget* dest) {
		destination = dest;
	}

