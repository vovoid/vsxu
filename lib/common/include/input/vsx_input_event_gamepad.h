#pragma once

#include <inttypes.h>
#include <math/vector/vsx_vector2.h>
#include "vsx_input_event.h"

class vsx_input_event_gamepad
    : public vsx_input_event
{
public:

  vsx_vector2f analog_left;
  vsx_vector2f analog_right;

  bool left;
  bool down;
  bool right;
  bool up;

  bool shoulder_right;
  bool shoulder_left;

  bool start;
  bool select;

  bool d_pad_left;
  bool d_pad_down;
  bool d_pad_right;
  bool d_pad_up;
};
