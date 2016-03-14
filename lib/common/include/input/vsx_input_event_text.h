#pragma once

#include <inttypes.h>
#include "vsx_input_event.h"

class vsx_input_event_keyboard
    : public vsx_input_event
{
public:
  wchar_t character_wide = 0;
  char character = 0;
};
