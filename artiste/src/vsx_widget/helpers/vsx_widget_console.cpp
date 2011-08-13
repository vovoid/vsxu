#include <map>
#include <list>
#include <vector>
#include <math.h>
#include "vsx_gl_global.h"
#include "vsx_math_3d.h"
#include "vsx_texture_info.h"
#include "vsx_texture.h"
#include "vsx_command.h"
#include "vsx_font.h"
#include "vsx_param.h"
#include "vsx_module.h"

// local includes
#include "vsx_widget_base.h"
#include "window/vsx_widget_window.h"
#include "vsx_widget_object_inspector.h" 
#include "vsx_widget_desktop.h"
#include "lib/vsx_widget_lib.h"
//#include "vsx_widget_comp.h"
//#include "vsx_widget_server.h"
//#include "vsx_widget_module_chooser.h"
#include "dialogs/vsx_widget_window_statics.h"
#include "lib/vsx_widget_panel.h"
#include "lib/vsx_widget_base_edit.h"

#ifndef _WIN32
#include "GL/glfw.h"
#endif

#include "vsx_widget_console.h"

// VSX_WIDGET_CONSOLE **************************************************************************************************
// VSX_WIDGET_CONSOLE **************************************************************************************************  
// VSX_WIDGET_CONSOLE **************************************************************************************************
// VSX_WIDGET_CONSOLE **************************************************************************************************

  void vsx_widget_2d_console::init() {
    up = false;
    render_type = VSX_WIDGET_RENDER_2D;
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
    set_pos(vsx_vector(0,yposs));
    set_size(vsx_vector(1,fontsize*rows+0.01));
    ythrust = 0;
    htime = sqrt(size.y-yposs);
    vsx_widget_base_edit* edit = (vsx_widget_base_edit*)add(new vsx_widget_base_edit,".e");

//    edit->set_pos(vsx_vector(0, -size.y-fontsize+0.003)); 
    edit->set_pos(vsx_vector(size.x*0.5f, -size.y+fontsize*0.5f)); 
    edit->size_from_parent = true;
    edit->set_size(vsx_vector(1.0f, fontsize));
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
  
  int vsx_widget_2d_console::inside_xy_l(vsx_vector &test, vsx_vector &global) {
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
  
  void vsx_widget_2d_console::vsx_command_process_b(vsx_command_s *t) {
    cmd_parent = destination;
    ((vsx_widget_base_edit*)editor)->set_string("");
    ((vsx_widget_base_edit*)editor)->caret_goto_end();
    command_q_b.add(vsx_command_parse(t->raw));
  }

  void vsx_widget_2d_console::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button) {
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
    //glGetIntegerv(GL_VIEWPORT, viewport);
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
    	tx += dtime*1.5f;
    	if (tx > htime) { ythrust = 0; tx = htime; topmost = true; 
      k_focus = editor;
      }
    	ypos = htime*htime-(htime-tx)*(htime-tx);
  	}
  	if (ythrust < 0) {
    	tx -= dtime*1.5f;
    	if (tx < 0) { ythrust = 0; tx = 0; topmost = true; k_focus = root;}
    	ypos = htime*htime-(htime-tx)*(htime-tx);
  	}
  	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  	glColor4f(1,1,1,1);
  	target_pos.y = pos.y = yposs+ypos;
  	glBegin(GL_QUADS);
      glColor4f(skin_color[7].r,skin_color[7].g,skin_color[7].b,skin_color[7].a);
  			glVertex3f(0, pos.y-size.y, 0);
      glColor4f(skin_color[8].r,skin_color[8].g,skin_color[8].b,skin_color[8].a);
  			glVertex3f(0, pos.y, 0);
  			glVertex3f(1.0f*screen_aspect, pos.y, 0);
      glColor4f(skin_color[7].r,skin_color[7].g,skin_color[7].b,skin_color[7].a);
   			glVertex3f(1.0f*screen_aspect, pos.y-size.y, 0);
  	glEnd();
    glColor4f(skin_color[10].r,skin_color[10].g,skin_color[10].b,skin_color[10].a);
    texty = pos.y-size.y+fontsize+fontsize/4;
    if (tx != 0) {
      double c = 0;
      if (destination)
      for (int x = destination->log_cmd.size()-1; ((x >= 0) && (c < rows-1)); x--) {
        myf.print(vsx_vector(0,texty+fontsize*c*0.9),destination->log_cmd[x],fontsize);
        ++c;
      }
    }
    glColor4f(skin_color[9].r,skin_color[9].g,skin_color[9].b,skin_color[9].a);
    if (destination)
    myf.print(vsx_vector(0,pos.y-0.025),h+"console:"+destination->name,0.025); 
    glLineWidth(1.0);
   	glBegin(GL_LINE_STRIP);
   	  glVertex3f(0,pos.y-0.025,0);
      glColor4f(skin_color[11].r,skin_color[11].g,skin_color[11].b,skin_color[11].a);
   	  glVertex3f(0.41*screen_aspect,pos.y-0.025,0);
      glColor4f(skin_color[12].r,skin_color[12].g,skin_color[12].b,skin_color[12].a);
   	  glVertex3f(screen_aspect,pos.y-0.025,0);
   	  
 	  glEnd();

    draw_children_2d();
  }

	void vsx_widget_2d_console::set_destination(vsx_widget* dest) {
		destination = dest;
	}

