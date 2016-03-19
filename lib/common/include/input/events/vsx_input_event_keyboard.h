#pragma once

#include <inttypes.h>

class vsx_input_event_keyboard
{
public:
  uint16_t scan_code = 0;
  bool pressed = false;
};
