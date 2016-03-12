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

class vsx_application
{
protected:

  vsx_string<> window_title;

public:

  virtual void init(int id) // which context is used?
  {
    VSX_UNUSED(id);
  }

  void window_title_set(vsx_string<> new_title)
  {
    window_title = new_title;
  }

  vsx_string<> window_title_get()
  {
    return window_title;
  }

  virtual void pre_draw()
  {
  }

  virtual void draw(int id)
  {
    VSX_UNUSED(id);
  }

  virtual void print_help()
  {
    vsx_printf(
      L"Usage:\n"
       "  vsxu_player [path_to_vsx_file]\n"
       "\n"
       "Flags: \n"
       "  -p [x,y]   Set window position x,y \n"
       "  -s [x,y]   Set window size x,y \n\n\n"
    );
  }


  virtual void char_event(const wchar_t& character)
  {
    VSX_UNUSED(character);
  }

  virtual void key_down_event(long scancode)
  {
    VSX_UNUSED(scancode);
  }

  virtual void key_up_event(long scancode)
  {
    VSX_UNUSED(scancode);
  }

  virtual void mouse_move_passive_event(int x, int y)
  {
    VSX_UNUSED(x);
    VSX_UNUSED(y);
  }

  virtual void mouse_move_event(int x, int y)
  {
    VSX_UNUSED(x);
    VSX_UNUSED(y);
  }

  // buttons: 0 = left, 1 = middle, 2 = right
  virtual void mouse_down_event(unsigned long button, int x, int y)
  {
    VSX_UNUSED(button);
    VSX_UNUSED(x);
    VSX_UNUSED(y);
  }

  // buttons: 0 = left, 1 = middle, 2 = right
  virtual void mouse_up_event(unsigned long button, int x, int y)
  {
    VSX_UNUSED(button);
    VSX_UNUSED(x);
    VSX_UNUSED(y);
  }

  // -1 to -5 or whatever up to +1
  virtual void mousewheel_event(float diff, int x, int y)
  {
    VSX_UNUSED(diff);
    VSX_UNUSED(x);
    VSX_UNUSED(y);
  }
};
