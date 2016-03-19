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
  // **************************************************************************
  // ** K E Y B O A R D *******************************************************
  // **************************************************************************

  void handle_key_down(SDL_Event& sdl_event)
  {
    // vsx event
    vsx_input_event vsx_event(vsx_input_event::type_keyboard);
    vsx_event.keyboard.pressed = true;
    vsx_event.keyboard.scan_code = sdl_event.key.keysym.scancode;

    // application
    vsx_application_input_state_manager::get()->consume_event(vsx_event);
    vsx_application_manager::get()->key_down_event(sdl_event.key.keysym.scancode);
    vsx_application_manager::get()->input_event(vsx_event);
  }

  void handle_key_up(SDL_Event& sdl_event)
  {
    // vsx event
    vsx_input_event vsx_event(vsx_input_event::type_keyboard);
    vsx_event.keyboard.pressed = false;
    vsx_event.keyboard.scan_code = sdl_event.key.keysym.scancode;

    // application
    vsx_application_input_state_manager::get()->consume_event(vsx_event);
    vsx_application_manager::get()->key_up_event(sdl_event.key.keysym.scancode);
    vsx_application_manager::get()->input_event(vsx_event);
  }

  vsx_string<> utf8_text;
  vsx_string<wchar_t> text;
  void handle_text(SDL_Event& sdl_event)
  {
    // data
    utf8_text = sdl_event.text.text;
    vsx_string_helper::utf8_string_to_wchar_string(text, utf8_text);

    // vsx event
    vsx_input_event vsx_event(vsx_input_event::type_text);
    vsx_event.text.character_wide = text[0];
    if (vsx_event.text.character_wide > 255)
      vsx_event.text.character = '?';
    else
      vsx_event.text.character = (char)(text[0] & 0x00FF);

    // application
    vsx_application_input_state_manager::get()->consume_event(vsx_event);
    vsx_application_manager::get_instance()->get()->char_event( text[0] );
    vsx_application_manager::get()->input_event(vsx_event);
  }


  // **************************************************************************
  // ** M O U S E *************************************************************
  // **************************************************************************
  void handle_mouse_motion(SDL_Event& sdl_event)
  {
    // vsx event
    vsx_input_event vsx_event(vsx_input_event::type_mouse);
    vsx_event.mouse.type = vsx_input_event_mouse::movement;
    vsx_event.mouse.x = sdl_event.motion.x;
    vsx_event.mouse.y = sdl_event.motion.y;
    vsx_event.mouse.button_state = false;

    // application
    vsx_application_input_state_manager::get()->consume_event(vsx_event);

    if (sdl_event.motion.state & SDL_BUTTON_LMASK)
      vsx_application_manager::get()->mouse_move_event(sdl_event.motion.x, sdl_event.motion.y);
    else
      vsx_application_manager::get()->mouse_move_passive_event(sdl_event.motion.x, sdl_event.motion.y);

    vsx_application_manager::get()->input_event(vsx_event);
  }

  void handle_mouse_down(SDL_Event& sdl_event)
  {
    // vsx event
    vsx_input_event vsx_event(vsx_input_event::type_mouse);

    vsx_event.mouse.type = vsx_input_event_mouse::button_other;

    if (sdl_event.button.button == SDL_BUTTON_LEFT)
      vsx_event.mouse.type = vsx_input_event_mouse::button_left;

    if (sdl_event.button.button == SDL_BUTTON_RIGHT)
      vsx_event.mouse.type = vsx_input_event_mouse::button_right;

    if (sdl_event.button.button == SDL_BUTTON_MIDDLE)
      vsx_event.mouse.type = vsx_input_event_mouse::button_middle;

    if (sdl_event.button.button == SDL_BUTTON_X1)
      vsx_event.mouse.type = vsx_input_event_mouse::button_x1;

    if (sdl_event.button.button == SDL_BUTTON_X2)
      vsx_event.mouse.type = vsx_input_event_mouse::button_x2;

    vsx_event.mouse.button_id = sdl_event.button.button;
    vsx_event.mouse.button_state = true;

    // application
    vsx_application_input_state_manager::get()->consume_event(vsx_event);

    if (sdl_event.button.button == SDL_BUTTON_LEFT)
      vsx_application_manager::get()->mouse_down_event(0, sdl_event.button.x, sdl_event.button.y);

    if (sdl_event.button.button == SDL_BUTTON_MIDDLE)
      vsx_application_manager::get()->mouse_down_event(1, sdl_event.button.x, sdl_event.button.y);

    if (sdl_event.button.button == SDL_BUTTON_RIGHT)
      vsx_application_manager::get()->mouse_down_event(2, sdl_event.button.x, sdl_event.button.y);

    vsx_application_manager::get()->input_event(vsx_event);
  }

  void handle_mouse_up(SDL_Event& sdl_event)
  {
    // vsx event
    vsx_input_event vsx_event(vsx_input_event::type_mouse);

    vsx_event.mouse.type = vsx_input_event_mouse::button_other;

    if (sdl_event.button.button == SDL_BUTTON_LEFT)
      vsx_event.mouse.type = vsx_input_event_mouse::button_left;

    if (sdl_event.button.button == SDL_BUTTON_RIGHT)
      vsx_event.mouse.type = vsx_input_event_mouse::button_right;

    if (sdl_event.button.button == SDL_BUTTON_MIDDLE)
      vsx_event.mouse.type = vsx_input_event_mouse::button_middle;

    if (sdl_event.button.button == SDL_BUTTON_X1)
      vsx_event.mouse.type = vsx_input_event_mouse::button_x1;

    if (sdl_event.button.button == SDL_BUTTON_X2)
      vsx_event.mouse.type = vsx_input_event_mouse::button_x2;

    vsx_event.mouse.button_id = sdl_event.button.button;
    vsx_event.mouse.button_state = false;

    // application
    vsx_application_input_state_manager::get()->consume_event(vsx_event);

    if (sdl_event.button.button == SDL_BUTTON_LEFT)
      vsx_application_manager::get()->mouse_up_event(0, sdl_event.motion.x, sdl_event.motion.y);

    if (sdl_event.button.button == SDL_BUTTON_MIDDLE)
      vsx_application_manager::get()->mouse_up_event(1, sdl_event.motion.x, sdl_event.motion.y);

    if (sdl_event.button.button == SDL_BUTTON_RIGHT)
      vsx_application_manager::get()->mouse_up_event(2, sdl_event.motion.x, sdl_event.motion.y);

    vsx_application_manager::get()->input_event(vsx_event);
  }

  void handle_mouse_wheel(SDL_Event& sdl_event)
  {
    // vsx event
    vsx_input_event vsx_event(vsx_input_event::type_mouse);
    vsx_event.mouse.x = sdl_event.wheel.x;
    vsx_event.mouse.y = sdl_event.wheel.y;
    vsx_event.mouse.type = vsx_input_event_mouse::wheel;
    vsx_event.mouse.button_state = false;

    // application
    vsx_application_input_state_manager::get()->consume_event(vsx_event);
    vsx_application_manager::get()->mouse_wheel_event(sdl_event.wheel.y, sdl_event.wheel.x, sdl_event.wheel.y);
    vsx_application_manager::get()->input_event(vsx_event);
  }

  // **************************************************************************
  // ** G A M E    C O N T R O L L E R ****************************************
  // **************************************************************************
  void handle_controller_axis_motion(SDL_Event& sdl_event)
  {
    // vsx event
    vsx_input_event vsx_event(vsx_input_event::type_gamepad);
    if (sdl_event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
      vsx_event.gamepad.type = vsx_input_event_gamepad::axis_left_x;

    if (sdl_event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY)
      vsx_event.gamepad.type = vsx_input_event_gamepad::axis_left_y;

    // application
    vsx_application_input_state_manager::get()->consume_event(vsx_event);
    vsx_application_manager::get()->input_event(vsx_event);
  }

  void handle_controller_button_down(SDL_Event& sdl_event)
  {
    // vsx event
    vsx_input_event vsx_event(vsx_input_event::type_gamepad);

    // application
    vsx_application_input_state_manager::get()->consume_event(vsx_event);
    vsx_application_manager::get()->input_event(vsx_event);
  }

  void handle_controller_button_up(SDL_Event& sdl_event)
  {
    // vsx event
    vsx_input_event vsx_event(vsx_input_event::type_gamepad);

    // application
    vsx_application_input_state_manager::get()->consume_event(vsx_event);
    vsx_application_manager::get()->input_event(vsx_event);
  }

public:

  void consume(SDL_Event& sdl_event)
  {
    if (sdl_event.type == SDL_KEYDOWN) handle_key_down(sdl_event);
    if (sdl_event.type == SDL_KEYUP) handle_key_up(sdl_event);
    if (sdl_event.type == SDL_TEXTINPUT) handle_text(sdl_event);
    if (sdl_event.type == SDL_MOUSEMOTION) handle_mouse_motion(sdl_event);
    if (sdl_event.type == SDL_MOUSEBUTTONDOWN) handle_mouse_down(sdl_event);
    if (sdl_event.type == SDL_MOUSEBUTTONUP) handle_mouse_up(sdl_event);
    if (sdl_event.type == SDL_MOUSEWHEEL) handle_mouse_wheel(sdl_event);
    if (sdl_event.type == SDL_CONTROLLERAXISMOTION) handle_controller_axis_motion(sdl_event);
    if (sdl_event.type == SDL_CONTROLLERBUTTONDOWN) handle_controller_button_down(sdl_event);
    if (sdl_event.type == SDL_CONTROLLERBUTTONUP) handle_controller_button_up(sdl_event);
  }
};
