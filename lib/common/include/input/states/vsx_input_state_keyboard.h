#pragma once

#include <tools/vsx_req.h>
#include <input/vsx_input_event.h>
#include <input/events/vsx_input_event_keyboard_scancode.h>

class vsx_input_state_keyboard
{
  bool keys[512];
  int64_t keys_counter[512];

public:

  bool pressed(int16_t key)
  {
    return keys[key];
  }

  bool pressed_alt()
  {
    if (keys[VSX_SCANCODE_LALT])
      return true;

    if (keys[VSX_SCANCODE_RALT])
      return true;

    return false;
  }

  bool pressed_ctrl()
  {
    if (keys[VSX_SCANCODE_LCTRL])
      return true;

    if (keys[VSX_SCANCODE_RCTRL])
      return true;

    return false;
  }

  bool pressed_shift()
  {
    if (keys[VSX_SCANCODE_LSHIFT])
      return true;

    if (keys[VSX_SCANCODE_RSHIFT])
      return true;

    return false;
  }

  void consume(vsx_input_event& event)
  {
    req(event.type == vsx_input_event::type_keyboard);
    keys[event.keyboard.scan_code] = event.keyboard.pressed;
  }

  vsx_input_state_keyboard()
  {
    for (size_t i = 0; i < 512; i++)
    {
      keys[i] = false;
      keys_counter[i] = 0;
    }
  }

};
