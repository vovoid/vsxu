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

#pragma once

#include <vsx_application.h>
#include <vsx_application_input_state_manager.h>
#include "vsx_artiste_draw.h"
#include <vsx_module_list_factory.h>
#include <perf/vsx_perf.h>

class vsx_application_artiste
    : public vsx_application
{
  vsx_artiste_draw my_draw;
  vsx_input_event_queue event_queue;

  size_t window_title_i = 0;
  void update_window_title()
  {
    req(!(window_title_i++ % 60));
    vsx_perf perf;
    char titlestr[ 200 ];
    sprintf( titlestr, "Vovoid VSXu Artiste %s [%s %d-bit] [%d MB RAM used] %s", VSXU_VER, PLATFORM_NAME, PLATFORM_BITS, perf.memory_currently_used(), VSXU_VERSION_COPYRIGHT);
    window_title = vsx_string<>(titlestr);
    vsx_application_control::get_instance()->window_title = window_title;
  }

public:

  vsx_application_artiste()
  {
    update_window_title();
  }

  void init_graphics()
  {
    my_draw.init();
    my_draw.vxe->set_input_event_queue(&event_queue);
  }

  void uninit_graphics()
  {
    my_draw.uninit();
  }

  void pre_draw()
  {
    my_draw.pre_draw();
  }

  void draw()
  {
    my_draw.draw();
    event_queue.reset();
    update_window_title();
  }

  void print_help()
  {
    vsx_printf(
       L"VSXu Artiste command syntax:\n"
       "  -ff            start preview in fullwindow mode (same as Ctrl+F)"
       "  -gl_debug      enable nvidia's gl debug callback\n"
       "\n"
    );
    vsx_module_list_factory_create()->print_help();
  }

  void input_event(const vsx_input_event& event)
  {
    if (!my_draw.is_engine_fullscreen())
    {
      if (event.type == vsx_input_event::type_keyboard)
        return;

      if (event.type == vsx_input_event::type_mouse)
        return;
    }
    event_queue.add(event);
  }


  void event_text(const wchar_t& character_wide, char character)
  {
    req(my_draw.desktop);
    my_draw.desktop->input_text(character_wide, character);
  }

  void event_key_down(long scancode)
  {
    my_draw.key_down_event(scancode);

    req(my_draw.desktop);
    if (vsx_input_keyboard.pressed_ctrl() && scancode == VSX_SCANCODE_5)
      vsx_profiler_manager::get_instance()->enable();

    if (vsx_input_keyboard.pressed_ctrl() && scancode == VSX_SCANCODE_4)
      vsx_profiler_manager::get_instance()->disable();

    my_draw.desktop->input_key_down(scancode);
  }

  void event_key_up(long scancode)
  {
    req(my_draw.desktop);
    my_draw.desktop->input_key_up(scancode);
  }

  // movement with left mouse button pressed, i.e. dragging or moving after click
  void event_mouse_move(int x, int y)
  {
    req(my_draw.desktop);
    my_draw.desktop->input_mouse_move(x, y);
  }

  // movement without left button pressed - "hovering"
  void event_mouse_move_passive(int x, int y)
  {
    req(my_draw.desktop);
    my_draw.desktop->input_mouse_move_passive(x, y);
  }

  // buttons: 0 = left, 1 = middle, 2 = right
  void event_mouse_down(unsigned long button, int x, int y)
  {
    req(my_draw.desktop);
    my_draw.desktop->input_mouse_down(x, y, button );
  }

  // buttons: 0 = left, 1 = middle, 2 = right
  void event_mouse_up(unsigned long button, int x, int y)
  {
    req(my_draw.desktop);
    my_draw.desktop->input_mouse_up( x, y, button );
  }

  // -1 to -5 or whatever up to +1
  void event_mouse_wheel(float diff, int x, int y)
  {
    my_draw.desktop->input_mouse_wheel(diff);
    VSX_UNUSED(x);
    VSX_UNUSED(y);
  }
};

