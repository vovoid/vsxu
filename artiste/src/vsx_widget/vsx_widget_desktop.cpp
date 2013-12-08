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
#include "vsx_texture_info.h"
#include "vsx_texture.h"
#include "vsx_command.h"
#include "vsx_font.h"
#include "vsx_param.h"
#include "vsx_module.h"

#include <sys/types.h>
#include <sys/stat.h>
#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
#include <io.h>
#endif
#include <fcntl.h>

// local includes
#include "vsx_widget_base.h"
#include "window/vsx_widget_window.h"
#include "helpers/vsx_widget_object_inspector.h"
#include "helpers/vsx_widget_preview.h"
#include "helpers/vsx_widget_assistant.h"
#include "helpers/vsx_widget_console.h"
#include "server/vsx_widget_anchor.h"
#include <vsx_command_client_server.h>
#include "server/vsx_widget_server.h"
#include "dialogs/vsx_widget_window_statics.h"
#include "lib/vsx_widget_panel.h"
#include "lib/vsx_widget_base_edit.h"

#include "vsx_widget_desktop.h"
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

  log("welcome to vsxu");

  init_run = true;
  logo_time = 0;
  first_draw = true;
}

void vsx_widget_desktop::reinit()
{
  vsx_widget::reinit();
  mtex.init_opengl_texture_2d();
  mtex.load_jpeg(skin_path+"desktop.jpg");
}

bool vsx_widget_desktop::key_down(signed long key, bool n_alt, bool n_ctrl, bool n_shift)
{
  this->alt = n_alt;
  this->ctrl = n_ctrl;
  this->shift = n_shift;

  if (!k_focus)
    return true;

  if (!k_focus->event_key_down(key,alt,ctrl,shift))
    return true;

  if (ctrl)
  {
    switch (key)
    {
      case -GLFW_KEY_SPACE:
        ((vsx_window_texture_viewer*)tv)->run = !((vsx_window_texture_viewer*)tv)->run;
        break;
      // fullwindow
      case -'F':
      case -'f': // F
        ((vsx_window_texture_viewer*)tv)->fullwindow = !((vsx_window_texture_viewer*)tv)->fullwindow;
        if (((vsx_window_texture_viewer*)tv)->fullwindow) mouse.hide_cursor(); else mouse.show_cursor();
        if (!((vsx_window_texture_viewer*)tv)->fullwindow) this->performance_mode = false;
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
        if (((vsx_window_texture_viewer*)tv)->fullwindow)
        {
          mouse.show_cursor();
          this->performance_mode = !this->performance_mode;
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
        if (a_focus->widget_type != VSX_WIDGET_TYPE_SERVER) {
          vsx_vector a = a_focus->get_pos_p();
          move_camera(vsx_vector(a.x,a.y,2.0f));
        } else move_camera(vsx_vector(xp,yp,2.0f));
      }
    }
    break;
    case -GLFW_KEY_UP: case -'E': case -'e':{ interpolating = false;ypd=1.0;} break;
    case -GLFW_KEY_DOWN: case -'D': case -'d':{ interpolating = false;ypd=-1.0;} break;
    case -GLFW_KEY_LEFT: case -'s': case -'S':{ interpolating = false;xpd=-1.0;} break;
    case -GLFW_KEY_RIGHT: case -'F': case -'f':{ interpolating = false;xpd=1.0;} break;
    case -GLFW_KEY_PAGEUP: case -'R': case -'r':{ interpolating = false;zpd=-1.0;} break;
    case -GLFW_KEY_PAGEDOWN: case -'W': case -'w':{ interpolating = false;zpd=1.0;} break;
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
    case GLFW_KEY_UP:    case 'E': case 'e': ypd=0.0; break;
    case GLFW_KEY_DOWN:  case 'D': case 'd': ypd=0.0; break;
    case GLFW_KEY_LEFT:  case 'S': case 's': xpd=0.0; break;
    case GLFW_KEY_RIGHT: case 'F': case 'f': xpd=0.0; break;
    case GLFW_KEY_PAGEUP: case 'R': case 'r': zpd=0.0; break;
    case GLFW_KEY_PAGEDOWN: case 'W': case 'w': zpd=0.0; break;
    case GLFW_KEY_TAB: ((vsxu_assistant*)assistant)->toggle_size(); break;
  }

  return false;
}

void vsx_widget_desktop::event_mouse_wheel(float y)
{
  zps += -y;

  if (zps > 2.0f) zps = 2.0f;
  if (zps < -2.0f) zps = -2.0f;
}

void vsx_widget_desktop::move_camera(vsx_vector world)
{
  camera_target = world;
  interpolating = true;
}

void vsx_widget_desktop::draw() {
  if (!init_run) return;
  // this is designed to be root, so set up things

  // Deal with movement around the desktop

  #define SGN(N) (N >= 0 ? 1 : -1)
  #define MAX(N, M) ((N) >= (M) ? (N) : (M))
  #define MIN(N, M) ((N) <= (M) ? (N) : (M))
  #define CLAMP(N, L, U) (MAX(MIN((N), (U)), (L)))
  if (!interpolating)
  {
    double acc = 4, dec = 3, spd = global_key_speed;
    // interpolation falloff control
    float tt = dtime*interpolation_speed*global_interpolation_speed;
    if (tt > 1) { tt = 1; }

    if(zpd != 0.0) {
      double sgn = SGN(zpd);
      zps += dtime * acc * sgn * global_interpolation_speed;
      zps = CLAMP(zps, -1.2f, 1.2f);
    }
    if(zpd == 0.0) {
      double sgn = SGN(zps);
      zps -= dtime * dec * sgn * global_interpolation_speed;
      zps = MAX(zps * sgn, 0) * sgn;
    }

    zp += zps * fabs(zp - 1.1)* spd * dtime + zpp*(zp - 1.0f);
    zpp = zpp*(1-tt);

    if (zp > 100) {zp = 100; zps = 0;}
    if (zp < 1.2) {zp = 1.2; zps = 0;}

    if(xpd != 0.0) {
      double sgn = SGN(xpd);
      xps += dtime * acc * sgn * global_interpolation_speed;
      xps = CLAMP(xps, -1, 1);
    }
    if(xpd == 0.0) {
      double sgn = SGN(xps);
      xps -= dtime * dec * sgn * global_interpolation_speed;
      xps = MAX(xps * sgn, 0) * sgn;
    }
    xp += xps * fabs(zp - 1.1)* spd * dtime*0.6 + xpp*(zp-1.0f);
    xpp = xpp*(1-tt);

    if (xp > 10) {xp = 10; xps = 0;}
    if (xp < -10) {xp = -10; xps = 0;}

    if(ypd != 0.0) {
      double sgn = SGN(ypd);
      yps += dtime * acc * sgn * global_interpolation_speed;
      yps = CLAMP(yps, -1, 1);
    }
    if(ypd == 0.0) {
      double sgn = SGN(yps);
      yps -= dtime * dec * sgn * global_interpolation_speed;
      yps = MAX(yps * sgn, 0) * sgn;
    }
    yp += yps * fabs(zp - 1.1)* spd * dtime*0.6 + ypp*(zp-1.0f);
    ypp = ypp*(1-tt);

    if (yp > 10) {yp = 10; yps = 0;}
    if (yp < -10) {yp = -10; yps = 0;}

  }
  else
  {
    float tt = dtime*10.0f*global_interpolation_speed;
    if (tt > 1) { tt = 1; interpolating = false;}
    xp = xp*(1-tt)+camera_target.x*tt;
    yp = yp*(1-tt)+camera_target.y*tt;
    zp = zp*(1-tt)+camera_target.z*tt;
    if (
      (round(xp*2000) == round(camera_target.x*2000)) &&
      (round(yp*2000) == round(camera_target.y*2000)) &&
      (round(zp*2000) == round(camera_target.z*2000))
    ) interpolating = false;
  }

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45,screen_x/screen_y,0.001,120.0);

  gluLookAt(xp,yp,zp-1.1f,xp,yp,-1.1f,0.0,1.0,0.0);
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
    if (!mtex.bind())
    glColor4f(skin_colors[13].r,skin_colors[13].g,skin_colors[13].b,skin_colors[13].a);
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
    mtex._bind();
  }
  draw_children();
}




void vsx_widget_desktop::draw_2d()
{
  GLint	viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  screen_x = (float)viewport[2];
  screen_y = (float)viewport[3];

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
  gluLookAt(xp,yp,zp-1.1f,xp,yp,0.0-1.1f,0.0,1.0,0.0);

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
  vsx_command_list main_conf;
  vsx_string config_file = vsx_get_data_path() + "vsxu.conf";

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
  while ( (mc = main_conf.get()) ) {
    if (mc->cmd == "skin") {
      skin_path = PLATFORM_SHARED_FILES+vsx_string("gfx")+DIRECTORY_SEPARATOR+mc->cmd_data+DIRECTORY_SEPARATOR;
    } else
    if (mc->cmd == "global_interpolation_speed") {
      global_interpolation_speed = s2f(mc->cmd_data);
    } else
    if (mc->cmd == "automatic_undo") {
      auto_undo = vsx_string_aux::s2i(mc->cmd_data);
    } else
    if (mc->cmd == "global_framerate_limit") {
      global_framerate_limit = s2f(mc->cmd_data);
    } else
    if (mc->cmd == "global_key_speed") {
      global_key_speed = s2f(mc->cmd_data);
    }
    if (mc->cmd != "" && mc->cmd != "#")
    configuration[mc->cmd] = mc->cmd_data;
  }
}

void vsx_widget_desktop::save_configuration() {
  // final destination for the configuration data
  vsx_string save_filename = vsx_get_data_path() + "vsxu.conf";

  vsx_command_list s_conf;
  for (std::map<vsx_string, vsx_string>::iterator it = configuration.begin(); it != configuration.end(); ++it) {
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
      system_command_queue->addc(t);
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

  interpolating = false;
  root = this;
  enabled = false;

  // messing with the configuration
  skin_path = "_gfx/vsxu/";
  vsx_command_list modelist;
  vsx_command_s* mc = 0;
  load_configuration();
  font.init(PLATFORM_SHARED_FILES+"font"+DIRECTORY_SEPARATOR+"font-ascii.png");
  vsx_command_list skin_conf;
  skin_conf.load_from_file(skin_path+"skin.conf",true,4);

  skin_conf.reset();
  while ( (mc = skin_conf.get()) ) {
    if (mc->cmd == "color") {
      vsx_avector<vsx_string> parts;
      vsx_string deli = ",";
      explode(mc->parts[2],deli, parts);
      vsx_color p;
      p.r = s2f(parts[0]);
      p.g = s2f(parts[1]);
      p.b = s2f(parts[2]);
      p.a = s2f(parts[3]);
      skin_colors[ vsx_string_aux::s2i(mc->parts[1]) ] = p;
    }
  }
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

  mtex.init_opengl_texture_2d();
  mtex.load_jpeg(skin_path+"desktop.jpg");

  k_focus = this;
  m_focus = this;

  zpf = 0.0;
  zpa = 0.0;
  xpd = 0.0;
  ypd = 0.0;
  zpd = 0.0;
  zps = 0.0;
  yps = 0.0;
  xps = 0.0;
  yp = 0.0;
  xp = 0.0;
  zp = 2.0;
  ypp = zpp = xpp = 0.0f;


}
