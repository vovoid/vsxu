#pragma once

#include <inttypes.h>
#include <math/vector/vsx_vector2.h>

class vsx_input_event_mouse
{
public:

  bool button_state = false;

  // x on screen when type == movement
  // button_id otherwise
  union{
    int32_t x = 0;
    uint32_t button_id;
  };

  // y on screen
  // mousewheel movement
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
