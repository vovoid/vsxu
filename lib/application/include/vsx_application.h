/**
* Project: VSXu: Realtime modular visual programming language, music/audio visualizer.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#pragma once

#include <vsx_argvector.h>
#include <input/vsx_input_event.h>

class vsx_application
{
protected:

public:

  virtual void init()
  {
  }

  virtual void uninit()
  {
  }

  virtual vsx_string<> window_title_get() = 0;

  virtual void pre_draw()
  {
  }

  virtual void draw()
  {
  }

  virtual void print_help()
  {
  }

  virtual void input_event(const vsx_input_event& event)
  {
    VSX_UNUSED(event);
  }


  virtual void event_text(const wchar_t& character_wide, char character)
  {
    VSX_UNUSED(character_wide);
    VSX_UNUSED(character);
  }

  virtual void event_key_down(long scancode)
  {
    VSX_UNUSED(scancode);
  }

  virtual void event_key_up(long scancode)
  {
    VSX_UNUSED(scancode);
  }

  // movement with left mouse button pressed, i.e. dragging or moving after click
  virtual void event_mouse_move(int x, int y)
  {
    VSX_UNUSED(x);
    VSX_UNUSED(y);
  }

  // movement without left button pressed - "hovering"
  virtual void event_mouse_move_passive(int x, int y)
  {
    VSX_UNUSED(x);
    VSX_UNUSED(y);
  }

  // buttons: 0 = left, 1 = middle, 2 = right
  virtual void event_mouse_down(unsigned long button, int x, int y)
  {
    VSX_UNUSED(button);
    VSX_UNUSED(x);
    VSX_UNUSED(y);
  }

  // buttons: 0 = left, 1 = middle, 2 = right
  virtual void event_mouse_up(unsigned long button, int x, int y)
  {
    VSX_UNUSED(button);
    VSX_UNUSED(x);
    VSX_UNUSED(y);
  }

  // -1 to -5 or whatever up to +1
  virtual void event_mouse_wheel(float diff, int x, int y)
  {
    VSX_UNUSED(diff);
    VSX_UNUSED(x);
    VSX_UNUSED(y);
  }
};
