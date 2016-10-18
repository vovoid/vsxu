/**
* Project: VSXu Profiler - Data collection and data visualizer
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

#ifndef VSX_WIDGET_DESKTOP_H
#define VSX_WIDGET_DESKTOP_H

#include <vsx_widget.h>

#include "profiler/vsx_widget_profiler.h"

class vsx_profiler_desktop : public vsx_widget
{
  vsx_widget_profiler* profiler;
  std::unique_ptr<vsx_texture<>> mtex;

public:
  vsx_command_list* system_command_queue;

  double xpp,ypp,zpp;
  void init()
  {
    enabled = true;
    name = "desktop";
    widget_type = 1000;


    profiler = (vsx_widget_profiler*)this->add((vsx_widget*)(new vsx_widget_profiler), "profiler");
    profiler->init();

    vsx_widget_skin::get_instance()->skin_path_set( PLATFORM_SHARED_FILES+vsx_string<>("gfx")+DIRECTORY_SEPARATOR+"vsxu_luna"+DIRECTORY_SEPARATOR );
    vsx_widget_skin::get_instance()->init();

    vsx_widget_global_interpolation::get_instance()->set( 1.0);
    camera.set_key_speed( 3.0 );

    vsx::filesystem filesystem;
    font.load(PLATFORM_SHARED_FILES+"font"+DIRECTORY_SEPARATOR+"font-ascii.png", &filesystem);

    mtex = vsx_texture_loader::load(
      vsx_widget_skin::get_instance()->skin_path_get()+"desktop.jpg",
      vsx::filesystem::get_instance(),
      false, // threaded
      vsx_bitmap::flip_vertical_hint,
      vsx_texture_gl::linear_interpolate_hint
    );

    init_children();

    init_run = true;
  }


  void reinit()
  {
    vsx_widget::reinit();
  }

  bool input_key_down(uint16_t key)
  {
    if (!k_focus)
      return true;

    if (!k_focus->event_key_down(key))
      return true;

    camera.event_key_down(key);
    return false;
  }

  bool input_key_up(uint16_t key)
  {
    if (!k_focus)
      return true;

    if (!k_focus->event_key_up(key))
      return false;

    camera.event_key_up(key);
    return false;
  }

  void event_mouse_wheel(float y)
  {
    camera.increase_zps( -y );
  }

  void command_process_back_queue(vsx_command_s *t)
  {
    if (
      t->cmd == "system.shutdown" ||
      t->cmd == "fullscreen" ||
      t->cmd == "fullscreen_toggle"
    )
    {
      system_command_queue->addc(t);
      return;
    }

    if (t->cmd == "conf")
      configuration[t->parts[1]] = t->parts[2];
  }

  void draw()
  {
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
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glColor4f(1,1,1,1);

    if (!mtex->bind())
      VSX_ERROR_RETURN("Could not bind texture");

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

    draw_children();
  }

  void draw_2d()
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
  vsx_profiler_desktop()
  {
    root = this;
    enabled = false;

    a_focus = this;
    k_focus = this;
    m_focus = this;
  }

};








#endif
