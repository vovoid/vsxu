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
#include <gl_helper.h>
#include <map>
#include <list>
#include <vector>
#include <math.h>
#include <texture/vsx_texture.h>
#include <command/vsx_command.h>
#include <command/vsx_command_list.h>
#include <command/vsx_command_client_server.h>
#include "vsx_font.h"
#include <texture/vsx_texture.h>

// local includes
#include "vsx_widget.h"
#include "widgets/vsx_widget_2d_pager.h"
#include "widgets/vsx_widget_popup_menu.h"
#include "vsx_widget_object_inspector.h"
#include <filesystem/vsx_filesystem.h>
#include "vsx_param.h"
#include <module/vsx_module.h>
#include <time/vsx_timer.h>
#include "vsx_engine.h"
#include "widgets/vsx_widget_panel.h"
#include "widgets/vsx_widget_base_edit.h"
#include "vsx_widget_assistant.h"
#include "artiste_desktop.h"
#include <vsx_data_path.h>
// -------------------------------------------------------------------------------------------------------------------
//-- VSXU ASSISTANT -----------------------------------------------------------------------------------------------

void vsxu_assistant::i_draw()
{
  if (auto_)
  {
    reload = false;

    if (help_timestamp != timestamp)
    {
      timestamp = help_timestamp;
      reload = true;
    }
    if (a_focus != this)
    if (a_focus->widget_type != VSX_WIDGET_TYPE_2D_MENU_POPUP)
    if (a_focus != inspected)
    reload = true;

    if (reload)
    {

      inspected = a_focus;
      if (course.size())
      course.clear();
      vsx_nw_vector<vsx_string<> > parts;
      vsx_string<>deli = "|||";
      if (inspected->help_text != "") {
        vsx_string_helper::explode(inspected->help_text, deli, parts);
        foreach(parts, i)
          course.push_back(parts[i]);

      }
      ((vsx_widget_2d_pager*)pager)->set_max_page( course.size() );
      ((vsx_widget_2d_pager*)pager)->set_cur_page( 0 );
    }
  }

  tt = vsx_widget_time::get_instance()->get_dtime() * 10.0f *
      vsx_widget_global_interpolation::get_instance()->get()
      ;
  if (tt > 1) tt = 1;
	clickpoint.x = clickpoint.x*(1-tt)+size_multiplier*tt;

  size.x = 0.3*clickpoint.x*(screen_aspect);
  size.y = 0.5*clickpoint.x;
  alpha = (clickpoint.x-0.2)*2;
  set_pos(vsx_vector3<>((screen_aspect-size.x),-0.03));

  pager->set_pos(vsx_vector3<>(0.065f*clickpoint.x,clickpoint.x*0.14f));
  pager->set_size(vsx_vector3<>(0.1f*clickpoint.x,0.03f*clickpoint.x));

  texture->bind();
    glColor3f(1,1,1);
    draw_box_tex_upside_down(pos, size.x, size.y);
  texture->_bind();


  if (alpha > 0.01)
  {
    if (course.size() && ((vsx_widget_2d_pager*)pager)->get_cur_page() >= 0)
    {
      text = course[ ((vsx_widget_2d_pager*)pager)->get_cur_page() ];
      font.color.a = alpha;
      font.print
      (
        vsx_vector3<>(
          (pos.x + size.x * 0.06) * screen_aspect,
          pos.y + size.y - (size.y * 0.12)
        ),
        text,
        0.012*clickpoint.x
      );
    }
  }
  target_pos = pos;
  target_size = size;
}

void vsxu_assistant::reinit()
{
  vsx_widget::reinit();
}

void vsxu_assistant::init()
{
  temp_size = -1.0f;
  if (!init_run) vsx_widget::init();
  render_type = render_2d;
  coord_type = VSX_WIDGET_COORD_CORNER;
  topmost = true;

  texture = vsx_texture_loader::load(
    PLATFORM_SHARED_FILES+"gfx"+DIRECTORY_SEPARATOR+"assistant_luna.dds",
    vsx::filesystem::get_instance(),
    true, // threaded
    vsx_bitmap::flip_vertical_hint,
    vsx_texture_gl::linear_interpolate_hint
  );

  if (configuration.find("assistant_size") != configuration.end())
  {
    size_multiplier = vsx_string_helper::s2f(configuration["assistant_size"]);
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
  menu->commands.adds(VSX_COMMAND_MENU, "Courses;Performance mode", "load","course_performance_mode");
  menu->commands.adds(VSX_COMMAND_MENU, "Courses;Macros", "load","course_macro");
  menu->size.x = 0.2;
  menu->init();

  vsx_command_s* c;
  vsx_command_list cla(false);
  cla.load_from_file( vsx_data_path::get_instance()->data_path_get() + "help_settings.conf",true,4);
  cla.garbage_collect();
  auto_ = false;
  while ( (c = cla.pop()) ) {
    if (c->cmd == "auto") {
      auto_ = true;
    }
  }
  if (!auto_) {
    command_q_b.add_raw("load course_intro");
    vsx_command_queue_b(this);
  }
  target_pos = pos;
  target_size = size;
}

void vsxu_assistant::on_delete()
{
  texture = nullptr;
}

void vsxu_assistant::toggle_size()
{
  if (size_multiplier == 1.3f) {
    size_multiplier = 1.0f;
  } else
  if (size_multiplier == 1.0f) {
    size_multiplier = 0.8f;
  } else
  if (size_multiplier == 0.8f) {
    size_multiplier = 0.2f;
  } else
  if (size_multiplier == 0.2f)
  {
    size_multiplier = 1.3f;
  }
  printf("assistant size is now: %f\n",size_multiplier);
  configuration["assistant_size"] = vsx_string_helper::f2s(size_multiplier);
  ((vsx_artiste_desktop*)root)->save_configuration();
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

void vsxu_assistant::command_process_back_queue(vsx_command_s *t) {
  if (t->cmd == "mini") {
    toggle_size();
    //size_multiplier = 0.2;
  }
  else
  if (t->cmd == "auto")
  {
    if (!auto_)
    {
      inspected = 0;
      vsx_command_list cla(true);
      cla.add_raw("auto");

      vsx_string<> cla_string = cla.get_as_string();
      vsx_string_helper::write_to_file(
          vsx_data_path::get_instance()->data_path_get() + "help_settings.conf",
          cla_string
        );

      auto_ = true;
      course.clear();
      ((vsx_widget_2d_pager*)pager)->set_max_page( 0 );//course.size();
    }
  }
  else
  if (t->cmd == "load")
  {
    auto_ = false;
    vsx_command_s* c;
    vsx_command_list cl(true);
    cl.load_from_file(PLATFORM_SHARED_FILES+"doc/"+t->cmd_data+".txt",false,4);
    vsx_string<>newstr;
    course.clear();
    while ((c = cl.pop()))
    {
      if (c->raw == "|||") {
        course.push_back(newstr);
        newstr = "";
      } else {
        newstr = newstr + c->raw+"\n";
      }
      delete c;
    }
    course.push_back(newstr);
    ((vsx_widget_2d_pager*)pager)->set_max_page( course.size() );
    ((vsx_widget_2d_pager*)pager)->set_cur_page( 0 );
  }
}

vsxu_assistant::vsxu_assistant() {
  timestamp = 0;
  reload = false;
}
