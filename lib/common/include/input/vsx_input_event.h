#pragma once

#include <inttypes.h>

#include "events/vsx_input_event_game_controller.h"
#include "events/vsx_input_event_keyboard.h"
#include "events/vsx_input_event_mouse.h"
#include "events/vsx_input_event_text.h"

class vsx_input_event
{
public:

  enum event_type_list {
    type_keyboard,
    type_mouse,
    type_text,
    type_game_controller
  } type = type_keyboard;

  union
  {
    vsx_input_event_game_controller game_controller;
    vsx_input_event_keyboard keyboard;
    vsx_input_event_mouse mouse;
    vsx_input_event_text text;
  };

  vsx_input_event(event_type_list t)
    :
      type(t)
  {}
};

