/**
* Project: VSXu launcher - Data collection and data visualizer
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Vovoid Media Technologies AB Copyright (C) 2014
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

// Globals
#include <gl_helper.h>

// Local libs

// Generic Widget
#include "vsx_widget.h"
#include "widgets/vsx_widget_popup_menu.h"

// Local widgets
#include "vsx_widget_launcher.h"




void vsx_widget_launcher::init()
{
  support_interpolation = true;
  allow_resize_x = true;
  allow_resize_y = true;
  set_size(vsx_vector3<>(20.0f,20.0f));
  size_min.x = 0.2;
  size_min.y = 0.2;

  title = "select profile";

  allow_move_x = false;
  allow_move_y = false;

  target_pos = pos = camera.get_pos_2d() + vsx_vector3<>(0.25);
  camera.set_distance(2.9);

  // Menu
  menu = add(new vsx_widget_popup_menu, ".comp_menu");
  menu->commands.adds(VSX_COMMAND_MENU, "close", "menu_close", "");
  menu->size.x = size.x * 0.4;
  menu->init();

  // make sure interpolation is called
  this->interpolate_size();
  init_run = true;
}

void vsx_widget_launcher::update_list()
{

}

void vsx_widget_launcher::i_draw()
{
  vsx_vector3<> parentpos = get_pos_p();
  glBegin(GL_QUADS);
    vsx_widget_skin::get_instance()->set_color_gl(1);
    glVertex3f(parentpos.x-size.x*0.5f, parentpos.y+size.y*0.5f,pos.z);
    glVertex3f(parentpos.x+size.x*0.5f, parentpos.y+size.y*0.5f,pos.z);
    glVertex3f(parentpos.x+size.x*0.5f, parentpos.y+-size.y*0.5f,pos.z);
    glVertex3f(parentpos.x-size.x*0.5f, parentpos.y+-size.y*0.5f,pos.z);
  glEnd();
  vsx_widget_skin::get_instance()->set_color_gl(0);
  draw_box_border(vsx_vector3<>(parentpos.x-size.x*0.5,parentpos.y-size.y*0.5f), vsx_vector3<>(size.x,size.y), dragborder);

  glColor4f(1,1,1,1);

  vsx_widget::i_draw();
}


void vsx_widget_launcher::command_process_back_queue(vsx_command_s *t)
{
  if (t->cmd == "menu_close")
  {
    _delete();
    return;
  }

  vsx_printf(L"t->cmd: %s\n", t->cmd_data.c_str());
}

bool vsx_widget_launcher::event_key_down(uint16_t key)
{
  VSX_UNUSED(key);
  return true;
}

void vsx_widget_launcher::event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords)
{
  VSX_UNUSED(distance);
}

void vsx_widget_launcher::update_children()
{
}

void vsx_widget_launcher::event_mouse_wheel(float y)
{
  update_children();
}

void vsx_widget_launcher::interpolate_size()
{
  vsx_widget::interpolate_size();
}
