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

class vsx_application_artiste
    : public vsx_application
{
  vsx_string<> window_title;

  vsx_artiste_draw my_draw;

public:

  void init()
  {
    my_draw.init();
  }

  void uninit()
  {
    my_draw.uninit();
  }

  void window_title_set(vsx_string<> new_title)
  {
    window_title = new_title;
  }

  vsx_string<> window_title_get()
  {
    return window_title;
  }

  void pre_draw()
  {
    my_draw.pre_draw();
  }

  void draw()
  {
    my_draw.draw();
  }

  void print_help()
  {
    vsx_printf(
       L"VSXu Artiste command syntax:\n"
       "  -f             fullscreen mode\n"
       "  -ff            start preview in fullwindow mode (same as Ctrl+F)"
       "  -s 1920,1080   screen/window size\n"
       "  -p 100,100     window posision\n"
       "  -novsync       disable vsync\n"
       "  -gl_debug      enable nvidia's gl debug callback\n"
       "\n"
    );
    vsx_module_list_factory_create()->print_help();
  }

  void input_event(const vsx_input_event& event)
  {
    req(my_draw.is_engine_fullscreen());
  }


  void char_event(const wchar_t& character)
  {
    req(my_draw.desktop);
    my_draw.desktop->key_down(character, vsx_input_keyboard.pressed_alt(), vsx_input_keyboard.pressed_ctrl(), vsx_input_keyboard.pressed_shift());
  }

  void key_down_event(long scancode)
  {
    req(my_draw.desktop);
    if (vsx_input_keyboard.pressed_ctrl() && scancode == VSX_SCANCODE_5)
      vsx_profiler_manager::get_instance()->enable();

    if (vsx_input_keyboard.pressed_ctrl() && scancode == VSX_SCANCODE_4)
      vsx_profiler_manager::get_instance()->disable();

    my_draw.desktop->key_down(scancode, vsx_input_keyboard.pressed_alt(), vsx_input_keyboard.pressed_ctrl(), vsx_input_keyboard.pressed_shift());
  }

  void key_up_event(long scancode)
  {
    req(my_draw.desktop);
    my_draw.desktop->key_up(scancode, vsx_input_keyboard.pressed_alt(), vsx_input_keyboard.pressed_ctrl(), vsx_input_keyboard.pressed_shift());
  }

  // movement with left mouse button pressed, i.e. dragging or moving after click
  void mouse_move_event(int x, int y)
  {
    req(my_draw.desktop);
    my_draw.desktop->mouse_move(x, y);
  }

  // movement without left button pressed - "hovering"
  void mouse_move_passive_event(int x, int y)
  {
    req(my_draw.desktop);
    my_draw.desktop->mouse_move_passive(x, y);
  }

  // buttons: 0 = left, 1 = middle, 2 = right
  void mouse_down_event(unsigned long button, int x, int y)
  {
    req(my_draw.desktop);
    my_draw.desktop->mouse_down(x, y, button );
  }

  // buttons: 0 = left, 1 = middle, 2 = right
  void mouse_up_event(unsigned long button, int x, int y)
  {
    req(my_draw.desktop);
    my_draw.desktop->mouse_up( x, y, button );
  }

  // -1 to -5 or whatever up to +1
  void mouse_wheel_event(float diff, int x, int y)
  {
    VSX_UNUSED(diff);
    VSX_UNUSED(x);
    VSX_UNUSED(y);
  }
};

