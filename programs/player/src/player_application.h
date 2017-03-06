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
#include <audiovisual/vsx_statelist.h>
#include "player_overlay.h"
#include <vsx_application_input_state_manager.h>

class player_application
    : public vsx_application
{
  vsx_statelist manager;
  vsx_overlay* overlay = 0x0;
  bool no_overlay = false;

public:

  vsx_string<> window_title_get()
  {
    char titlestr[ 200 ];
    sprintf( titlestr, "Vovoid VSXu Player %s [%s %d-bit]", VSXU_VER, PLATFORM_NAME, PLATFORM_BITS);
    return vsx_string<>(titlestr);
  }

  void print_help()
  {
    vsx_application::print_help();
    vsx_printf(
      L"  -pl        Preload all visuals on start \n"
       "  -dr        Disable randomizer     \n"
    );

  }

  void init()
  {
    printf("INFO: app_draw first\n");

    no_overlay = vsx_argvector::get_instance()->has_param("no");

    // create a new manager
    manager.set_option_preload_all(vsx_argvector::get_instance()->has_param("pl"));

    // init manager with the shared path and sound input type.
    manager.init( (PLATFORM_SHARED_FILES).c_str(), "");

    // create a new text overlay
    overlay = new vsx_overlay;
    overlay->set_manager(&manager);

    if (vsx_argvector::get_instance()->has_param("dr"))
      manager.set_randomizer(false);

    printf("INFO: app_draw first done\n");
  }

  void draw()
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    manager.render();

    if (overlay && !no_overlay)
      overlay->render();
  }

  void event_key_down(long key)
  {
    switch (key)
    {
      case VSX_SCANCODE_ESCAPE:
        exit(0);
      case VSX_SCANCODE_PAGEUP:
        manager.inc_speed();
        break;
      case VSX_SCANCODE_PAGEDOWN:
        manager.dec_speed();
        break;
      case VSX_SCANCODE_UP:
        manager.inc_fx_level();
        overlay->show_fx_graph();
        break;
      case VSX_SCANCODE_DOWN:
        manager.dec_fx_level();
        overlay->show_fx_graph();
        break;
      case VSX_SCANCODE_LEFT:
        manager.prev_state();
        break;
      case VSX_SCANCODE_RIGHT:
        manager.next_state();
        break;
      case VSX_SCANCODE_F1:
        overlay->set_help(1);
        break;
      case VSX_SCANCODE_F:
        overlay->set_help(2);
        break;
      case VSX_SCANCODE_R:
        if (vsx_input_keyboard.pressed_ctrl())
          manager.random_state();
        else
        {
          manager.toggle_randomizer();
          overlay->show_randomizer_status();
        }
        break;
    }
  }

};

