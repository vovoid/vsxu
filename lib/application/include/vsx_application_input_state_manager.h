#pragma once

#include <input/vsx_input_event.h>
#include <input/states/vsx_input_state_keyboard.h>
#include <input/states/vsx_input_state_mouse.h>
#include <input/states/vsx_input_state_game.h>


class vsx_application_input_state_manager
{
public:

  vsx_input_state_keyboard keyboard;
  vsx_input_state_mouse mouse;
  vsx_input_state_game game;

  void consume_event(vsx_input_event& event)
  {
    keyboard.consume(event);
    mouse.consume(event);
    game.consume(event);
  }

  static vsx_application_input_state_manager* get()
  {
    static vsx_application_input_state_manager vaism;
    return &vaism;
  }
};

#define vsx_input_keyboard vsx_application_input_state_manager::get()->keyboard
#define vsx_input_mouse vsx_application_input_state_manager::get()->mouse
#define vsx_input_game vsx_application_input_state_manager::get()->game
