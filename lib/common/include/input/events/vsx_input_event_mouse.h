#pragma once

#include <inttypes.h>
#include <math/vector/vsx_vector2.h>

class vsx_input_event_mouse
{
public:

  bool button_state = false;
  union{
    int32_t x = 0;
    uint32_t button_id;
  };
  int32_t y = 0;

  enum
  {
    movement,
    button_left,
    button_right,
    button_middle,
    button_x1,
    button_x2,
    button_other,
    wheel
  } type;
};
