/**
* Project: VSXu: Realtime modular visual programming language, music/audio visualizer.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Vovoid Media Technologies AB Copyright (C) 2003-2016
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

#include <vsx_version.h>
#include <vsx_platform.h>
#include <vsx_gl_global.h>
#include <vsx_application_manager.h>
#include <vsx_application_input_state_manager.h>
#include <vsx_application_run.h>

class input_test_application
    : public vsx_application
{
public:

  vsx_string<> window_title_get()
  {
    char titlestr[ 200 ];
    sprintf( titlestr, "Vovoid VSXu Applicataion Test Input %s [%s %d-bit]", VSXU_VER, PLATFORM_NAME, PLATFORM_BITS);
    return vsx_string<>(titlestr);
  }

  void draw()
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
    glVertex2f(0.0, 0.0);
    glVertex2f(1.0, 0.0);
    glVertex2f(0.0, 1.0);
    glEnd();
  }

  void input_event(const vsx_input_event& event)
  {
    if (event.type == vsx_input_event::type_mouse)
      vsx_printf(L" mouse event, type: %d    button id: %d   button state: %d   x: %d     y: %d\n ", event.mouse.type, event.mouse.button_id, event.mouse.button_state, event.mouse.x, event.mouse.y);

    if (event.type == vsx_input_event::type_keyboard)
      vsx_printf(L" keyboard event, pressed: %d     scancode: %d \n ", event.keyboard.pressed,  event.keyboard.scan_code);

    if (event.type == vsx_input_event::type_game_controller)
    {
      vsx_printf(L" dpad: %d %d %d %d\n", vsx_input_game.controllers[0].d_pad_left, vsx_input_game.controllers[0].d_pad_right, vsx_input_game.controllers[0].d_pad_up, vsx_input_game.controllers[0].d_pad_down);
      vsx_printf(L" dpad direction: %f, %f\n", vsx_input_game.controllers[0].d_pad_direction().x, vsx_input_game.controllers[0].d_pad_direction().y);
      vsx_printf(L" right buttons: %d %d %d %d\n", vsx_input_game.controllers[0].button_down, vsx_input_game.controllers[0].button_up, vsx_input_game.controllers[0].button_right, vsx_input_game.controllers[0].button_left);
      vsx_printf(L" analog left: %f %f\n", vsx_input_game.controllers[0].analog_left.x, vsx_input_game.controllers[0].analog_left.y);
      vsx_printf(L" analog right: %f %f \n", vsx_input_game.controllers[0].analog_right.x, vsx_input_game.controllers[0].analog_right.y);
      vsx_printf(L" start select: %d %d\n", vsx_input_game.controllers[0].button_start, vsx_input_game.controllers[0].button_select);
      vsx_printf(L" shoulder buttons: %d %d\n", vsx_input_game.controllers[0].button_shoulder_left, vsx_input_game.controllers[0].button_shoulder_right);
      vsx_printf(L" triggers: %f %f\n", vsx_input_game.controllers[0].trigger_left, vsx_input_game.controllers[0].trigger_right);
      vsx_printf(L" analog stick buttons: %d %d\n", vsx_input_game.controllers[0].button_analog_left, vsx_input_game.controllers[0].button_analog_right);
    }

  }

  void event_key_down(long key)
  {
    vsx_printf(L"application key down event, scan code: %d\n", key);
  }
};



int main(int argc, char* argv[])
{
  vsx_argvector::get_instance()->init_from_argc_argv(argc, argv);
  input_test_application application;
  vsx_application_manager::get_instance()->application_set(&application);
  vsx_application_run::run();
  return 0;
}
