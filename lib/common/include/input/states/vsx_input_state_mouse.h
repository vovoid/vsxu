#pragma once

#include <tools/vsx_req.h>
#include <input/vsx_input_event.h>
#include <vsx_gl_global.h>
#include <vsx_gl_state.h>

class vsx_input_state_mouse
{
public:
  vsx_vector2f position;
  vsx_vector2f position_screen;
  bool position_changed_since_last_frame = false;

  vsx_vector2f wheel;

  vsx_vector2f position_click_left;
  vsx_vector2f position_click_middle;
  vsx_vector2f position_click_right;

  bool buttons[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  bool button_left = false;
  bool button_middle = false;
  bool button_right = false;
  bool button_x1 = false;
  bool button_x2 = false;

  bool dragging = false;

  bool is_hovering()
  {
    return !button_left && !button_middle && !button_right;
  }

  vsx_vector2f position_centered()
  {
    return vsx_vector2f(-0.5, -0.5) + position;
  }

  vsx_vector2f position_click_left_centered()
  {
    return vsx_vector2f(-0.5, -0.5) + position_click_left;
  }

  vsx_vector2f position_click_right_centered()
  {
    return vsx_vector2f(-0.5, -0.5) + position_click_right;
  }

  void reset()
  {
    position_changed_since_last_frame = false;
  }

  void consume(const vsx_input_event& event)
  {
    req(event.type == vsx_input_event::type_mouse);

    if (event.mouse.type == vsx_input_event_mouse::movement)
    {
      position_screen.x = (float)event.mouse.x;
      position_screen.y = (float)event.mouse.y;
      position.x = (float)event.mouse.x / (float)vsx_gl_state::get_instance()->viewport_get_width();
      position.y = (float)event.mouse.y / (float)vsx_gl_state::get_instance()->viewport_get_height();
      dragging = button_left || button_right;
      position_changed_since_last_frame = true;
      return;
    }

    if (event.mouse.type == vsx_input_event_mouse::wheel)
    {
      wheel.x += (float)event.mouse.x;
      wheel.y += (float)event.mouse.y;
      return;
    }

    req(event.mouse.button_id < 32);
    buttons[event.mouse.button_id] = event.mouse.button_state;

    if (event.mouse.type == vsx_input_event_mouse::button_left)
    {
      button_left = event.mouse.button_state;
      if (button_left)
        position_click_left = position;
    }

    if (event.mouse.type == vsx_input_event_mouse::button_middle)
    {
      button_middle = event.mouse.button_state;
      if (button_middle)
        position_click_middle = position;
    }

    if (event.mouse.type == vsx_input_event_mouse::button_right)
    {
      button_right = event.mouse.button_state;
      if (button_right)
        position_click_right = position;
    }


    if (event.mouse.type == vsx_input_event_mouse::button_x1)
      button_x1 = event.mouse.button_state;

    if (event.mouse.type == vsx_input_event_mouse::button_x2)
      button_x2 = event.mouse.button_state;

  }

};
