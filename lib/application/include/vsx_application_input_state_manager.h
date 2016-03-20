#pragma once

#include <input/vsx_input_event.h>
#include <input/states/vsx_input_state_keyboard.h>
#include <input/states/vsx_input_state_mouse.h>
#include <input/states/vsx_input_state_gamepad.h>

class vsx_application_input_state_manager
{
public:

  vsx_input_state_keyboard keyboard;
  vsx_input_state_mouse mouse;
  vsx_input_state_gamepad gamepad;

  void consume_event(vsx_input_event& event)
  {
    keyboard.consume(event);
    mouse.consume(event);
  }

  static vsx_application_input_state_manager* get()
  {
    static vsx_application_input_state_manager vaism;
    return &vaism;
  }
};

#define vsx_keyboard vsx_application_input_state_manager::get()->keyboard

