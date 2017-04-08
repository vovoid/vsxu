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


#include "vsx_gl_global.h"
#include <map>
#include <list>
#include <vector>
#include <math.h>
#include <command/vsx_command.h>
#include <command/vsx_command_list.h>
#include <command/vsx_command_client_server.h>
#include "vsx_font.h"
#include <vsx_data_path.h>
#include <texture/vsx_texture.h>

// local includes
#include "vsx_widget.h"
#include "vsx_widget_object_inspector.h"
#include "server/vsx_widget_anchor.h"
#include "server/vsx_widget_connector_bezier.h"
#include "server/vsx_widget_comp.h"
#include <filesystem/vsx_filesystem.h>
#include "vsx_param.h"
#include <module/vsx_module.h>
#include <time/vsx_timer.h>
#include "widgets/vsx_widget_panel.h"
#include "widgets/vsx_widget_base_edit.h"
#include "widgets/vsx_widget_button.h"
#include "widgets/vsx_widget_label.h"


void vsx_window_object_inspector::draw_2d() {
	vsx_widget_window::draw_2d();

  if (view_type == 1 && texture)
  {
    pos_.y = pos.y+size.y-font_size;
    size_.y = size.y-font_size-dragborder;
    pos_.x = pos.x+dragborder;
    size_.x = size.x-dragborder*2;
    title = filename_loaded+" "+vsx_string_helper::i2s((int)texture->texture->bitmap->width)+"x"+vsx_string_helper::i2s((int)texture->texture->bitmap->height);
    glColor4f(1,1,1,1);
      texture->bind();
        glBegin(GL_QUADS);
          glTexCoord2i(0, 0);
          glVertex3f(pos_.x,pos_.y,0);
          glTexCoord2i(1, 0);
          glVertex3f(pos_.x+size_.x,pos_.y,0);
          glTexCoord2i(1, 1);
          glVertex3f(pos_.x+size_.x,pos_.y-size_.y,0);
          glTexCoord2i(0, 1);
          glVertex3f(pos_.x,pos_.y-size_.y,0);
        glEnd();
      texture->_bind();
    float screenaspect = screen_x/screen_y;

    if (texture_loaded == false)
      if (texture->texture->bitmap->width != 0.0) {
        texture_loaded = true;
        vsx_vector3<> aa;
        aa.x = 0.4/screenaspect;
        aa.x *= ((float)texture->texture->bitmap->width/(float)texture->texture->bitmap->height);
        aa.y = 0.4;
        aa.z = 0;
        resize_to(aa);
        move(1*screen_aspect-0.4/screenaspect*((float)texture->texture->bitmap->width/(float)texture->texture->bitmap->height),1.0f-aa.y,0);
      }
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
      label2->title = vsx_string<>("Anchor data type:")+((vsx_widget_anchor*)inspected)->p_type;
      label1->visible = label2->visible = 1;
      if (((vsx_widget_anchor*)inspected)->alias) {
      	label1->title = "Rename alias:";
      	component_rename_button->visible = 1;
      	component_rename_edit->visible = 1;
      }
      ((vsx_widget_base_edit*)component_rename_edit)->set_string(((vsx_widget_anchor*)a_focus)->name);
      title = "anchor "+((vsx_widget_anchor*)a_focus)->name;
      view_type = 0;
      set_size(vsx_vector3<>(0.2f, 0.175f, 0.0f));
      move(1.0f - 0.2f , 1.0f - 0.175f, 0.0f);
    }
    else
    if (inspected->widget_type == VSX_WIDGET_TYPE_CONNECTOR)
    {
      label1->title = ((vsx_widget_connector_bezier*)a_focus)->destination->name;
      view_type = 0;
      label1->visible = 1;
    }
    else
    if (inspected->widget_type == VSX_WIDGET_TYPE_COMPONENT)
    {
    	label1->title = "Rename module:";
    	component_rename_button->visible = 1;
    	component_rename_edit->visible = 1;

      for (unsigned long i = 0; i < component_list.size(); ++i) component_list[i]->visible = 1;

      ((vsx_widget_base_edit*)component_rename_edit)->set_string(((vsx_widget_component*)a_focus)->real_name);
      ((vsx_widget_base_edit*)component_rename_edit)->caret_goto_end();
      title = ((vsx_widget_component*)a_focus)->real_name;
      view_type = 0;
      set_size(vsx_vector3<>(0.2f, 0.175f, 0.0f));
      move(1.0f - 0.2f , 1.0f - 0.175f, 0.0f);
      // request timing data
      label2->title = "This build of the engine does not\nsupport timing. The reason?\nTiming slows down performance.\n";
#ifdef VSXU_MODULE_TIMING
			command_q_b.add_raw("component_timing "+((vsx_widget_component*)a_focus)->real_name+" "+vsx_string_helper::i2s(id));
#endif
  		((vsx_widget_component*)a_focus)->server->vsx_command_queue_b(this);
    } else
    if (inspected->widget_type == VSX_WIDGET_TYPE_ULTRA_CHOOSER)
    {

    } else view_type = 0;
    // image viewer
  }
  vsx_status_timer -= vsx_widget_time::get_instance()->get_dtime();
  if (inspected->widget_type == VSX_WIDGET_TYPE_COMPONENT)
  {
  	if (vsx_status_timer < 0.0f) {
  		vsx_status_timer = 1.0f;
#ifdef VSXU_MODULE_TIMING
			command_q_b.add_raw("component_timing "+((vsx_widget_component*)inspected)->real_name+" "+vsx_string_helper::i2s(id));
  		((vsx_widget_component*)inspected)->server->vsx_command_queue_b(this);
#endif
  	}
  }
}

void vsx_window_object_inspector::command_process_back_queue(vsx_command_s *t) {
  if (k_focus == component_rename_edit || t->cmd == "component_rename_button") {
    if (inspected->widget_type == VSX_WIDGET_TYPE_COMPONENT) {
      vsx_widget_component* cc = (vsx_widget_component*)inspected;
      vsx_string<>curname = "";
      vsx_string<>newname = "";
      newname += ((vsx_widget_base_edit*)component_rename_edit)->get_string();
      command_q_b.add_raw("component_rename " +cc->name+" "+newname);
      cc->server->vsx_command_queue_b(this);
    } else
    if (inspected->widget_type == VSX_WIDGET_TYPE_ANCHOR) {
      command_q_b.add_raw(
        "pa_ren "+ // command
        ((vsx_widget_anchor*)inspected)->component->name+" "+ // component
        ((vsx_widget_anchor*)inspected)->name+" "+
        ((vsx_widget_base_edit*)component_rename_edit)->get_string()+" "+
        vsx_string_helper::i2s(((vsx_widget_anchor*)inspected)->io)
      );
      ((vsx_widget_component*)((vsx_widget_anchor*)inspected)->component)->server->vsx_command_queue_b(this);
    }
  }
  if (t->cmd == "component_timing_ok")
  {
  	label2->title = vsx_string<>("run time:   ")+t->parts[2] + " seconds\noutput time:" + t->parts[3] + " seconds\n";
  	label2->title = label2->title + "% of total frame time: "+vsx_string_helper::f2s( (vsx_string_helper::s2f(t->parts[2])+vsx_string_helper::s2f(t->parts[3]))*100.0f / vsx_string_helper::s2f(t->parts[4]),3);

  }
}


vsx_window_object_inspector::vsx_window_object_inspector()
{
  vsx_widget_window::init();
  texture = 0;
  inspected  = 0;
  init_run = false;
  init_run = true;
  component_rename_edit = add(new vsx_widget_base_edit,"edit1");
  component_rename_edit->init();
  topmost = true;
  component_rename_button = add(new vsx_widget_button,"button_rename");
  label1 = add(new vsx_widget_label,"label1");
  label2 = add(new vsx_widget_label,"label1");

  vsx_widget::init_children();
  title = "object inspector";
  set_pos(vsx_vector3<>(1.0f,1.0f));
  set_size(vsx_vector3<>(0.25f,0.05f));
  allow_resize_x = allow_resize_y = 1;

  //-- COMPONENT INSPECTOR ---------------------------------------------------------------------------------------------

  component_rename_button->visible = 0;
  component_rename_button->set_size(vsx_vector3<>(0.033,0.020));
  component_rename_button->set_pos(vsx_vector3<>(0.18,0.025));
  component_rename_button->title = "RENAME";
  component_rename_button->commands.adds(4,"component_rename_button","component_rename_button","");
  ((vsx_widget_button*)component_rename_button)->border = 0.0003;

  component_rename_edit->visible = 1;
  component_rename_edit->set_pos(vsx_vector3<>(0.085f,0.025f));
  component_rename_edit->set_size(vsx_vector3<>(0.155f,0.02f));
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

  ((vsx_widget_label*)label1)->halign = a_left;
  label1->pos.x = 0.005f + size.x * 0.5f;
  label1->pos.y = 0.05f;
  label1->size = vsx_vector3f(size.x, 0.02f);
  label1->visible = 1;
  ((vsx_widget_label*)label2)->halign = a_left;
  label2->pos.x = 0.005f + size.x * 0.5f;
  label2->pos.y = 0.12f;
  label2->size = vsx_vector3f(size.x, 0.02f);
  label2->visible = 0;

  // normal OI
  view_type = 0;
}

void vsx_window_object_inspector::show() {
  visible = 1;
  pos.x = 0.5-size.x/2;
  pos.y = 0.5-size.y/2;
}


void vsx_window_object_inspector::unload()
{
  texture.reset(nullptr);
}

void vsx_window_object_inspector::load_file(vsx_string<>filename)
{
  filename = vsx_data_path::get_instance()->data_path_get() + filename;

  if (filename_loaded == filename)
    return;

  texture = vsx_texture_loader::load(
    filename,
    vsx::filesystem::get_instance(),
    true, // threaded
    (int)vsx_string_helper::verify_filesuffix(filename, "tga") * vsx_bitmap::flip_vertical_hint, // only flip TGAs
    vsx_texture_gl::linear_interpolate_hint | vsx_texture_gl::generate_mipmaps_hint
  );

  view_type = 1;
  texture_loaded = false;
  filename_loaded = filename;
}

