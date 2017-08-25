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
#include <tools/vsx_process.h>
#include <container/vsx_nw_vector.h>

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

#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
  vsx_widget_checkbox* console = 0x0;
#endif

  vsx_widget_button* launch_button = 0x0;

  void set_window_position_to_center(size_t value)
  {
    std::pair<int, int> new_x_y = vsx_application_display::get()->get_local_pos(value, vsx_string_helper::s2i(resolution_x_edit->get_string()), vsx_string_helper::s2i(resolution_y_edit->get_string()));
    position_x_edit->set_string( vsx_string_helper::i2s( new_x_y.first ) );
    position_y_edit->set_string( vsx_string_helper::i2s( new_x_y.second ) );
  }

  void init()
  {
    support_interpolation = true;
    allow_resize_x = true;
    allow_resize_y = true;
    set_size(vsx_vector3<>(2.28f, 1.2f));
    size_min.x = 0.2f;
    size_min.y = 0.2f;

    title = "Launch VSXu";

    allow_move_x = false;
    allow_move_y = false;

    set_pos( camera.get_pos_2d() );
    camera.set_distance(2.9);

    // Menu
    menu = add(new vsx_widget_popup_menu, ".comp_menu");
    menu->commands.adds(VSX_COMMAND_MENU, "close", "menu_close", "");
    menu->size.x = size.x * 0.1f;
    menu->init();
    menu->set_render_type(render_2d);

    // title
    label = dynamic_cast<vsx_widget_label*>( add(new vsx_widget_label(), "label") );
    label->set_pos( vsx_vector3f( 0.0f, 0.55f ) );
    label->set_size( vsx_vector3f( 2.28f, 0.1f) );
    label->set_font_size( 0.08f );
    label->title = "VSXu Launcher";
    label->halign = a_center;
    label->set_render_type(vsx_widget_render_type::render_3d);

    float gui_base_pos_x = 0.25f;

    // application
    float application_selection_pos_x = gui_base_pos_x;
    float application_selection_pos_y = 0.4f;
    application_label = dynamic_cast<vsx_widget_label*>( add(new vsx_widget_label(), "label") );
    application_label->set_pos( vsx_vector3f( application_selection_pos_x -0.4f - 0.25f, application_selection_pos_y ) );
    application_label->set_size( vsx_vector3f( 0.5f, 0.1f) );
    application_label->set_font_size( 0.06f );
    application_label->title = "Select Application:";
    application_label->halign = a_left;
    application_label->set_render_type(vsx_widget_render_type::render_3d);

    application_selection = dynamic_cast<vsx_widget_dropbox*>( add(new vsx_widget_dropbox("Select Application"), "application_selection") );
    application_selection->set_pos( vsx_vector3f( application_selection_pos_x, application_selection_pos_y ) );
    application_selection->set_size( vsx_vector3f(0.8f, 0.1f) );
    application_selection->set_font_size( 0.08f );
    application_selection->set_render_type(vsx_widget_render_type::render_3d);
    application_selection->init();

    application_selection->add_option( 0, "VSXu Player" );
    application_selection->add_option( 1, "VSXu Artiste" );
    application_selection->add_option( 2, "VSXu Profiler" );
    #if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
      application_selection->add_option( 3, "VSXu Server" );
    #endif

    // display selection
    float display_selection_pos_x = gui_base_pos_x;
    float display_selection_pos_y = 0.3f;
    display_label = dynamic_cast<vsx_widget_label*>( add(new vsx_widget_label(), "label") );
    display_label->set_pos( vsx_vector3f( display_selection_pos_x -0.4f - 0.25f, display_selection_pos_y ) );
    display_label->set_size( vsx_vector3f( 0.5f, 0.1f) );
    display_label->set_font_size( 0.06f );
    display_label->title = "Select display:";
    display_label->halign = a_left;
    display_label->set_render_type(vsx_widget_render_type::render_3d);

    display_selection = dynamic_cast<vsx_widget_dropbox*>( add(new vsx_widget_dropbox("Select display"), "display_selection") );
    display_selection->set_pos( vsx_vector3f( display_selection_pos_x, display_selection_pos_y ) );
    display_selection->set_size( vsx_vector3f(0.8f, 0.1f) );
    display_selection->set_font_size( 0.08f );
    display_selection->set_render_type(vsx_widget_render_type::render_3d);
    display_selection->init();
    display_selection->on_selection =
      [this](size_t value, vsx_string<> title)
      {
        VSX_UNUSED(title);
        set_window_position_to_center(value);
      };

    display_selection->add_option( 0, "Display 1 (primary)" );

    if (vsx_application_display::get()->displays.size() > 1)
    {
      for_n(i, 1, vsx_application_display::get()->displays.size())
        display_selection->add_option((int)i, "Display " + vsx_string_helper::i2s((int)i + 1));
    }


    // fullscreen
    fullscreen = dynamic_cast<vsx_widget_checkbox*>( add(new vsx_widget_checkbox("Fullscreen"), "fullscreen") );
    fullscreen->set_pos( vsx_vector3f( gui_base_pos_x, 0.2f ) );
    fullscreen->set_size( vsx_vector3f(0.8f, 0.1f) );
    fullscreen->set_font_size( 0.08f );
    fullscreen->set_render_type(vsx_widget_render_type::render_3d);
    fullscreen->init();



    // window resolution
    float resolution_selection_pos_x = gui_base_pos_x;
    float resolution_selection_pos_y = 0.1f;
    resolution_label = dynamic_cast<vsx_widget_label*>( add(new vsx_widget_label(), "label") );
    resolution_label->set_pos( vsx_vector3f( resolution_selection_pos_x -0.4f - 0.25f, resolution_selection_pos_y ) );
    resolution_label->set_size( vsx_vector3f( 0.5f, 0.1f) );
    resolution_label->set_font_size( 0.06f );
    resolution_label->title = "Window size:";
    resolution_label->halign = a_left;
    resolution_label->set_render_type(vsx_widget_render_type::render_3d);

    resolution_x_edit = dynamic_cast<vsx_widget_base_edit*>( add(new vsx_widget_base_edit(), "editor") );
    resolution_x_edit->set_pos( vsx_vector3f( resolution_selection_pos_x - 0.4f + 0.15f, resolution_selection_pos_y ) );
    resolution_x_edit->set_size( vsx_vector3f(0.3f, 0.1f) );
    resolution_x_edit->set_font_size( 0.08f );
    resolution_x_edit->set_string("1280");
    resolution_x_edit->set_render_type(vsx_widget_render_type::render_3d);
    resolution_x_edit->allowed_chars = "0123456789";

    resolution_y_edit = dynamic_cast<vsx_widget_base_edit*>( add(new vsx_widget_base_edit(), "editor") );
    resolution_y_edit->set_pos( vsx_vector3f( resolution_selection_pos_x - 0.4f + 0.15f + 0.45f, resolution_selection_pos_y ) );
    resolution_y_edit->set_size( vsx_vector3f(0.3f, 0.1f) );
    resolution_y_edit->set_font_size( 0.08f );
    resolution_y_edit->set_string("720");
    resolution_y_edit->set_render_type(vsx_widget_render_type::render_3d);

    resolution_label_mid = dynamic_cast<vsx_widget_label*>( add(new vsx_widget_label(), "label") );
    resolution_label_mid->set_pos( vsx_vector3f( resolution_selection_pos_x - 0.4f + 0.15f + 0.25f, resolution_selection_pos_y ) );
    resolution_label_mid->set_size( vsx_vector3f( 0.1f, 0.1f) );
    resolution_label_mid->set_font_size( 0.06f );
    resolution_label_mid->title = "by";
    resolution_label_mid->halign = a_left;
    resolution_label_mid->set_render_type(vsx_widget_render_type::render_3d);

    // window position
    float position_selection_pos_x = gui_base_pos_x;
    float position_selection_pos_y = 0.0;
    position_label = dynamic_cast<vsx_widget_label*>( add(new vsx_widget_label(), "label") );
    position_label->set_pos( vsx_vector3f( position_selection_pos_x -0.4f - 0.25f, position_selection_pos_y ) );
    position_label->set_size( vsx_vector3f( 0.5f, 0.1f) );
    position_label->set_font_size( 0.06f );
    position_label->title = "Window position:";
    position_label->halign = a_left;
    position_label->set_render_type(vsx_widget_render_type::render_3d);

    position_x_edit = dynamic_cast<vsx_widget_base_edit*>( add(new vsx_widget_base_edit(), "editor") );
    position_x_edit->set_pos( vsx_vector3f( position_selection_pos_x - 0.4f + 0.15f, position_selection_pos_y ) );
    position_x_edit->set_size( vsx_vector3f(0.3f, 0.1f) );
    position_x_edit->set_font_size( 0.08f );
    position_x_edit->set_string("1280");
    position_x_edit->set_render_type(vsx_widget_render_type::render_3d);

    position_y_edit = dynamic_cast<vsx_widget_base_edit*>( add(new vsx_widget_base_edit(), "editor") );
    position_y_edit->set_pos( vsx_vector3f( position_selection_pos_x - 0.4f + 0.15f + 0.45f, position_selection_pos_y ) );
    position_y_edit->set_size( vsx_vector3f(0.3f, 0.1f) );
    position_y_edit->set_font_size( 0.08f );
    position_y_edit->set_string("720");
    position_y_edit->set_render_type(vsx_widget_render_type::render_3d);

    position_label_mid = dynamic_cast<vsx_widget_label*>( add(new vsx_widget_label(), "label") );
    position_label_mid->set_pos( vsx_vector3f( position_selection_pos_x - 0.4f + 0.15f + 0.25f, position_selection_pos_y ) );
    position_label_mid->set_size( vsx_vector3f( 0.1f, 0.1f) );
    position_label_mid->set_font_size( 0.06f );
    position_label_mid->title = ",";
    position_label_mid->halign = a_left;
    position_label_mid->set_render_type(vsx_widget_render_type::render_3d);


    // borderless
    borderless = dynamic_cast<vsx_widget_checkbox*>( add(new vsx_widget_checkbox("Borderless window"), "borderless") );
    borderless->set_pos( vsx_vector3f( gui_base_pos_x, -0.1f ) );
    borderless->set_size( vsx_vector3f(0.8f, 0.1f) );
    borderless->set_font_size( 0.08f );
    borderless->set_render_type(vsx_widget_render_type::render_3d);
    borderless->init();

    #if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
    // console
    console = dynamic_cast<vsx_widget_checkbox*>( add(new vsx_widget_checkbox("Enable console window"), "console") );
    console->set_pos( vsx_vector3f( gui_base_pos_x, -0.2f ) );
    console->set_size( vsx_vector3f(0.8f, 0.1f) );
    console->set_font_size( 0.08f );
    console->set_render_type(vsx_widget_render_type::render_3d);
    console->init();
    #endif


    // launch button
    launch_button = dynamic_cast<vsx_widget_button*>( add(new vsx_widget_button(), "launch_button") );
    launch_button->set_pos( vsx_vector3f( 0.0f, -0.4f ) );
    launch_button->set_size( vsx_vector3f(1.5f, 0.1f) );
    launch_button->title = "Launch!";
    launch_button->set_font_size( 0.08f );
    launch_button->set_render_type(vsx_widget_render_type::render_3d);
    launch_button->on_click = [this](){
      launch();
    };

    set_window_position_to_center(0);

    // make sure interpolation is called
    this->interpolate_size();
    init_run = true;
  }

  void launch()
  {
    vsx_string<> working_directory = vsx_argvector::get_instance()->get_executable_directory();
    vsx_string<> command = working_directory;
    vsx_nw_vector< vsx_string<> > args;

    if (application_selection->selected == 0)
      command += DIRECTORY_SEPARATOR  "vsxu_player";
    if (application_selection->selected == 1)
      command += DIRECTORY_SEPARATOR  "vsxu_artiste";
    if (application_selection->selected == 2)
      command += DIRECTORY_SEPARATOR  "vsxu_profiler";
    #if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
      if (application_selection->selected == 3)
        command += DIRECTORY_SEPARATOR  "vsxu_server";
    #endif

    args.push_back("-d");
    args.push_back(vsx_string_helper::i2s((int)display_selection->selected + 1));

    if (fullscreen->checked)
      args.push_back("-f");

    if (!fullscreen->checked)
    {
      args.push_back("-p");
      args.push_back(position_x_edit->get_string() + "," + position_y_edit->get_string());
      args.push_back("-s");
      args.push_back(resolution_x_edit->get_string() + "x" + resolution_y_edit->get_string());
      if (borderless->checked)
        args.push_back("-bl");
    }

    #if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
    if (console->checked)
    {
       args.push_back("-console");
       ShowWindow( GetConsoleWindow(), SW_SHOW );
       vsx_application_control::get_instance()->show_console_window_request();
    }
    #endif
    vsx_process::exec( command, args, working_directory );
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

