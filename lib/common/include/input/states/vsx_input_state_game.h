#pragma once

#include <inttypes.h>
#include <tools/vsx_req.h>
#include <math/vector/vsx_vector2.h>
#include <input/vsx_input_event.h>
#include "game/vsx_input_state_game_controller.h"

class vsx_input_state_game
{
public:
  vsx_input_state_game_controller controllers[4];

  void consume(vsx_input_event& event)
  {
    controllers[event.game_controller.id].consume(event);
  }

  vsx_input_state_game()
  {
  }
};
