#pragma once

#include <tools/vsx_req.h>
#include <input/vsx_input_event.h>

class vsx_input_state_mouse
{
public:
  vsx_vector2f position;
  float wheel;

  bool button_left;
  bool button_middle;
  bool button_right;
  bool button_x1;
  bool button_x2;

  void consume(vsx_input_event& event)
  {
    req(event.type == vsx_input_event::type_mouse);
    if (event.mouse.type == vsx_input_event_mouse::movement)
    {
      position.x = (float)event.mouse.x / (float)vsx_gl_state::get_instance()->viewport_get_width();
      position.y = (float)event.mouse.y / (float)vsx_gl_state::get_instance()->viewport_get_height();
    }
  }
};
