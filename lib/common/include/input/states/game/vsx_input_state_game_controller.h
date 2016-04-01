#pragma once

#include <inttypes.h>
#include <tools/vsx_req.h>
#include <math/vector/vsx_vector2.h>
#include <input/vsx_input_event.h>
#include <string/vsx_string.h>

class vsx_input_state_game_controller
{
public:

  vsx_vector2f analog_left;
  float analog_left_treshold = 0.15f;

  vsx_vector2f analog_right;
  float analog_right_treshold = 0.15f;

  float trigger_left = 0.0f;
  float trigger_right = 0.0f;

  bool d_pad_left = false;
  bool d_pad_down = false;
  bool d_pad_right = false;
  bool d_pad_up = false;

  bool button_analog_left = false;
  bool button_analog_right = false;

  bool button_left = false;
  bool button_down = false;
  bool button_right = false;
  bool button_up = false;

  bool button_shoulder_right = false;
  bool button_shoulder_left = false;

  bool button_start = false;
  bool button_select = false;

  vsx_vector2f d_pad_direction()
  {
    if (d_pad_left)
      return vsx_vector2f(-1.0f, 0.0f);
    if (d_pad_right)
      return vsx_vector2f(1.0f, 0.0f);
    if (d_pad_up)
      return vsx_vector2f(0.0f, 1.0f);
    if (d_pad_down)
      return vsx_vector2f(0.0f, -1.0f);
    return vsx_vector2f(0.0f, 0.0f);
  }

  void consume(vsx_input_event& event)
  {
    req(event.type == vsx_input_event::type_game_controller);
    if (event.game_controller.type == vsx_input_event_game_controller::axis_analog_left_x)
      analog_left.x = fabs(event.game_controller.axis_value) > analog_left_treshold ? event.game_controller.axis_value : 0.0f;
    if (event.game_controller.type == vsx_input_event_game_controller::axis_analog_left_y)
      analog_left.y = fabs(event.game_controller.axis_value) > analog_left_treshold ? event.game_controller.axis_value : 0.0f;

    if (event.game_controller.type == vsx_input_event_game_controller::axis_analog_right_x)
      analog_right.x = fabs(event.game_controller.axis_value) > analog_right_treshold ? event.game_controller.axis_value : 0.0f;
    if (event.game_controller.type == vsx_input_event_game_controller::axis_analog_right_y)
      analog_right.y = fabs(event.game_controller.axis_value) > analog_right_treshold ? event.game_controller.axis_value : 0.0f;

    if (event.game_controller.type == vsx_input_event_game_controller::axis_trigger_left)
      trigger_left = event.game_controller.axis_value;
    if (event.game_controller.type == vsx_input_event_game_controller::axis_trigger_right)
      trigger_right = event.game_controller.axis_value;

    if (event.game_controller.type == vsx_input_event_game_controller::button_dpad_left)
      d_pad_left =  event.game_controller.button_state;
    if (event.game_controller.type == vsx_input_event_game_controller::button_dpad_down)
      d_pad_down =  event.game_controller.button_state;
    if (event.game_controller.type == vsx_input_event_game_controller::button_dpad_right)
      d_pad_right =  event.game_controller.button_state;
    if (event.game_controller.type == vsx_input_event_game_controller::button_dpad_up)
      d_pad_up =  event.game_controller.button_state;

    if (event.game_controller.type == vsx_input_event_game_controller::button_analog_left)
      button_analog_left =  event.game_controller.button_state;
    if (event.game_controller.type == vsx_input_event_game_controller::button_analog_right)
      button_analog_right = event.game_controller.button_state;

    if (event.game_controller.type == vsx_input_event_game_controller::button_left)
      button_left = event.game_controller.button_state;
    if (event.game_controller.type == vsx_input_event_game_controller::button_down)
      button_down = event.game_controller.button_state;
    if (event.game_controller.type == vsx_input_event_game_controller::button_right)
      button_right = event.game_controller.button_state;
    if (event.game_controller.type == vsx_input_event_game_controller::button_up)
      button_up = event.game_controller.button_state;

    if (event.game_controller.type == vsx_input_event_game_controller::button_shoulder_right)
      button_shoulder_right = event.game_controller.button_state;
    if (event.game_controller.type == vsx_input_event_game_controller::button_shoulder_left)
      button_shoulder_left = event.game_controller.button_state;

    if (event.game_controller.type == vsx_input_event_game_controller::button_start)
      button_start = event.game_controller.button_state;
    if (event.game_controller.type == vsx_input_event_game_controller::button_select)
      button_select = event.game_controller.button_state;
  }
};
