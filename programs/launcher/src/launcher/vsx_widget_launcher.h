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

#pragma once

// widget
#include <vsx_widget.h>
#include <widgets/vsx_widget_popup_menu.h>
#include <widgets/vsx_widget_editor.h>
#include <widgets/vsx_widget_base_edit.h>
#include <widgets/vsx_widget_label.h>
#include <widgets/vsx_widget_dropbox.h>

// engine_graphics
#include <gl_helper.h>
#include <vsx_vbo_bucket.h>

using std::unique_ptr;

class vsx_widget_launcher : public vsx_widget
{
public:

  vsx_widget_base_edit* editor = 0x0;
  vsx_widget_label* label = 0x0;
  vsx_widget_dropbox* application_selection;


  void init()
  {
    support_interpolation = true;
    allow_resize_x = true;
    allow_resize_y = true;
    set_size(vsx_vector3<>(1.6f,0.9f));
    size_min.x = 0.2;
    size_min.y = 0.2;

    title = "Launch VSXu";

    allow_move_x = false;
    allow_move_y = false;

    set_pos( camera.get_pos_2d() );
    camera.set_distance(2.9);

    // Menu
    menu = add(new vsx_widget_popup_menu, ".comp_menu");
    menu->commands.adds(VSX_COMMAND_MENU, "close", "menu_close", "");
    menu->size.x = size.x * 0.1;
    menu->init();
    menu->set_render_type(render_2d);

    editor = dynamic_cast<vsx_widget_base_edit*>( add(new vsx_widget_base_edit(), "editor") );
    editor->set_pos( vsx_vector3f( -0.4, 0.0 ) );
    editor->set_size( vsx_vector3f(0.1, 0.1) );
    editor->set_font_size( 0.08f );
    editor->set_string("hej");
    editor->set_render_type(vsx_widget_render_type::render_3d);

    label = dynamic_cast<vsx_widget_label*>( add(new vsx_widget_label(), "label") );
    label->set_pos( vsx_vector3f( -0.4, 0.0 ) );
    label->set_size( vsx_vector3f(0.1, 0.1) );
    label->set_font_size( 0.08f );
    label->title = "hej";
    label->set_render_type(vsx_widget_render_type::render_3d);

    application_selection = dynamic_cast<vsx_widget_dropbox*>( add(new vsx_widget_dropbox("Select Application"), "application_selection") );
    application_selection->set_pos( vsx_vector3f( 0.0, 0.45 - 0.05 ) );
    application_selection->set_size( vsx_vector3f(0.8, 0.1) );
    application_selection->set_font_size( 0.08f );
    application_selection->set_render_type(vsx_widget_render_type::render_3d);
    application_selection->init();

    application_selection->add_option( 0, "VSXu Artiste" );
    application_selection->add_option( 1, "VSXu Player" );
    application_selection->add_option( 2, "VSXu Profiler" );


    // make sure interpolation is called
    this->interpolate_size();
    init_run = true;
  }

  void i_draw()
  {
    vsx_vector3<> position = get_pos_p();
    glBegin(GL_QUADS);
      vsx_widget_skin::get_instance()->set_color_gl(1);
      glVertex3f(position.x-size.x*0.5f, position.y+size.y*0.5f,position.z);
      glVertex3f(position.x+size.x*0.5f, position.y+size.y*0.5f,position.z);
      glVertex3f(position.x+size.x*0.5f, position.y+-size.y*0.5f,position.z);
      glVertex3f(position.x-size.x*0.5f, position.y+-size.y*0.5f,position.z);
    glEnd();
    vsx_widget_skin::get_instance()->set_color_gl(0);
    draw_box_border(vsx_vector3<>(position.x-size.x*0.5,position.y-size.y*0.5f), vsx_vector3<>(size.x,size.y), dragborder);

    glColor4f(1,1,1,1);

    vsx_widget::i_draw();
  }

  void command_process_back_queue(vsx_command_s *t)
  {
    if (t->cmd == "menu_close")
    {
      _delete();
      return;
    }

    vsx_printf(L"t->cmd: %s\n", t->cmd_data.c_str());
  }

  bool event_key_down(uint16_t key)
  {
    VSX_UNUSED(key);
    return true;
  }

  void event_mouse_wheel(float y)
  {
  }

  void event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords)
  {
    VSX_UNUSED(distance);
  }

  void interpolate_size()
  {
    vsx_widget::interpolate_size();
  }

};

