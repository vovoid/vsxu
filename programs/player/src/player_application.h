/**
* Project: VSXu: Realtime modular visual programming language, music/audio visualizer.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Vovoid Media Technologies AB Copyright (C) 2003-2013
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

#include "vsx_application.h"
#include <vsx_application_control.h>
#include <audiovisual/vsx_state_manager.h>
#include "player_overlay.h"
#include <vsx_application_input_state_manager.h>
#include <audiovisual/vsx_state_fx_save.h>
#include <perf/vsx_perf.h>

class player_application
    : public vsx_application
{
  vsx_overlay* overlay = 0x0;
  bool no_overlay = false;

public:

  size_t window_title_i = 0;
  void update_window_title()
  {
    req(!(window_title_i++ % 60));
    vsx_perf perf;
    char titlestr[ 200 ];
    sprintf( titlestr, "Vovoid VSXu Player %s [%s %d-bit] [%d MB RAM used] %s", VSXU_VER, PLATFORM_NAME, PLATFORM_BITS, perf.memory_currently_used(), VSXU_VERSION_COPYRIGHT);
    window_title = vsx_string<>(titlestr);
    vsx_application_control::get_instance()->window_title = window_title;
  }

  player_application()
  {
    update_window_title();
    vsx_application_control::get_instance()->create_preferences_path_request();
  }

  void print_help()
  {
    vsx_application::print_help();
    vsx_printf(
      L"    -pl                       Preload all visuals on start \n"
       "    -dr                       Disable randomizer     \n"
       "    -rs                       Sequential visual progression\n"
    );
  }

  void init()
  {
    no_overlay = vsx_argvector::get_instance()->has_param("no");

    vsx_module_list_manager::get()->module_list = vsx_module_list_factory_create();
    vsx::engine::audiovisual::state_manager::create();

    // create a new manager
    vsx::engine::audiovisual::state_manager::get()->option_preload_all = vsx_argvector::get_instance()->has_param("pl");

    // init manager with the shared path and sound input type.
    vsx::engine::audiovisual::state_manager::get()->load( (PLATFORM_SHARED_FILES).c_str());

    // create a new text overlay
    overlay = new vsx_overlay;

    if (vsx_argvector::get_instance()->has_param("dr"))
      vsx::engine::audiovisual::state_manager::get()->set_randomizer(false);

    if (vsx_argvector::get_instance()->has_param("rs"))
    {
      vsx::engine::audiovisual::state_manager::get()->set_randomizer(false);
      vsx::engine::audiovisual::state_manager::get()->set_sequential(true);
    }

  }

  bool fx_levels_loaded = false;
  void draw()
  {
    update_window_title();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    vsx::engine::audiovisual::state_manager::get()->render();

    if (overlay && !no_overlay)
      overlay->render();

    if (!fx_levels_loaded && vsx_application_control::get_instance()->preferences_path.size())
    {
      vsx::engine::audiovisual::fx_load(
          vsx::engine::audiovisual::state_manager::get()->states_get(),
          vsx_application_control::get_instance()->preferences_path + "fx_levels.json"
        );
      fx_levels_loaded = true;
    }

    if (vsx::engine::audiovisual::state_manager::get()->system_message.size())
    {
      vsx_application_control::get_instance()->message_box_title = "Error";
      vsx_application_control::get_instance()->message_box_message = vsx::engine::audiovisual::state_manager::get()->system_message;
    }
  }

  void event_key_down(long key)
  {
    switch (key)
    {
      case VSX_SCANCODE_ESCAPE:
        vsx_application_control::get_instance()->shutdown_request();
      case VSX_SCANCODE_PAGEUP:
        vsx::engine::audiovisual::state_manager::get()->speed_inc();
        break;
      case VSX_SCANCODE_PAGEDOWN:
        vsx::engine::audiovisual::state_manager::get()->speed_dec();
        break;
      case VSX_SCANCODE_UP:
        vsx::engine::audiovisual::state_manager::get()->fx_level_inc();
        overlay->show_fx_graph();
        break;
      case VSX_SCANCODE_DOWN:
        vsx::engine::audiovisual::state_manager::get()->fx_level_dec();
        overlay->show_fx_graph();
        break;
      case VSX_SCANCODE_LEFT:
        vsx::engine::audiovisual::state_manager::get()->select_prev_state();
        break;
      case VSX_SCANCODE_RIGHT:
        vsx::engine::audiovisual::state_manager::get()->select_next_state();
        break;
      case VSX_SCANCODE_F1:
        overlay->set_help(1);
        break;
      case VSX_SCANCODE_F:
        overlay->set_help(2);
        break;
      case VSX_SCANCODE_R:
        if (vsx_input_keyboard.pressed_ctrl())
          vsx::engine::audiovisual::state_manager::get()->select_random_state();
        else
        {
          vsx::engine::audiovisual::state_manager::get()->toggle_randomizer();
          overlay->show_randomizer_status();
        }
        break;
    }
  }

  void uninit()
  {
    vsx::engine::audiovisual::fx_save(
        vsx::engine::audiovisual::state_manager::get()->states_get(),
        vsx_application_control::get_instance()->preferences_path + "fx_levels.json"
      );

    vsx::engine::audiovisual::state_manager::destroy();
    vsx_module_list_factory_destroy(vsx_module_list_manager::get()->module_list);
  }

};

