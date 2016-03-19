#pragma once

#include <inttypes.h>
#include <math/vector/vsx_vector2.h>

class vsx_input_event_mouse
{
public:

  bool button_state;
  int32_t x;
  int32_t y;

  enum
  {
    movement,
    button_left,
    button_right,
    button_middle,
    button_x1,
    button_x2,
  } type;
};
