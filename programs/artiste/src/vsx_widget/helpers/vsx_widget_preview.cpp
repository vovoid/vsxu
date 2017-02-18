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
#include <vsx_gl_global.h>
#include "vsx_font.h"
// local includes
#include "vsx_engine.h"

#include "vsx_version.h"

#include "vsx_widget.h"
#include <command/vsx_command.h>
#include <command/vsx_command_list.h>
#include <command/vsx_command_client_server.h>
#include "server/vsx_widget_server.h"
#include <vsx_gl_state.h>


#include "vsx_widget_preview.h"

void vsx_window_texture_viewer::draw_2d()
{
  visible = !performance_mode;

  if (performance_mode)
    return;

  frame_delta += vsx_widget_time::get_instance()->get_dtime();
  ++frame_count;

  if (frame_count == 50)
  {
    fps = 1/(frame_delta/frame_count);
    frame_count = 0;
    frame_delta = 0;
  }

  float vis = visible;

  if (!visible)
    color.a = 0.3;
  else
    color.a = 1;

  visible = 1;
  vsx_widget_window::draw_2d();
  visible = vis;
  pos_.y = pos.y+dragborder;
  size_.y = size.y-font_size-dragborder;
  pos_.x = pos.x+dragborder;
  size_.x = size.x-dragborder*2;

  title = VSXU_VERSION " @ "+
      vsx_string_helper::i2s( (int)ceil(size_.x*(screen_x-1)) )+"x"+
      vsx_string_helper::i2s( (int)ceil(size_.y*(screen_y-1)) )+", "+
      vsx_string_helper::i2s((int)round(fps))+" FPS, Ctrl+F(ullscreen)";

  if (visible)
    glColor4f(0,0,0,1);
  else
    glColor4f(0,0,0,0.3);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  if (*engine == 0)
  {
    set_pos(vsx_vector3<>(0.0,2.0,0.0));
    set_size(vsx_vector3<>(0.0,0.0,0.0));
  }
  // set new viewport
  if (run)
  if (*engine)
  {
    // get viewport
    GLint	viewport[4];
    vsx_gl_state::get_instance()->viewport_get(&viewport[0]);
    vsx_gl_state::get_instance()->viewport_set(
      (int)ceil(pos_.x*(screen_x-1)),
      (int)ceil((pos_.y)*(screen_y-1)),
      (int)ceil(size_.x*(screen_x-1)),
      (int)ceil((size_.y)*(screen_y-1))
    );

    glScissor(
      (int)ceil(pos_.x*(screen_x-1)),
      (int)ceil((pos_.y)*(screen_y-1)),
      (int)ceil(size_.x*(screen_x-1)),
      (int)ceil(size_.y*(screen_y-1))
    );

    glEnable(GL_SCISSOR_TEST);

    // render the engine
    ((vsx_engine*)(*engine))->render();

    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_FILL);
    glDisable(GL_POLYGON_SMOOTH);


    // reset the viewport
    vsx_gl_state::get_instance()->viewport_set(&viewport[0]);

    glScissor(viewport[0],viewport[1],viewport[2],viewport[3]);
    glDisable(GL_SCISSOR_TEST);
  }
  glDepthMask(GL_TRUE);
  glDisable(GL_DEPTH_TEST);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, 1, 0, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  draw_children_2d();
}

bool vsx_window_texture_viewer::event_key_down(uint16_t key)
{
  if (vsx_input_keyboard.pressed_ctrl() && key == VSX_SCANCODE_SPACE)
  {
    run = !run;
    return false;
  }

  if (key == VSX_SCANCODE_SPACE)
  {
    run = !run;
    return false;
  }

  if (key == VSX_SCANCODE_RETURN)
  {
    fullwindow = !fullwindow;
  }
  return false;
}

void vsx_window_texture_viewer::command_process_back_queue(vsx_command_s *t)
{
  if (t->cmd == "fullwindow")
  {
    fullwindow = !fullwindow;
    return;
  }

  if (t->cmd == "toggle")
  {
    run = !run;
    return;
  }

  vsx_widget::command_process_back_queue(t);
}

void vsx_window_texture_viewer::init()
{
  if (init_run)
    return;

  init_children();

  vsx_widget_window::init();
  topmost = true;
  title = "vsxu output preview";
  allow_resize_x = true;
  allow_resize_y = true;
  visible = 1;
  size_min.x = 0.2;
  size_min.y = 0.15;
  init_run = true;

  // start in fullwindow mode (from command line arguments)
  if (vsx_argvector::get_instance()->has_param("ff") )
  {
    fullwindow = true;
  }
}

void vsx_window_texture_viewer::set_server(vsx_widget* new_server)
{
  server = new_server;
  engine = & ((vsx_widget_server*)new_server)->engine;
}

void vsx_window_texture_viewer::toggle_run()
{
  run =! run;
}

void vsx_window_texture_viewer::toggle_fullwindow()
{
  fullwindow =! fullwindow;
}

bool vsx_window_texture_viewer::get_fullwindow()
{
  return fullwindow;
}

bool* vsx_window_texture_viewer::get_fullwindow_ptr()
{
  return &fullwindow;
}


vsx_window_texture_viewer::vsx_window_texture_viewer()
{
  fps = 0.0;
  engine = 0x0;
  frame_count = 0;
  frame_delta = 0;
  init_run = false;
  run = true;
  fullwindow = false;
  modestring = "";
}
