#pragma once

#include <inttypes.h>
#include <math/vector/vsx_vector2.h>

class vsx_input_state_gamepad
{
public:

  vsx_vector2f analog_left;
  vsx_vector2f analog_right;

  bool button_left;
  bool button_down;
  bool button_right;
  bool button_up;

  bool shoulder_right;
  bool shoulder_left;

  bool start;
  bool select;

  bool d_pad_left;
  bool d_pad_down;
  bool d_pad_right;
  bool d_pad_up;

  void consume(vsx_input_event& event)
  {

  }

};
