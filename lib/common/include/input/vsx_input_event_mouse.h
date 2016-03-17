#pragma once

#include <inttypes.h>
#include <math/vector/vsx_vector2.h>
#include "vsx_input_event.h"

class vsx_input_event_mouse
    : public vsx_input_event
{
public:
  bool button_left;
  bool button_right;
  bool buttons_extra[16];
  vsx_vector2f position;
  float scroll;
};
