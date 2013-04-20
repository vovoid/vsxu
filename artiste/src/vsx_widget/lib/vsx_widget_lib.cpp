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
//#include <string>
//#include <iostream>
#include <map>
#include <list>
#include <vector>
#include <math.h>
//#include <sstream>
//#include <fstream>
#include "vsxfst.h"
#include "vsx_math_3d.h"
#include "vsx_texture_info.h"
#include "vsx_texture.h"
#include "gl_helper.h"

#include "vsx_command.h"
#include "vsx_widget_lib.h"

// VSX_WIDGET_POPUP ****************************************************************************************************
// VSX_WIDGET_POPUP ****************************************************************************************************
// VSX_WIDGET_POPUP ****************************************************************************************************
// VSX_WIDGET_POPUP ****************************************************************************************************
  void vsx_widget_popup_menu::i_draw() {
    render_type =   VSX_WIDGET_RENDER_2D;

    target_size.y = (float)(menu_items.count())*row_size;

    if (visible >= 1) {
      if (a_focus->widget_type != VSX_WIDGET_TYPE_2D_MENU_POPUP && visible != 2) {
        visible = 0;
        return;
      } else {
        if (visible == 2 && a_focus->widget_type == VSX_WIDGET_TYPE_2D_MENU_POPUP) {
          // in case of another menu being open before us
          if (a_focus != this)
          if (a_focus != parent) {
            a_focus->visible = 0;
          }
        }
        if (visible == 2) {
          hide_children();
          over = 0;
          oversub = false;
          target_pos.y -= target_size.y;
          pos.y = target_pos.y;
        }
        visible = 1;
        a_focus = this;
        if (parent->widget_type != VSX_WIDGET_TYPE_2D_MENU_POPUP) {
          if (pos.x+size.x > 1) pos.x = 1-size.x;
        }
      }
   	  float sx = target_pos.x;
   	  float alpha = 0.8f;
   	  //if (!oversub) alpha = 1.0f; else alpha = 1.0f;

     	//glBegin(GL_QUADS);
      if (parent->widget_type == VSX_WIDGET_TYPE_2D_MENU_POPUP) {
        if (((vsx_widget_popup_menu*)parent)->over && ((vsx_widget_popup_menu*)parent)->over != id_over)
        visible = 0; else
        {
          alpha = 1.0f;
        glColor4f(skin_color[0].r*alpha,skin_color[0].g,skin_color[0].b,skin_color[0].a*alpha);
        draw_box(vsx_vector((sx-0.001)*screen_aspect,target_pos.y+0.001+target_size.y),(size.x+0.002)*screen_aspect,-((float)menu_items.count())*row_size-0.002);
        }
      } else {
        glColor4f(skin_color[0].r*alpha,skin_color[0].g,skin_color[0].b,skin_color[0].a*alpha);
        draw_box(vsx_vector((sx-0.001)*screen_aspect,target_pos.y+0.001+target_size.y),(size.x+0.002)*screen_aspect,-((float)menu_items.count())*row_size-0.002);
    	  myf.print(vsx_vector(sx*screen_aspect,target_pos.y+target_size.y),parent->title,row_size);
      }


   	  double y = target_pos.y+target_size.y;
   	  vsx_command_s *t;

   	  menu_items.reset();
   	  int c = 1;
      while ( (t = menu_items.get()) ) {
        if (c == over && over != 0)
        {
          current_command_over = t;
          glColor4f(skin_color[4].r,skin_color[4].g,skin_color[4].b,skin_color[4].a);
          if (t->cmd == "" && l_list.find(t->title) != l_list.end())
          {
            vsx_widget_popup_menu* sub = ((vsx_widget_popup_menu*)l_list[t->title]);
            if (!sub->visible) {
              sub->visible = 2;
              sub->target_pos.x = target_pos.x+(1/screen_aspect)*row_size*(float)t->title.size()*0.33;
              sub->target_pos.y = y;
              sub->size = size;
              sub->id_over = over;
            }
            oversub = true;
          } else {
            oversub = false;
          }
        }
        else
        glColor4f(skin_color[1].r,skin_color[1].g,skin_color[1].b,skin_color[1].a*alpha);
        draw_box(vsx_vector((sx)*screen_aspect,y),size.x*screen_aspect,-row_size);
        myf.print(vsx_vector((sx+0.003)*screen_aspect,y-row_size),t->title,row_size*0.8);
        y-=row_size;
        ++c;
      }
      draw_children_2d();
    }
  }

  void vsx_widget_popup_menu::add_commands(vsx_command_s *command) {
    // split the title
    vsx_string title = command->title;
    vsx_string deli = ";";
    std::vector<vsx_string> add_c;
    explode(title,deli,add_c);
    if (add_c.size() > 1) {
      // ok, we're not topmost
      vsx_widget* t = 0;
      if (l_list.find(add_c[0]) == l_list.end()) {
        t = add(new vsx_widget_popup_menu, add_c[0]);
        t->pos.x = 0;
        t->pos.y = pos.y;
        t->init();
        menu_items.adds(VSX_COMMAND_MENU, add_c[0],"","");
      } else t = l_list[add_c[0]];
      add_c.erase(add_c.begin());
      // mod the command, yeah!
      command->title = implode(add_c,deli);
      ((vsx_widget_popup_menu*)t)->add_commands(command);
      // GAH I'M TIRED OF THIS, THIS IS LIKE THE 5TH similar thing i write ARGHH
      // AND it's not WORKING :(
    } else {
      menu_items.add(command);
    }
  }

  void vsx_widget_popup_menu::init() {
    if (init_run) return;
    widget_type = VSX_WIDGET_TYPE_2D_MENU_POPUP;
    coord_type = VSX_WIDGET_COORD_CORNER;
    coord_related_parent = false;

    topmost = true;
    row_size = 0.02;
    visible = 0;
    over = 0;
    oversub = false;


    commands.reset();
    vsx_command_s* t;
    while ( (t = commands.get()) ) {
      add_commands(t);
      //

    }
    init_run = true;
  }

  void vsx_widget_popup_menu::event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords)
  {
    VSX_UNUSED(coords);
    //printf("menu type: %d\n",render_type);
    //printf("menu passive %d %f %f \n",menu_items.count(),target_pos.y,distance.corner.y);

    //!glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
      over = menu_items.count()-((int)((distance.corner.y)/row_size));
  }

  void vsx_widget_popup_menu::vsx_command_process_b(vsx_command_s *t) {
    visible = 0;
    vsx_widget::vsx_command_process_b(t);
  }

  void vsx_widget_popup_menu::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button)
  {
    VSX_UNUSED(distance);
    VSX_UNUSED(button);
    if (over) {
      if (!oversub) {
        // add a number of commands
        vsx_command_s* t = command_q_b.addc(current_command_over);
        if (t-> cmd_data == "$mpos") {

          t->cmd_data = (coords.world_global-parent->get_pos_p()).to_string(2);//distance.center.to_string(2);//f2s(distance.center.x)+","+f2s(distance.center.y);
          if (t->parts.size() > 2) t->parts[2] = t->cmd_data;
        }
        parent->vsx_command_queue_b(this);
        visible = 0;
      }
    }
  }

  void vsx_widget_popup_menu::on_delete() {
    menu_items.clear(true); // really delete the items.
  }


// VSX_WIDGET_BUTTON ***************************************************************************************************
// VSX_WIDGET_BUTTON ***************************************************************************************************
// VSX_WIDGET_BUTTON ***************************************************************************************************
// VSX_WIDGET_BUTTON ***************************************************************************************************

void vsx_widget_button::init() {
  coord_type = VSX_WIDGET_COORD_CENTER;
  if (render_type == VSX_WIDGET_RENDER_2D) {
    border = 0.0023;
    size.x = 0.06;
    size.y = 0.03;
  } else {
    border = 0.0009;
    size.x = 0.03;
    size.y = 0.015;
  }
  target_size = size;
}

void vsx_widget_button::event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  VSX_UNUSED(distance);
  VSX_UNUSED(coords);
  VSX_UNUSED(button);

  if (!outside)
  {
    commands.reset();
    command_q_b.addc(commands.get_cur());
    parent->vsx_command_queue_b(this);
  }
}

void vsx_widget_button::event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords)
{
  VSX_UNUSED(coords);

  outside = !((distance.corner.x > 0) && (distance.corner.x < target_size.x) && (distance.corner.y > 0) && (distance.corner.y < target_size.y));
}

void vsx_widget_button::i_draw() {
	//if (type == 0) return;
	if (!visible) return;

	vsx_vector p = get_pos_p();
	if (coord_type == VSX_WIDGET_COORD_CENTER) {
		p.x -= size.x*0.5;
		p.y += size.y*0.5;
		//p.z = 1.1f;
	}
	//printf("p.z %f\n",p.z);
	//if (render_type == VSX_WIDGET_RENDER_2D) {
//      p.z = 0.0f;
//    }
	float font_size_smaller = 1.0f;
	if ((m_focus == this) && (!outside)) {
		font_size_smaller = 0.75f;
		skin_color[0].gl_color();
	}
	else
		skin_color[6].gl_color();

	glBegin(GL_QUADS);
			glVertex3f((p.x+border)*screen_aspect, p.y-size.y+border, p.z);
			glVertex3f((p.x+border)*screen_aspect, p.y-border, p.z);
			glVertex3f((p.x+size.x-border)*screen_aspect, p.y-border, p.z);
			glVertex3f((p.x+size.x-border)*screen_aspect, p.y-size.y+border, p.z);

	if ((m_focus == this) && (!outside))
	skin_color[1].gl_color();
	else
	skin_color[4].gl_color();

			// left
			glVertex3f(p.x, p.y-border, p.z);
			glVertex3f((p.x+border), p.y-border, p.z);
			glVertex3f((p.x+border), p.y-size.y+border, p.z);
			glVertex3f(p.x,p.y-size.y+border, p.z);

			// right
			glVertex3f((p.x+size.x-border), p.y-border, p.z);
			glVertex3f((p.x+size.x), p.y-border, pos.z);
			glVertex3f((p.x+size.x), p.y-size.y+border, p.z);
			glVertex3f((p.x+size.x-border),p.y-size.y+border, p.z);

			glVertex3f(p.x, p.y, p.z);
			glVertex3f((p.x+size.x), p.y, p.z);
			glVertex3f((p.x+size.x), p.y-border, p.z);
			glVertex3f(p.x,p.y-border, p.z);

			glVertex3f(p.x, p.y-size.y+border, p.z);
			glVertex3f((p.x+size.x), p.y-size.y+border, p.z);
			glVertex3f((p.x+size.x), p.y-size.y, p.z);
			glVertex3f(p.x,p.y-size.y, p.z);
	glEnd();
	//glColor3f(1,1,1);
	float dd;
	if ((m_focus == this) && (!outside))
	dd = size.y*0.05;
	else
	dd = 0;

	myf.print_center(vsx_vector((p.x+size.x/2+dd),p.y-size.y+size.y*0.25), title,size.y*0.4f*font_size_smaller);
}

  //void vsx_widget_button::draw_2d() {
    //if (render_type == VSX_WIDGET_RENDER_2D) { draw(); }
    /*if (type == 1) return;
    if (!visible) return;
    vsx_vector p = parent->get_pos_p()+pos;
    if ((m_focus == this) && (!outside))
      glColor4f(skin_color[0].r,skin_color[0].g,skin_color[0].b,1);
    else
      glColor4f(skin_color[6].r,skin_color[6].g,skin_color[6].b,1);

  	glBegin(GL_QUADS);
  			glVertex3f((p.x+border)*screen_aspect, p.y-size.y+border, 0);
  			glVertex3f((p.x+border)*screen_aspect, p.y-border, 0);
  			glVertex3f((p.x+size.x-border)*screen_aspect, p.y-border, 0);
   			glVertex3f((p.x+size.x-border)*screen_aspect, p.y-size.y+border, 0);
  	//glEnd();

    if ((m_focus == this) && (!outside))
    glColor4f(skin_color[1].r,skin_color[1].g,skin_color[1].b,1);
    else
    glColor4f(skin_color[4].r,skin_color[4].g,skin_color[4].b,1);


  	//glBegin(GL_QUADS);
  			glVertex3f(p.x*screen_aspect, p.y, 0);
  			glVertex3f((p.x+border)*screen_aspect, p.y, 0);
  			glVertex3f((p.x+border)*screen_aspect, p.y-size.y, 0);
   			glVertex3f(p.x*screen_aspect,p.y-size.y, 0);
  	//glEnd();
  	//glBegin(GL_QUADS);
  			glVertex3f((p.x+size.x-border)*screen_aspect, p.y, 0);
  			glVertex3f((p.x+size.x)*screen_aspect, p.y, 0);
  			glVertex3f((p.x+size.x)*screen_aspect, p.y-size.y, 0);
   			glVertex3f((p.x+size.x-border)*screen_aspect,p.y-size.y, 0);
  	//glEnd();
  	//glBegin(GL_QUADS);
  			glVertex3f(p.x*screen_aspect, p.y, 0);
  			glVertex3f((p.x+size.x)*screen_aspect, p.y, 0);
  			glVertex3f((p.x+size.x)*screen_aspect, p.y-border, 0);
   			glVertex3f(p.x*screen_aspect,p.y-border, 0);
  	//glEnd();
  	//glBegin(GL_QUADS);
  			glVertex3f(p.x*screen_aspect, p.y-size.y+border, 0);
  			glVertex3f((p.x+size.x)*screen_aspect, p.y-size.y+border, 0);
  			glVertex3f((p.x+size.x)*screen_aspect, p.y-size.y, 0);
   			glVertex3f(p.x*screen_aspect,p.y-size.y, 0);
  	glEnd();
  	glColor3f(1,1,1);
  	if ((m_focus == this) && (!outside))
  	myf.print_center(vsx_vector((p.x+size.x/2+size.y*0.05)*screen_aspect,p.y-size.y+size.y*0.15,0), title,"ascii",size.y*0.6);
  	else
  	myf.print_center(vsx_vector((p.x+size.x/2)*screen_aspect,p.y-size.y+size.y*0.2,0), title,"ascii",size.y*0.6);*/
  //}

  void vsx_widget_button::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button)
  {
    VSX_UNUSED(distance);
    VSX_UNUSED(coords);
    VSX_UNUSED(button);

    outside = false;
    m_focus = this;
  }


// VSX_WIDGET_LABEL ****************************************************************************************************
// VSX_WIDGET_LABEL ****************************************************************************************************
// VSX_WIDGET_LABEL ****************************************************************************************************
// VSX_WIDGET_LABEL ****************************************************************************************************

  void vsx_widget_2d_label::init() {
  	//myf.outline_transparency = 0.35;
    halign = a_center;
    font_size = size.y = 0.014;

  }

  void vsx_widget_2d_label::draw_2d() {
    if (!visible) return;
    glColor3f(1,1,1);
    vsx_vector p = parent->get_pos_p()+pos;
    p.y -= font_size*0.5f;
    switch ((align)halign) {
      case a_left:
        myf.print(p, title,font_size);
      break;
      case a_center:
        myf.print_center(p, title,font_size);
      break;
      case a_right:
        myf.print_right(p, title,font_size);
      break;
    }
  }


// VSX_WIDGET_PAGER ***************************************************************************************************
// VSX_WIDGET_PAGER ***************************************************************************************************
// VSX_WIDGET_PAGER ***************************************************************************************************
// VSX_WIDGET_PAGER ***************************************************************************************************

vsx_widget_2d_pager::vsx_widget_2d_pager() {
  type = 0;
  max_pages = 0;
  cur_page = 0;
}
void vsx_widget_2d_pager::init() {
  set_size(vsx_vector(0.06,0.1));
  pos.x = 0.0f;
  pos.y = 0.0f;
	render_type = VSX_WIDGET_RENDER_2D;
	coord_type = VSX_WIDGET_COORD_CENTER;
}

void vsx_widget_2d_pager::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  VSX_UNUSED(coords);
  VSX_UNUSED(button);

  distance.center.x > 0.0f ? increase() : decrease();
}

void vsx_widget_2d_pager::i_draw() {
  if (!visible) return;
  //myf.color = vsx_color(1.0f,1.0f,1.0f,1.0f);
  if (max_pages == 0) cur_page = -1;
	myf.print_center(get_pos_p(), "<< "+i2s(cur_page+1)+" / "+i2s(max_pages)+" >>",size.y*0.6);
}

void vsx_widget_2d_pager::increase() {
  ++cur_page;
  if (cur_page >= max_pages) cur_page = 0;
}

void vsx_widget_2d_pager::decrease() {
  if (max_pages && cur_page) --cur_page; else cur_page = max_pages-1;
}

#endif
