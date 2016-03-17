#pragma once

#include <SDL2/SDL.h>
#include <input/vsx_input_scancode.h>
#include <string/vsx_string_helper.h>
#include "vsx_application_manager.h"

class vsx_application_input_manager
{
  bool keys[512];
  int64_t keys_counter[512];

  vsx_string<> utf8_text;
  vsx_string<wchar_t> text;

  void handle_key_down(SDL_Event& event)
  {
    keys[event.key.keysym.scancode] = true;
    vsx_application_manager::get_instance()->get()->key_down_event(event.key.keysym.scancode);
  }

  void handle_key_up(SDL_Event& event)
  {
    keys[event.key.keysym.scancode] = false;
    vsx_application_manager::get_instance()->get()->key_up_event(event.key.keysym.scancode);
  }

public:

  vsx_application_input_manager()
  {
    for (size_t i = 0; i < 512; i++)
    {
      keys[i] = false;
      keys_counter[i] = 0;
    }
  }

  bool get_key(int16_t key)
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

  void consume(SDL_Event& event)
  {
    if (event.type == SDL_KEYDOWN)
      handle_key_down(event);

    if (event.type == SDL_KEYUP)
      handle_key_up(event);

    if (event.type == SDL_TEXTINPUT)
    {
      utf8_text = event.text.text;
      vsx_string_helper::utf8_string_to_wchar_string(text, utf8_text);
      vsx_application_manager::get_instance()->get()->char_event( text[0] );
    }
  }

  static vsx_application_input_manager* get_instance()
  {
    static vsx_application_input_manager seh;
    return &seh;
  }
};
