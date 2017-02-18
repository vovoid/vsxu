#pragma once

#include <inttypes.h>
#include <math/vector/vsx_vector2.h>

class vsx_input_event_game_controller
{
public:

  uint8_t id;

  union
  {
    float axis_value;
    bool button_state;
    const char* name_str;
  };

  enum {

    // left analog stick
    axis_analog_left_x,
    axis_analog_left_y,

    // right analog stick
    axis_analog_right_x,
    axis_analog_right_y,

    // triggers
    axis_trigger_left,
    axis_trigger_right,

    // pressing down on the analog sticks
    button_analog_left,
    button_analog_right,

    // dpad
    button_dpad_left,
    button_dpad_right,
    button_dpad_down,
    button_dpad_up,

    // right hand buttons
    button_left, // X on xbox 360 controllers, [] on PSx controllers
    button_down, // A on xbox 360 controllers, X on PSx controllers
    button_right, // B on xbox 360 controllers, O on PSx controllers
    button_up, // Y on xbox 360 controllers, /\ on PSx controllers

    // start / select buttons
    button_start,
    button_select, // or "back" on xbox 360 controllers

    // shoulder buttons
    button_shoulder_right,
    button_shoulder_left
  } type;

};
