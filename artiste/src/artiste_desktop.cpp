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
#include <vector>
#include <math.h>
#include <unistd.h>
#include "vsx_gl_global.h"
#include <texture/vsx_texture.h>
#include "vsx_command.h"
#include "vsx_font.h"
#include "vsx_param.h"
#include "vsx_module.h"

// engine
#include <vsx_data_path.h>

// local includes
#include "vsx_widget.h"
#include "vsx_widget_window.h"
#include "helpers/vsx_widget_object_inspector.h"
#include "helpers/vsx_widget_preview.h"
#include "helpers/vsx_widget_assistant.h"
#include "helpers/vsx_widget_console.h"
#include "server/vsx_widget_anchor.h"
#include <vsx_command_client_server.h>
#include "server/vsx_widget_server.h"
#include "widgets/vsx_widget_panel.h"
#include "widgets/vsx_widget_base_edit.h"

#include "artiste_desktop.h"
#include "GL/glfw.h"

// VSX_WIDGET_DESKTOP **************************************************************************************************
// VSX_WIDGET_DESKTOP **************************************************************************************************
// VSX_WIDGET_DESKTOP **************************************************************************************************
// VSX_WIDGET_DESKTOP **************************************************************************************************
void vsx_widget_desktop::init()
{
  // popup window
  add(new vsx_window_object_inspector(),"object_inspector");

  assistant = add(new vsxu_assistant(),"luna");
  front(assistant);

  enabled = true;
  name = "desktop";
  widget_type = VSX_WIDGET_TYPE_DESKTOP;

  init_children();

  font.load(PLATFORM_SHARED_FILES+"font"+DIRECTORY_SEPARATOR+"font-ascii.png", vsxf::get_instance());

  log("welcome to vsxu");

  init_run = true;
  logo_time = 0;
  first_draw = true;
}

void vsx_widget_desktop::reinit()
{
  vsx_widget::reinit();
}

void vsx_widget_desktop::unload()
{
  font.unload();
}

bool vsx_widget_desktop::key_down(signed long key, bool n_alt, bool n_ctrl, bool n_shift)
{
  this->alt = n_alt;
  this->ctrl = n_ctrl;
  this->shift = n_shift;

  if (!k_focus)
    return true;

  bool k_focus_result = k_focus->event_key_down(key,alt,ctrl,shift);

  if (!k_focus_result)
    return true;

  if (ctrl)
  {
    switch (key)
    {
      case -GLFW_KEY_SPACE:
        ((vsx_window_texture_viewer*)tv)->toggle_run();
        break;
      // fullwindow
      case -'F':
      case -'f': // F
        ((vsx_window_texture_viewer*)tv)->toggle_fullwindow();

        if (((vsx_window_texture_viewer*)tv)->get_fullwindow())
        {
          mouse.hide_cursor();
        }
        else
        {
          mouse.show_cursor();
        }

        if (!((vsx_window_texture_viewer*)tv)->get_fullwindow())
          this->performance_mode = false;
      break;
      // close all controllers
      case -'C':
      case -'c':
        delete_all_by_type(VSX_WIDGET_TYPE_CONTROLLER);
      break;
      // close all open anchors
      case -'D':
      case -'d':
        for (std::map<int, vsx_widget*>::iterator it = global_index_list.begin();  it != global_index_list.end(); ++it) {
          if ((*it).second->widget_type == VSX_WIDGET_TYPE_ANCHOR) {
            if ((*it).second->parent)
            if ((*it).second->parent->widget_type == VSX_WIDGET_TYPE_COMPONENT) {
              ((vsx_widget_anchor*)(*it).second)->toggle(1);
            }
          }
        }
      break;
    }
  }

  if (alt)
  {
    switch (key)
    {
      case -'F':
      case -'f': // F
        if (((vsx_window_texture_viewer*)tv)->get_fullwindow())
        {
          this->performance_mode = !this->performance_mode;
        }
        if (this->performance_mode)
        {
          mouse.show_cursor();
        } else
        {
          mouse.hide_cursor();
        }

      default:
        break;
    }
  }

  if (!ctrl && !alt)
  switch (key)
  {
    case -GLFW_KEY_SPACE:
    {
      {
        if (a_focus->widget_type != VSX_WIDGET_TYPE_SERVER)
        {
          vsx_vector3<> a = a_focus->get_pos_p();
          camera.move_camera(vsx_vector3<>(a.x,a.y,2.0f));
        }
        else
          camera.set_distance(2.0f);;
      }
    }
    break;
    case -GLFW_KEY_UP: case -'E': case -'e':
      camera.set_movement_y( 1.0 );
    break;

    case -GLFW_KEY_DOWN: case -'D': case -'d':
      camera.set_movement_y( -1.0 );
    break;
    case -GLFW_KEY_LEFT: case -'s': case -'S':
      camera.set_movement_x( -1.0 );
    break;
    case -GLFW_KEY_RIGHT: case -'F': case -'f':
      camera.set_movement_x( 1.0 );
    break;
    case -GLFW_KEY_PAGEUP: case -'R': case -'r':
      camera.set_movement_z( -1.0 );
    break;
    case -GLFW_KEY_PAGEDOWN: case -'W': case -'w':
      camera.set_movement_z( 1.0 );
    break;
  } // switch
  return false;
}


bool vsx_widget_desktop::key_up(signed long key, bool alt, bool ctrl, bool shift)
{
  this->ctrl = ctrl;

  if (!k_focus)
    return true;

  if (!k_focus->event_key_up(key,alt,ctrl,shift))
    return false;

  switch (key)
  {
    case GLFW_KEY_UP: case 'E': case 'e':
    case GLFW_KEY_DOWN: case 'D': case 'd':
      camera.set_movement_y( 0.0 );
    break;

    case GLFW_KEY_LEFT: case 'S': case 's':
    case GLFW_KEY_RIGHT: case 'F': case 'f':
      camera.set_movement_x( 0.0 );
    break;

    case GLFW_KEY_PAGEUP: case 'R': case 'r':
    case GLFW_KEY_PAGEDOWN: case 'W': case 'w':
      camera.set_movement_z( 0.0 );
    break;

    case GLFW_KEY_TAB:
      ((vsxu_assistant*)assistant)->toggle_size();
    break;
  }

  return false;
}

void vsx_widget_desktop::event_mouse_wheel(float y)
{
  camera.increase_zps( -y );
}

void vsx_widget_desktop::draw() {
  if (!init_run)
    return;

  camera.run();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45,screen_x/screen_y,0.001,120.0);

  gluLookAt(
        camera.get_pos_x(), camera.get_pos_y(), camera.get_pos_z() - 1.1f,
        camera.get_pos_x(), camera.get_pos_y(), -1.1f,
        0.0,1.0,0.0
  );
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  if (!performance_mode)
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  if (!performance_mode)
  {
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glColor4f(1,1,1,1);
    mtex->bind();
    vsx_widget_skin::get_instance()->set_color_gl(13);
      glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex3f(pos.x-size.x/2,pos.y-size.y/2,-10.0f);
        glTexCoord2f(0, 1);
        glVertex3f(pos.x-size.x/2,pos.y+size.y/2,-10.0f);
        glTexCoord2f(1, 1);
        glVertex3f(pos.x+size.x/2,pos.y+size.y/2,-10.0f);
        glTexCoord2f(1, 0);
        glVertex3f(pos.x+size.x/2,pos.y-size.y/2,-10.0f);
      glEnd();
    mtex->_bind();
  }
  draw_children();
}


void vsx_widget_desktop::draw_2d()
{
  screen_x = vsx_gl_state::get_instance()->viewport_get_width();
  screen_y = vsx_gl_state::get_instance()->viewport_get_height();

  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
  glMatrixMode(GL_PROJECTION);

  glLoadIdentity();
  screen_aspect = 1.0f;
  gluOrtho2D(0, screen_aspect, 0, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  vsx_widget::draw_2d();

  // get the mouse area done
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45,(float)screen_x/(float)screen_y,0.001,120.0);
  glMatrixMode(GL_MODELVIEW);

  gluLookAt(
        camera.get_pos_x(), camera.get_pos_y(), camera.get_pos_z() - 1.1f,
        camera.get_pos_x(), camera.get_pos_y(), -1.1f,
        0.0,1.0,0.0
  );


  glBlendFunc(GL_ONE, GL_ONE);
  glDepthMask(GL_TRUE);
  glClear(GL_DEPTH_BUFFER_BIT);

  glColor4f(0,0,0,1.0f);
  glBegin(GL_QUADS);
    glVertex3f(-800,-800,0.0);
    glVertex3f(-800,800,0.0);
    glVertex3f(800,800,0.0);
    glVertex3f(800,-800,0.0);
  glEnd();
}


void vsx_widget_desktop::load_configuration()
{
  vsx_command_list main_conf(true);
  vsx_string<>config_file = vsx_data_path::get_instance()->data_path_get() + "vsxu.conf";

  if (access(config_file.c_str(),0) == 0)
  {
    main_conf.load_from_file(config_file.c_str(),true,0);
  } else
  {
    main_conf.load_from_file(PLATFORM_SHARED_FILES+"vsxu.conf",true,0);
  }

  main_conf.reset();
  vsx_command_s* mc = 0;
  auto_undo = 1;
  while ( (mc = main_conf.get()) )
  {
    if (mc->cmd == "skin")
    {
      vsx_widget_skin::get_instance()->skin_path_set( PLATFORM_SHARED_FILES+vsx_string<>("gfx")+DIRECTORY_SEPARATOR + mc->cmd_data+DIRECTORY_SEPARATOR );
      vsx_widget_skin::get_instance()->init();
    } else
    if (mc->cmd == "global_interpolation_speed") {
      vsx_widget_global_interpolation::get_instance()->set( vsx_string_helper::s2f(mc->cmd_data) );
    } else
    if (mc->cmd == "automatic_undo") {
      auto_undo = vsx_string_helper::s2i(mc->cmd_data);
    } else
    if (mc->cmd == "global_framerate_limit") {
      global_framerate_limit = vsx_string_helper::s2f(mc->cmd_data);
    } else
    if (mc->cmd == "global_key_speed") {
      camera.set_key_speed( vsx_string_helper::s2f(mc->cmd_data) );
    }
    if (mc->cmd != "" && mc->cmd != "#")
    configuration[mc->cmd] = mc->cmd_data;
  }
}

void vsx_widget_desktop::save_configuration() {
  // final destination for the configuration data
  vsx_string<>save_filename = vsx_data_path::get_instance()->data_path_get() + "vsxu.conf";

  vsx_command_list s_conf;
  for (std::map<vsx_string<>, vsx_string<> >::iterator it = configuration.begin(); it != configuration.end(); ++it) {
    s_conf.add_raw((*it).first+" "+(*it).second);
  }
  s_conf.save_to_file(save_filename);
}

  void vsx_widget_desktop::command_process_back_queue(vsx_command_s *t) {
    if (
      t->cmd == "system.shutdown" ||
      t->cmd == "fullscreen" ||
      t->cmd == "fullscreen_toggle"
    )
    {
      system_command_queue->addc(t, VSX_COMMAND_GARBAGE_COLLECT);
    }
    else
    if (t->cmd == "conf") {
      configuration[t->parts[1]] = t->parts[2];
      save_configuration();
      load_configuration();
    }
  }

vsx_widget_desktop::vsx_widget_desktop()
{
  root = this;
  enabled = false;

  // messing with the configuration

  load_configuration();


  // server widget
  sv = add(new vsx_widget_server,"desktop_local");

  sv->color.b = 255.0/255.0;
  sv->color.g = 200.0/255.0;
  sv->color.r = 200.0/255.0;
  sv->size.x = 5;
  sv->size.y = 5;

  // preview window
  tv = add(new vsx_window_texture_viewer(),"vsxu_preview");
  tv->init();
  ((vsx_window_texture_viewer*)tv)->set_server(sv);
  //---
  a_focus = sv;
  m_focus = sv;
  k_focus = sv;

  console =  add(new vsx_widget_2d_console,"system_console");

  ((vsx_widget_2d_console*)console)->set_destination(sv);

  mtex = vsx_texture_data_loader_jpg::get_instance()->load(vsx_widget_skin::get_instance()->skin_path_get()+"desktop.jpg", vsxf::get_instance(), true, true);

  k_focus = this;
  m_focus = this;
}
