#pragma once

#include <inttypes.h>
#include "vsx_input_event.h"

class vsx_input_event_keyboard
    : public vsx_input_event
{
public:
  uint16_t scan_code = 0;
  bool pressed = false;

};
