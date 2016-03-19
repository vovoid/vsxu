#pragma once

#include <SDL2/SDL.h>
#include <input/events/vsx_input_event_keyboard_scancode.h>
#include <input/vsx_input_event.h>
#include <vsx_gl_state.h>
#include <string/vsx_string_helper.h>

#include "vsx_application_manager.h"
#include "vsx_application_input_state_manager.h"

class vsx_application_sdl_input_event_handler
{
  vsx_string<> utf8_text;
  vsx_string<wchar_t> text;

  void handle_key_down(SDL_Event& event)
  {
    vsx_application_manager::get()->key_down_event(event.key.keysym.scancode);
    vsx_input_event vsx_event(vsx_input_event::type_keyboard);
    vsx_event.keyboard.pressed = true;
    vsx_event.keyboard.scan_code = event.key.keysym.scancode;
    vsx_application_input_state_manager::get()->consume_event(vsx_event);
    vsx_application_manager::get()->input_event(vsx_event);
  }

  void handle_key_up(SDL_Event& event)
  {
    vsx_application_manager::get()->key_up_event(event.key.keysym.scancode);
    vsx_input_event vsx_event(vsx_input_event::type_keyboard);
    vsx_event.keyboard.pressed = true;
    vsx_event.keyboard.scan_code = event.key.keysym.scancode;
    vsx_application_input_state_manager::get()->consume_event(vsx_event);
    vsx_application_manager::get()->input_event(vsx_event);
  }

  void handle_mouse_motion(SDL_Event& event)
  {
    vsx_input_event vsx_event(vsx_input_event::type_mouse);
    vsx_event.mouse.type = vsx_input_event_mouse::movement;
    vsx_event.mouse.x = event.motion.x;
    vsx_event.mouse.y = event.motion.y;
    vsx_application_input_state_manager::get()->consume_event(vsx_event);
  }

public:

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


    if (event.type == SDL_MOUSEMOTION)
      handle_mouse_motion(event);
  }
};
