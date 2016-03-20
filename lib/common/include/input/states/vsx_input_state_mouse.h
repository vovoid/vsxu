#pragma once

#include <tools/vsx_req.h>
#include <input/vsx_input_event.h>
#include <vsx_gl_global.h>
#include <vsx_gl_state.h>

class vsx_input_state_mouse
{
public:
  vsx_vector2f position;
  vsx_vector2f wheel;

  bool buttons[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  bool button_left = false;
  bool button_middle = false;
  bool button_right = false;
  bool button_x1 = false;
  bool button_x2 = false;

  void consume(vsx_input_event& event)
  {
    req(event.type == vsx_input_event::type_mouse);
    req(event.mouse.button_id < 32);

    if (event.mouse.type == vsx_input_event_mouse::movement)
    {
      position.x = (float)event.mouse.x / (float)vsx_gl_state::get_instance()->viewport_get_width();
      position.y = (float)event.mouse.y / (float)vsx_gl_state::get_instance()->viewport_get_height();
    }

    buttons[event.mouse.button_id] = event.mouse.button_state;

    if (event.mouse.type == vsx_input_event_mouse::button_left)
      button_left = event.mouse.button_state;

    if (event.mouse.type == vsx_input_event_mouse::button_right)
      button_right = event.mouse.button_state;

    if (event.mouse.type == vsx_input_event_mouse::button_middle)
      button_middle = event.mouse.button_state;

    if (event.mouse.type == vsx_input_event_mouse::button_x1)
      button_x1 = event.mouse.button_state;

    if (event.mouse.type == vsx_input_event_mouse::button_x2)
      button_x2 = event.mouse.button_state;

    if (event.mouse.type == vsx_input_event_mouse::wheel)
    {
      wheel.x += (float)event.mouse.x;
      wheel.y += (float)event.mouse.y;
    }
  }

};
