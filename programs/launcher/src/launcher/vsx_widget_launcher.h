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
#include <widgets/vsx_widget_checkbox.h>
#include <vsx_application_display.h>
#include <vsx_argvector.h>

// engine_graphics
#include <gl_helper.h>
#include <vsx_vbo_bucket.h>

using std::unique_ptr;

class vsx_widget_launcher : public vsx_widget
{
public:

  vsx_widget_label* label = 0x0;

  vsx_widget_label* application_label = 0x0;
  vsx_widget_dropbox* application_selection = 0x0;
  vsx_widget_checkbox* fullscreen = 0x0;

  vsx_widget_label* display_label = 0x0;
  vsx_widget_dropbox* display_selection = 0x0;


  vsx_widget_label* resolution_label = 0x0;
  vsx_widget_base_edit* resolution_x_edit = 0x0;
  vsx_widget_label* resolution_label_mid = 0x0;
  vsx_widget_base_edit* resolution_y_edit = 0x0;

  vsx_widget_label* position_label = 0x0;
  vsx_widget_base_edit* position_x_edit = 0x0;
  vsx_widget_label* position_label_mid = 0x0;
  vsx_widget_base_edit* position_y_edit = 0x0;

  vsx_widget_checkbox* borderless = 0x0;

  vsx_widget_button* launch_button = 0x0;

  void init()
  {
    support_interpolation = true;
    allow_resize_x = true;
    allow_resize_y = true;
    set_size(vsx_vector3<>(2.28f, 1.2f));
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

    // title
    label = dynamic_cast<vsx_widget_label*>( add(new vsx_widget_label(), "label") );
    label->set_pos( vsx_vector3f( 0.0, 0.55 ) );
    label->set_size( vsx_vector3f( 2.28, 0.1) );
    label->set_font_size( 0.08f );
    label->title = "VSXu Launcher";
    label->halign = a_center;
    label->set_render_type(vsx_widget_render_type::render_3d);


    // application
    float application_selection_pos_x = 0.0f;
    float application_selection_pos_y = 0.4f;
    application_label = dynamic_cast<vsx_widget_label*>( add(new vsx_widget_label(), "label") );
    application_label->set_pos( vsx_vector3f( application_selection_pos_x -0.4 - 0.25, application_selection_pos_y ) );
    application_label->set_size( vsx_vector3f( 0.5, 0.1) );
    application_label->set_font_size( 0.06f );
    application_label->title = "Select Application:";
    application_label->halign = a_left;
    application_label->set_render_type(vsx_widget_render_type::render_3d);

    application_selection = dynamic_cast<vsx_widget_dropbox*>( add(new vsx_widget_dropbox("Select Application"), "application_selection") );
    application_selection->set_pos( vsx_vector3f( application_selection_pos_x, application_selection_pos_y ) );
    application_selection->set_size( vsx_vector3f(0.8, 0.1) );
    application_selection->set_font_size( 0.08f );
    application_selection->set_render_type(vsx_widget_render_type::render_3d);
    application_selection->init();

    application_selection->add_option( 0, "VSXu Player" );
    application_selection->add_option( 1, "VSXu Artiste" );
    application_selection->add_option( 2, "VSXu Profiler" );

    // display selection
    float display_selection_pos_x = 0.0f;
    float display_selection_pos_y = 0.3f;
    display_label = dynamic_cast<vsx_widget_label*>( add(new vsx_widget_label(), "label") );
    display_label->set_pos( vsx_vector3f( display_selection_pos_x -0.4 - 0.25, display_selection_pos_y ) );
    display_label->set_size( vsx_vector3f( 0.5, 0.1) );
    display_label->set_font_size( 0.06f );
    display_label->title = "Select display:";
    display_label->halign = a_left;
    display_label->set_render_type(vsx_widget_render_type::render_3d);

    display_selection = dynamic_cast<vsx_widget_dropbox*>( add(new vsx_widget_dropbox("Select display"), "display_selection") );
    display_selection->set_pos( vsx_vector3f( display_selection_pos_x, display_selection_pos_y ) );
    display_selection->set_size( vsx_vector3f(0.8, 0.1) );
    display_selection->set_font_size( 0.08f );
    display_selection->set_render_type(vsx_widget_render_type::render_3d);
    display_selection->init();
    display_selection->on_selection =
      [this](size_t value, vsx_string<> title)
      {
        VSX_UNUSED(title);
        std::pair<int, int> new_x_y = vsx_application_display::get()->get_local_pos(value, vsx_string_helper::s2i(resolution_x_edit->get_string()), vsx_string_helper::s2i(resolution_y_edit->get_string()));
        position_x_edit->set_string( vsx_string_helper::i2s( new_x_y.first ) );
        position_y_edit->set_string( vsx_string_helper::i2s( new_x_y.second ) );
      };

    display_selection->add_option( 0, "Display 1 (primary)" );

    if (vsx_application_display::get()->displays.size() > 1)
    {
      for_n(i, 1, vsx_application_display::get()->displays.size())
        display_selection->add_option(i, "Display " + vsx_string_helper::i2s(i));
    }


    // fullscreen
    fullscreen = dynamic_cast<vsx_widget_checkbox*>( add(new vsx_widget_checkbox("Fullscreen"), "fullscreen") );
    fullscreen->set_pos( vsx_vector3f( 0.0, 0.2f ) );
    fullscreen->set_size( vsx_vector3f(0.8, 0.1) );
    fullscreen->set_font_size( 0.08f );
    fullscreen->set_render_type(vsx_widget_render_type::render_3d);
    fullscreen->init();


    // window resolution
    float resolution_selection_pos_x = 0.0f;
    float resolution_selection_pos_y = 0.1;
    resolution_label = dynamic_cast<vsx_widget_label*>( add(new vsx_widget_label(), "label") );
    resolution_label->set_pos( vsx_vector3f( resolution_selection_pos_x -0.4 - 0.25, resolution_selection_pos_y ) );
    resolution_label->set_size( vsx_vector3f( 0.5, 0.1) );
    resolution_label->set_font_size( 0.06f );
    resolution_label->title = "Window size:";
    resolution_label->halign = a_left;
    resolution_label->set_render_type(vsx_widget_render_type::render_3d);

    resolution_x_edit = dynamic_cast<vsx_widget_base_edit*>( add(new vsx_widget_base_edit(), "editor") );
    resolution_x_edit->set_pos( vsx_vector3f( resolution_selection_pos_x - 0.4f + 0.15, resolution_selection_pos_y ) );
    resolution_x_edit->set_size( vsx_vector3f(0.3, 0.1) );
    resolution_x_edit->set_font_size( 0.08f );
    resolution_x_edit->set_string("1280");
    resolution_x_edit->set_render_type(vsx_widget_render_type::render_3d);
    resolution_x_edit->allowed_chars = "0123456789";

    resolution_y_edit = dynamic_cast<vsx_widget_base_edit*>( add(new vsx_widget_base_edit(), "editor") );
    resolution_y_edit->set_pos( vsx_vector3f( resolution_selection_pos_x - 0.4f + 0.15 + 0.45f, resolution_selection_pos_y ) );
    resolution_y_edit->set_size( vsx_vector3f(0.3, 0.1) );
    resolution_y_edit->set_font_size( 0.08f );
    resolution_y_edit->set_string("720");
    resolution_y_edit->set_render_type(vsx_widget_render_type::render_3d);

    resolution_label_mid = dynamic_cast<vsx_widget_label*>( add(new vsx_widget_label(), "label") );
    resolution_label_mid->set_pos( vsx_vector3f( resolution_selection_pos_x - 0.4f + 0.15 + 0.25f, resolution_selection_pos_y ) );
    resolution_label_mid->set_size( vsx_vector3f( 0.1, 0.1) );
    resolution_label_mid->set_font_size( 0.06f );
    resolution_label_mid->title = "by";
    resolution_label_mid->halign = a_left;
    resolution_label_mid->set_render_type(vsx_widget_render_type::render_3d);

    // window position
    float position_selection_pos_x = 0.0f;
    float position_selection_pos_y = 0.0;
    position_label = dynamic_cast<vsx_widget_label*>( add(new vsx_widget_label(), "label") );
    position_label->set_pos( vsx_vector3f( position_selection_pos_x -0.4 - 0.25, position_selection_pos_y ) );
    position_label->set_size( vsx_vector3f( 0.5, 0.1) );
    position_label->set_font_size( 0.06f );
    position_label->title = "Window position:";
    position_label->halign = a_left;
    position_label->set_render_type(vsx_widget_render_type::render_3d);

    position_x_edit = dynamic_cast<vsx_widget_base_edit*>( add(new vsx_widget_base_edit(), "editor") );
    position_x_edit->set_pos( vsx_vector3f( position_selection_pos_x - 0.4f + 0.15, position_selection_pos_y ) );
    position_x_edit->set_size( vsx_vector3f(0.3, 0.1) );
    position_x_edit->set_font_size( 0.08f );
    position_x_edit->set_string("1280");
    position_x_edit->set_render_type(vsx_widget_render_type::render_3d);

    position_y_edit = dynamic_cast<vsx_widget_base_edit*>( add(new vsx_widget_base_edit(), "editor") );
    position_y_edit->set_pos( vsx_vector3f( position_selection_pos_x - 0.4f + 0.15 + 0.45f, position_selection_pos_y ) );
    position_y_edit->set_size( vsx_vector3f(0.3, 0.1) );
    position_y_edit->set_font_size( 0.08f );
    position_y_edit->set_string("720");
    position_y_edit->set_render_type(vsx_widget_render_type::render_3d);

    position_label_mid = dynamic_cast<vsx_widget_label*>( add(new vsx_widget_label(), "label") );
    position_label_mid->set_pos( vsx_vector3f( position_selection_pos_x - 0.4f + 0.15 + 0.25f, position_selection_pos_y ) );
    position_label_mid->set_size( vsx_vector3f( 0.1, 0.1) );
    position_label_mid->set_font_size( 0.06f );
    position_label_mid->title = ",";
    position_label_mid->halign = a_left;
    position_label_mid->set_render_type(vsx_widget_render_type::render_3d);


    // borderless
    borderless = dynamic_cast<vsx_widget_checkbox*>( add(new vsx_widget_checkbox("Borderless window"), "borderless") );
    borderless->set_pos( vsx_vector3f( 0.0, -0.1f ) );
    borderless->set_size( vsx_vector3f(0.8, 0.1) );
    borderless->set_font_size( 0.08f );
    borderless->set_render_type(vsx_widget_render_type::render_3d);
    borderless->init();

    // launch button
    launch_button = dynamic_cast<vsx_widget_button*>( add(new vsx_widget_button(), "launch_button") );
    launch_button->set_pos( vsx_vector3f( 0.0, -0.4f ) );
    launch_button->set_size( vsx_vector3f(1.5, 0.1) );
    launch_button->title = "Launch!";
    launch_button->set_font_size( 0.08f );
    launch_button->set_render_type(vsx_widget_render_type::render_3d);
    launch_button->on_click = [this](){
      launch();
    };

    // make sure interpolation is called
    this->interpolate_size();
    init_run = true;
  }

  void launch()
  {
    vsx_string<> command = vsx_argvector::get_instance()->get_executable_directory();
    if (application_selection->selected == 0)
      command += DIRECTORY_SEPARATOR  "vsxu_player";
    if (application_selection->selected == 1)
      command += DIRECTORY_SEPARATOR  "vsxu_artiste";
    if (application_selection->selected == 2)
      command += DIRECTORY_SEPARATOR  "vsxu_profiler";

    command += " ";
    command += "-d " + vsx_string_helper::i2s(display_selection->selected + 1) + " ";

    if (!fullscreen->checked)
    {
      command += "-p " + position_x_edit->get_string() + "," + position_y_edit->get_string() + " ";
      command += "-s " + resolution_x_edit->get_string() + "x" + resolution_y_edit->get_string();
      if (borderless->checked)
        command += " -bl";
    }
    command += " &";
    system(command.c_str());
    sleep(2);
    vsx_printf(L"command: %s\n", command.c_str());
    vsx_application_control::get_instance()->shutdown_request();
  }

  void i_draw()
  {
    resolution_label->visible = fullscreen->checked?0.0f:1.0f;
    resolution_label_mid->visible = fullscreen->checked?0.0f:1.0f;
    resolution_x_edit->visible = fullscreen->checked?0.0f:1.0f;
    resolution_y_edit->visible = fullscreen->checked?0.0f:1.0f;
    position_label->visible = fullscreen->checked?0.0f:1.0f;
    position_label_mid->visible = fullscreen->checked?0.0f:1.0f;
    position_x_edit->visible = fullscreen->checked?0.0f:1.0f;
    position_y_edit->visible = fullscreen->checked?0.0f:1.0f;
    borderless->visible = fullscreen->checked?0.0f:1.0f;

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

