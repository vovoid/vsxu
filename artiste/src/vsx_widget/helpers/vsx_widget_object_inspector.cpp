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
#include "server/vsx_widget_anchor.h"
#include "server/vsx_widget_connector_bezier.h"
#include "server/vsx_widget_comp.h"
#include "vsxfst.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include "vsx_timer.h"
#include "lib/vsx_widget_panel.h"
#include "lib/vsx_widget_base_edit.h"


void vsx_window_object_inspector::draw_2d() {
	vsx_widget_window::draw_2d();

  if (view_type == 1)
  {
    pos_.y = pos.y+size.y-font_size;
    size_.y = size.y-font_size-dragborder;
    pos_.x = pos.x+dragborder;
    size_.x = size.x-dragborder*2;
    //printf("size: %f\n",size_.x);
    title = filename_loaded+" "+i2s((int)texture.texture_info->size_x)+"x"+i2s((int)texture.texture_info->size_y);
#ifndef VSXU_PLAYER
   	texture.bind();
    glColor4f(1,1,1,1);
    if (texture.valid) {
    	glBegin(GL_QUADS);
      if (texture.texture_info->ogl_type == GL_TEXTURE_2D) {
      	glTexCoord2i(0, 1);
        glVertex3f(pos_.x,pos_.y,0);
      	glTexCoord2i(1, 1);
        glVertex3f(pos_.x+size_.x,pos_.y,0);
      	glTexCoord2i(1, 0);
        glVertex3f(pos_.x+size_.x,pos_.y-size_.y,0);
      	glTexCoord2i(0, 0);
        glVertex3f(pos_.x,pos_.y-size_.y,0);
      } else {
        glTexCoord2i(0, (int)texture.texture_info->size_y);
        glVertex3f(pos_.x,pos_.y,0);
        glTexCoord2i((int)texture.texture_info->size_x, (int)texture.texture_info->size_y);
        glVertex3f(pos_.x+size_.x,pos_.y,0);
        glTexCoord2i((int)texture.texture_info->size_x, 0);
        glVertex3f(pos_.x+size_.x,pos_.y-size_.y,0);
      	glTexCoord2i(0, 0);
        glVertex3f(pos_.x,pos_.y-size_.y,0);
      }
      glEnd();
    }
    texture._bind();
#endif
    float screenaspect = screen_x/screen_y;
    //printf("ts %f\n",texture.size_x);
#ifndef VSXU_PLAYER
    if (texture_loaded == false)
    if (texture.texture_info->size_y != 0.0) {
      texture_loaded = true;
      vsx_vector aa;
      aa.x = 0.4/screenaspect*(texture.texture_info->size_x/texture.texture_info->size_y);
      aa.y = 0.4;
      aa.z = 0;
      printf("resizing to tex\n");
      resize_to(aa);
      move(1*screen_aspect-0.4/screenaspect*(texture.texture_info->size_x/texture.texture_info->size_y),1.0f-aa.y,0);
    }
#endif
  }
}

void vsx_window_object_inspector::draw() {
  if (a_focus != component_rename_edit && inspected != a_focus && a_focus != this) {
    //resize_to(vsx_vector(0.001,0.001));
    //move(1-0.001,1);
  	move(1.0f,1.0f);

    inspected = a_focus;
    for (unsigned int i = 0; i < component_list.size(); ++i) component_list[i]->visible = 0;

    if (inspected->widget_type == VSX_WIDGET_TYPE_ANCHOR)
    {
      //label1->title = +a_focus->type;
      label2->title = vsx_string("Anchor data type:")+((vsx_widget_anchor*)inspected)->p_type;
      label1->visible = label2->visible = 1;
      if (((vsx_widget_anchor*)inspected)->alias) {
      	label1->title = "Rename alias:";
      	component_rename_button->visible = 1;
      	component_rename_edit->visible = 1;
      }
      //for (int i = 0; i < component_list.size(); ++i) component_list[i]->visible = 1;
      ((vsx_widget_base_edit*)component_rename_edit)->set_string(((vsx_widget_anchor*)a_focus)->name);
      //((vsx_widget_2d_edit*)component_rename_edit)->value.clear();
      title = "anchor "+((vsx_widget_anchor*)a_focus)->name;
      view_type = 0;
      set_size(vsx_vector(0.2f, 0.175f, 0.0f));
      move(1.0f - 0.2f , 1.0f - 0.175f, 0.0f);
    }
    else
    if (inspected->widget_type == VSX_WIDGET_TYPE_CONNECTOR)
    {
      label1->title = ((vsx_widget_connector_bezier*)a_focus)->destination->name;
      view_type = 0;
      label1->visible = 1;
      //label2->title = ((vsx_widget_anchor*)inspected)->p_type;
    }
    else
    if (inspected->widget_type == VSX_WIDGET_TYPE_COMPONENT)
    {
    	label1->title = "Rename module:";
    	component_rename_button->visible = 1;
    	component_rename_edit->visible = 1;

      for (unsigned long i = 0; i < component_list.size(); ++i) component_list[i]->visible = 1;
      //component_rename_button->visible = 1;
      //component_rename_edit->visible = 1;
      //label1->title = a_focus->name;

      ((vsx_widget_base_edit*)component_rename_edit)->set_string(((vsx_widget_component*)a_focus)->real_name);
      ((vsx_widget_base_edit*)component_rename_edit)->caret_goto_end();
//      ((vsx_widget_base_edit*)component_rename_edit)->value.clear();
      title = ((vsx_widget_component*)a_focus)->real_name;
      view_type = 0;
      //printf("resize to\n");
      set_size(vsx_vector(0.2f, 0.175f, 0.0f));
      move(1.0f - 0.2f , 1.0f - 0.175f, 0.0f);
      // request timing data
      label2->title = "This build of the engine does not\nsupport timing. The reason?\nTiming slows down performance.\n";
#ifdef VSXU_MODULE_TIMING
			command_q_b.add_raw("component_timing "+((vsx_widget_component*)a_focus)->real_name+" "+i2s(id));
#endif
  		((vsx_widget_component*)a_focus)->server->vsx_command_queue_b(this);
    } else
    if (inspected->widget_type == VSX_WIDGET_TYPE_ULTRA_CHOOSER)
    {

      //((vsx_widget_2d_edit*)component_rename_edit)->value.str(((vsx_widget_component*)a_focus)->real_name);
      //((vsx_widget_2d_edit*)component_rename_edit)->value.clear();
      //title = ((vsx_widget_component*)inspected)->component_type;
    } else view_type = 0;
    // image viewer
  }
  vsx_status_timer -= dtime;
  if (inspected->widget_type == VSX_WIDGET_TYPE_COMPONENT)
  {
  	if (vsx_status_timer < 0.0f) {
  		vsx_status_timer = 1.0f;
#ifdef VSXU_MODULE_TIMING
			command_q_b.add_raw("component_timing "+((vsx_widget_component*)inspected)->real_name+" "+i2s(id));
  		((vsx_widget_component*)inspected)->server->vsx_command_queue_b(this);
#endif
  	}
  }
}

void vsx_window_object_inspector::vsx_command_process_b(vsx_command_s *t) {
  if (k_focus == component_rename_edit || t->cmd == "component_rename_button") {
    if (inspected->widget_type == VSX_WIDGET_TYPE_COMPONENT) {
      vsx_widget_component* cc = (vsx_widget_component*)inspected;
      vsx_string curname = "";
      vsx_string newname = "";
//      if (cc->parent_name != "") {
//        curname += cc->parent_name+".";
//        newname += cc->parent_name+".";
//      }
      newname += ((vsx_widget_base_edit*)component_rename_edit)->get_string();
      command_q_b.add_raw("component_rename " +cc->name+" "+newname);
      cc->server->vsx_command_queue_b(this);
    } else
    if (inspected->widget_type == VSX_WIDGET_TYPE_ANCHOR) {
      //printf("sending\n");
      command_q_b.add_raw(
        "pa_ren "+ // command
        ((vsx_widget_anchor*)inspected)->component->name+" "+ // component
        ((vsx_widget_anchor*)inspected)->name+" "+
        ((vsx_widget_base_edit*)component_rename_edit)->get_string()+" "+
        i2s(((vsx_widget_anchor*)inspected)->io)
      );
      ((vsx_widget_component*)((vsx_widget_anchor*)inspected)->component)->server->vsx_command_queue_b(this);
    }
  }
  if (t->cmd == "component_timing_ok")
  {
  	label2->title = vsx_string("run time:   ")+t->parts[2] + " seconds\noutput time:" + t->parts[3] + " seconds\n";
  	label2->title = label2->title + "% of total frame time: "+f2s( (s2f(t->parts[2])+s2f(t->parts[3]))*100.0f / s2f(t->parts[4]),3);

  }
  //if (t->cmd == "cancel") { visible = 0; return; }
  //if (((vsx_widget_2d_edit*)edit1)->value.str().size())
  //{
    //command_q_b.add(name,((vsx_widget_2d_edit*)edit1)->value.str());
    //parent->vsx_command_queue_b(this);
    //((vsx_widget_2d_edit*)edit1)->value.str("");
    //((vsx_widget_2d_edit*)edit1)->value.clear();
  //}
  //visible = 0;
}


vsx_window_object_inspector::vsx_window_object_inspector()
{
  vsx_widget_window::init();
  inspected  = 0;
  init_run = false;
  init_run = true;
  component_rename_edit = add(new vsx_widget_base_edit,"edit1");
  component_rename_edit->init();
  topmost = true;
  component_rename_button = add(new vsx_widget_button,"button_rename");
  label1 = add(new vsx_widget_2d_label,"label1");
  label2 = add(new vsx_widget_2d_label,"label1");

  vsx_widget::init_children();
  title = "object inspector";
	set_pos(vsx_vector(1.0f,1.0f));
	set_size(vsx_vector(0.25f,0.05f));
  allow_resize_x = allow_resize_y = 1;

  //-- COMPONENT INSPECTOR ---------------------------------------------------------------------------------------------

  component_rename_button->visible = 0;
  component_rename_button->set_size(vsx_vector(0.033,0.020));
  component_rename_button->set_pos(vsx_vector(0.18,0.025));
  component_rename_button->title = "RENAME";
  component_rename_button->commands.adds(4,"component_rename_button","component_rename_button","");
  ((vsx_widget_button*)component_rename_button)->border = 0.0003;

  component_rename_edit->visible = 1;
	component_rename_edit->set_pos(vsx_vector(0.085f,0.025f));
	component_rename_edit->set_size(vsx_vector(0.155f,0.02f));
  component_rename_edit->set_font_size(0.018f);
  ((vsx_widget_base_edit*)component_rename_edit)->size_from_parent = true;
  ((vsx_widget_base_edit*)component_rename_edit)->single_row = true;
  ((vsx_widget_base_edit*)component_rename_edit)->set_string("");
  ((vsx_widget_base_edit*)component_rename_edit)->caret_goto_end();
  ((vsx_widget_base_edit*)component_rename_edit)->allowed_chars = "_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890";

  component_list.push_back(component_rename_button);
  component_list.push_back(component_rename_edit);
  component_list.push_back(label1);
  component_list.push_back(label2);

  // -------------------------------------------------------------------------------------------------------------------
  //-- CHOOSER INSPECTOR -----------------------------------------------------------------------------------------------

  ((vsx_widget_2d_label*)label1)->halign = a_left;
  label1->pos.x = 0.005f;
  label1->pos.y = 0.05f;
  label1->visible = 1;
  ((vsx_widget_2d_label*)label2)->halign = a_left;
  label2->pos.x = 0.005f;
  label2->pos.y = 0.12f;
  label2->visible = 0;

  // normal OI
  view_type = 0;
}

void vsx_window_object_inspector::show() {
//    ((vsx_widget_2d_edit*)edit1)->value.str("localhost");
//    ((vsx_widget_2d_edit*)edit1)->value.clear();
  //k_focus = edit_name;
  visible = 1;
  pos.x = 0.5-size.x/2;
  pos.y = 0.5-size.y/2;
}

void vsx_window_object_inspector::unload() {
  resize_to(vsx_vector(0.001,0.001));
  move(1-0.001,1,0);
#ifndef VSXU_PLAYER
  texture.unload();
#endif
  filename_loaded = "";
  view_type = 0;
}

void vsx_window_object_inspector::load_file(vsx_string filename) {
  //cout << "trying to load " << filename << endl;
  filename = vsx_get_data_path()+filename;
#ifdef VSXU_DEBUG
  printf("filename: %s\n", filename.c_str());
#endif
  std::vector<vsx_string> parts;
  vsx_string deli = ".";
  explode(filename, deli, parts);
  if (filename_loaded != filename)
  {
    if (parts[parts.size()-1] == "jpg") {
      view_type = 1; // image viewer
      texture_loaded = false;
#ifndef VSXU_PLAYER
      texture.texture_info->size_y = 0;
//      if (texture.valid)
      texture.unload();
      texture.init_opengl_texture_2d();
      texture.load_jpeg(filename,false);
      //float screenaspect = screen_x/screen_y;
      //printf("%f, %f\n",texture.texture_info.size_x,texture.texture_info.size_y);
#endif
      filename_loaded = filename;
    } else
    if (parts[parts.size()-1] == "png") {
      //printf("png");
      //texture.unload();
      view_type = 1; // image viewer
      texture_loaded = false;
#ifndef VSXU_PLAYER
      texture.texture_info->size_y = 0;
//      if (texture.valid)
      texture.unload();
      texture.load_png(filename,false);
      //float screenaspect = screen_x/screen_y;

      //printf("%f, %f\n",texture.texture_info.size_x,texture.texture_info.size_y);
#endif
      filename_loaded = filename;

      //vsx_vector aa;
      //aa.x = 0.4/screenaspect*(texture.size_x/texture.size_y);
      //aa.y = 0.4;
      //aa.z = 0;
      //resize_to(aa);
      //move(1-0.4/screenaspect*(texture.size_x/texture.size_y),1,0);
      //resize_to(vsx_vector(0.1,0.1,0));
      //move(0.9,1,0);
    }
  }
}

#endif
