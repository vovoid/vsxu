#pragma once

#include <inttypes.h>
#include <math/vector/vsx_vector2.h>

class vsx_input_event_gamepad
{
public:

  union
  {
    float axis_value;
    bool button_value;
  };

  enum {
    // analog
    axis_left_x,
    axis_left_y,
    axis_right_x,
    axis_right_y,

    // dpad
    button_dpad_left,
    button_dpad_right,
    button_dpad_down,
    button_dpad_up,

    // right hand buttons
    button_left,
    button_down,
    button_right,
    button_up,

    // start / select
    button_start,
    button_select,

    // shoulder
    button_shoulder_right,
    button_shoulder_left
  } type;

};
