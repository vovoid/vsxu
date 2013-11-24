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
#include "vsx_font.h"
// local includes
#include "vsx_engine.h"


#include "vsx_widget_base.h"
#include "dialogs/vsx_widget_window_statics.h"
#include <vsx_command_client_server.h>
#include "server/vsx_widget_server.h"

#include "application.h"

#include "vsx_widget_preview.h"

void vsx_window_texture_viewer::draw_2d()
{
	visible = !performance_mode;
  if (performance_mode)
    return;

    frame_delta += dtime;
    ++frame_count;
    if (frame_count == 50) {
      fps = 1/(frame_delta/frame_count);
      frame_count = 0;
      frame_delta = 0;
    }
    title = "vsxu preview (slow) @ "+i2s((int)round(fps))+" fps, Ctrl+F(ullscreen)";
    float vis = visible;
    if (!visible) color.a = 0.3; else color.a = 1;
    visible = 1;
    vsx_widget_window::draw_2d();
    visible = vis;
    pos_.y = pos.y+dragborder;
    size_.y = size.y-font_size-dragborder;
    pos_.x = pos.x+dragborder;
    size_.x = size.x-dragborder*2;

      if (visible)
      glColor4f(0,0,0,1);
      else
      glColor4f(0,0,0,0.3);

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      if (*engine == 0) {
      	set_pos(vsx_vector(0.0,2.0,0.0));
      	set_size(vsx_vector(0.0,0.0,0.0));
      }
      // set new viewport
      if (run)
      if (*engine)
      {
        // get viewport
        GLint	viewport[4];
        gl_state->viewport_get(viewport);
        gl_state->viewport_set(
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
        gl_state->viewport_set(viewport[0],viewport[1],viewport[2],viewport[3]);

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

bool vsx_window_texture_viewer::event_key_down(signed long key, bool alt, bool ctrl, bool shift)
{
  VSX_UNUSED(alt);
  VSX_UNUSED(shift);

  if (ctrl && key == 32) { run = !run; return false;}
  if (key == 32) { run = !run; return false;}
  if (key == 13) {fullwindow = !fullwindow; if (fullwindow) mouse.hide_cursor(); else mouse.show_cursor();}
  return false;
}

void vsx_window_texture_viewer::command_process_back_queue(vsx_command_s *t) {
//  printf("command: %s\n",t->cmd.c_str());
  if (t->cmd == "fullwindow") fullwindow = !fullwindow; else
  if (t->cmd == "toggle") run = !run; else
  vsx_widget::command_process_back_queue(t);

//  printf("run %i\n",run);
}

void vsx_window_texture_viewer::init() {
  if (init_run) return;
/*  menu = add(new vsx_widget_popup_menu,".obtx_menu");
  menu->commands.adds(VSX_COMMAND_MENU, "disable/enable {space or ctrl+space globally}", "toggle","");
  menu->commands.adds(VSX_COMMAND_MENU, "toggle fullwindow {enter or ctrl+f globally}", "fullwindow","");
  menu->commands.adds(VSX_COMMAND_MENU, "toggle fullscreen", "fullscreen","");
  modestring_commands.reset();
  vsx_command_s* c;
  menu->commands.adds(VSX_COMMAND_MENU, "fullscreen (current resolution) {alt+enter}", "fullscreen","");

  while (c = modestring_commands.get()) {
    menu->commands.adds(VSX_COMMAND_MENU, "fullscreen "+c->cmd_data, "fullscreen",c->cmd_data);
  }
  //menu->commands.adds(VSX_COMMAND_MENU, "fullscreen 800x600:32@60 {alt+1}", "fullscreen","800x600:32@60");
  //menu->commands.adds(VSX_COMMAND_MENU, "fullscreen 800x600:32@85 {alt+enter}", "fullscreen","800x600:32@85");
  //menu->commands.adds(VSX_COMMAND_MENU, "fullscreen 1024x768:32@60 {alt+2}", "fullscreen","1024x768:32@60");
  //menu->commands.adds(VSX_COMMAND_MENU, "fullscreen 1024x768:32@85", "fullscreen","1024x768:32@85");
  //menu->commands.adds(VSX_COMMAND_MENU, "fullscreen 1280x800:32@60", "fullscreen","1280x800:32@60");
  //menu->commands.adds(VSX_COMMAND_MENU, "fullscreen 1280x800:32@85", "fullscreen","1280x800:32@85");
  //menu->commands.adds(VSX_COMMAND_MENU, "fullscreen 1280x1024:32@85", "fullscreen","1280x1024:32@85");
  menu->commands.adds(VSX_COMMAND_MENU, "custom modestring", "custom","");
  menu->size.x = 0.35;
  menu->size.y = 0.5;*/
  init_children();
  /*pos_.y = pos.y-font_size+dragborder*2;
  size_.y = size.y-font_size+dragborder*2;
  pos_.x = pos.x+dragborder;
  size_.x = size.x-dragborder*2;*/

  if (!init_run) {
  	vsx_widget_window::init();
  }
  topmost = true;
//  texture = 0;
  title = "vsxu output preview";
  allow_resize_x = true;
  allow_resize_y = true;
  //dragborder = 0.001;
  //pos.y = 1;
  //pos.x = 0;
  //size.x = 0.3;
  //size.y = 0.3;
  //target_size = size;
  //target_pos = pos;
  visible = 1;
  size_min.x = 0.2;
  size_min.y = 0.15;

  init_run = true;
}

void vsx_window_texture_viewer::set_server(vsx_widget* new_server)
{
  server = new_server;
  engine = & ((vsx_widget_server*)new_server)->engine;
}

vsx_window_texture_viewer::vsx_window_texture_viewer() {
  // vsxu gui production control mechanisms
  fps = 0.0;
  engine = 0;
  frame_count = 0;
  frame_delta = 0;
  init_run = false;
  run = true;
  fullscreen = false;
  fullwindow = false;
  modestring = "";
}
