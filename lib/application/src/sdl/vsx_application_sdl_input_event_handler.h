#pragma once

#include <vsx_platform.h>
#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

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
    vsx_application_manager::get()->event_key_down(sdl_event.key.keysym.scancode);
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
    vsx_application_manager::get()->event_key_up(sdl_event.key.keysym.scancode);
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
    vsx_application_manager::get_instance()->get()->event_text( vsx_event.text.character_wide, vsx_event.text.character );
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

    if (sdl_event.motion.state)
      vsx_application_manager::get()->event_mouse_move(sdl_event.motion.x, sdl_event.motion.y);
    else
      vsx_application_manager::get()->event_mouse_move_passive(sdl_event.motion.x, sdl_event.motion.y);

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
      vsx_application_manager::get()->event_mouse_down(0, sdl_event.button.x, sdl_event.button.y);

    if (sdl_event.button.button == SDL_BUTTON_MIDDLE)
      vsx_application_manager::get()->event_mouse_down(1, sdl_event.button.x, sdl_event.button.y);

    if (sdl_event.button.button == SDL_BUTTON_RIGHT)
      vsx_application_manager::get()->event_mouse_down(2, sdl_event.button.x, sdl_event.button.y);

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
      vsx_application_manager::get()->event_mouse_up(0, sdl_event.motion.x, sdl_event.motion.y);

    if (sdl_event.button.button == SDL_BUTTON_MIDDLE)
      vsx_application_manager::get()->event_mouse_up(1, sdl_event.motion.x, sdl_event.motion.y);

    if (sdl_event.button.button == SDL_BUTTON_RIGHT)
      vsx_application_manager::get()->event_mouse_up(2, sdl_event.motion.x, sdl_event.motion.y);

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
    vsx_application_manager::get()->event_mouse_wheel((float)sdl_event.wheel.y, sdl_event.wheel.x, sdl_event.wheel.y);
    vsx_application_manager::get()->input_event(vsx_event);
  }

  // **************************************************************************
  // ** G A M E    C O N T R O L L E R ****************************************
  // **************************************************************************
  void handle_controller_axis_motion(SDL_Event& sdl_event)
  {
    // vsx event
    vsx_input_event vsx_event(vsx_input_event::type_game_controller);
    vsx_event.game_controller.id = sdl_event.caxis.which;
    if (sdl_event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
      vsx_event.game_controller.type = vsx_input_event_game_controller::axis_analog_left_x;

    if (sdl_event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY)
      vsx_event.game_controller.type = vsx_input_event_game_controller::axis_analog_left_y;

    if (sdl_event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX)
      vsx_event.game_controller.type = vsx_input_event_game_controller::axis_analog_right_x;

    if (sdl_event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY)
      vsx_event.game_controller.type = vsx_input_event_game_controller::axis_analog_right_y;

    if (sdl_event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT)
      vsx_event.game_controller.type = vsx_input_event_game_controller::axis_trigger_left;

    if (sdl_event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
      vsx_event.game_controller.type = vsx_input_event_game_controller::axis_trigger_right;

    vsx_event.game_controller.axis_value = 1.0f/32767.0f * (float)sdl_event.caxis.value;

    if (sdl_event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY)
      vsx_event.game_controller.axis_value = -vsx_event.game_controller.axis_value;

    if (sdl_event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY)
      vsx_event.game_controller.axis_value = -vsx_event.game_controller.axis_value;

    // application
    vsx_application_input_state_manager::get()->consume_event(vsx_event);
    vsx_application_manager::get()->input_event(vsx_event);
  }

  void handle_controller_button_down(SDL_Event& sdl_event)
  {
    // vsx event
    vsx_input_event vsx_event(vsx_input_event::type_game_controller);
    vsx_event.game_controller.button_state = true;
    vsx_event.game_controller.id = sdl_event.cbutton.which;

    if (sdl_event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT) vsx_event.game_controller.type = vsx_input_event_game_controller::button_dpad_left;
    if (sdl_event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT) vsx_event.game_controller.type = vsx_input_event_game_controller::button_dpad_right;
    if (sdl_event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_DOWN) vsx_event.game_controller.type = vsx_input_event_game_controller::button_dpad_down;
    if (sdl_event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_UP) vsx_event.game_controller.type = vsx_input_event_game_controller::button_dpad_up;
    if (sdl_event.cbutton.button == SDL_CONTROLLER_BUTTON_A) vsx_event.game_controller.type = vsx_input_event_game_controller::button_down;
    if (sdl_event.cbutton.button == SDL_CONTROLLER_BUTTON_B) vsx_event.game_controller.type = vsx_input_event_game_controller::button_right;
    if (sdl_event.cbutton.button == SDL_CONTROLLER_BUTTON_X) vsx_event.game_controller.type = vsx_input_event_game_controller::button_left;
    if (sdl_event.cbutton.button == SDL_CONTROLLER_BUTTON_Y) vsx_event.game_controller.type = vsx_input_event_game_controller::button_up;
    if (sdl_event.cbutton.button == SDL_CONTROLLER_BUTTON_START) vsx_event.game_controller.type = vsx_input_event_game_controller::button_start;
    if (sdl_event.cbutton.button == SDL_CONTROLLER_BUTTON_BACK) vsx_event.game_controller.type = vsx_input_event_game_controller::button_select;
    if (sdl_event.cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) vsx_event.game_controller.type = vsx_input_event_game_controller::button_shoulder_right;
    if (sdl_event.cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSHOULDER) vsx_event.game_controller.type = vsx_input_event_game_controller::button_shoulder_left;
    if (sdl_event.cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSTICK) vsx_event.game_controller.type = vsx_input_event_game_controller::button_analog_left;
    if (sdl_event.cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSTICK) vsx_event.game_controller.type = vsx_input_event_game_controller::button_analog_right;

    // application
    vsx_application_input_state_manager::get()->consume_event(vsx_event);
    vsx_application_manager::get()->input_event(vsx_event);
  }

  void handle_controller_button_up(SDL_Event& sdl_event)
  {
    // vsx event
    vsx_input_event vsx_event(vsx_input_event::type_game_controller);
    vsx_event.game_controller.button_state = false;
    vsx_event.game_controller.id = sdl_event.cbutton.which;

    if (sdl_event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT) vsx_event.game_controller.type = vsx_input_event_game_controller::button_dpad_left;
    if (sdl_event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT) vsx_event.game_controller.type = vsx_input_event_game_controller::button_dpad_right;
    if (sdl_event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_DOWN) vsx_event.game_controller.type = vsx_input_event_game_controller::button_dpad_down;
    if (sdl_event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_UP) vsx_event.game_controller.type = vsx_input_event_game_controller::button_dpad_up;
    if (sdl_event.cbutton.button == SDL_CONTROLLER_BUTTON_A) vsx_event.game_controller.type = vsx_input_event_game_controller::button_down;
    if (sdl_event.cbutton.button == SDL_CONTROLLER_BUTTON_B) vsx_event.game_controller.type = vsx_input_event_game_controller::button_right;
    if (sdl_event.cbutton.button == SDL_CONTROLLER_BUTTON_X) vsx_event.game_controller.type = vsx_input_event_game_controller::button_left;
    if (sdl_event.cbutton.button == SDL_CONTROLLER_BUTTON_Y) vsx_event.game_controller.type = vsx_input_event_game_controller::button_up;
    if (sdl_event.cbutton.button == SDL_CONTROLLER_BUTTON_START) vsx_event.game_controller.type = vsx_input_event_game_controller::button_start;
    if (sdl_event.cbutton.button == SDL_CONTROLLER_BUTTON_BACK) vsx_event.game_controller.type = vsx_input_event_game_controller::button_select;
    if (sdl_event.cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) vsx_event.game_controller.type = vsx_input_event_game_controller::button_shoulder_right;
    if (sdl_event.cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSHOULDER) vsx_event.game_controller.type = vsx_input_event_game_controller::button_shoulder_left;
    if (sdl_event.cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSTICK) vsx_event.game_controller.type = vsx_input_event_game_controller::button_analog_left;
    if (sdl_event.cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSTICK) vsx_event.game_controller.type = vsx_input_event_game_controller::button_analog_right;

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
