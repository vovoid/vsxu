/**
* Project: VSXu: Realtime modular visual programming language, music/audio visualizer.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
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

#include <math/vector/vsx_vector2.h>
#include <math/vector/vsx_vector3.h>

#include "vsx_application_dllimport.h"

#define MOUSE_CURSOR_ARROW 0
#define MOUSE_CURSOR_HAND 1
#define MOUSE_CURSOR_IBEAM 2
#define MOUSE_CURSOR_SIZE 3
#define MOUSE_CURSOR_NS 4
#define MOUSE_CURSOR_WE 5
#define MOUSE_CURSOR_NESW 6
#define MOUSE_CURSOR_NWSE 7

class APPLICATION_DLLIMPORT vsx_application_mouse_control
{
  int cursor;
  bool visible;

public:

  void set_cursor(int id);
  void set_cursor_pos(float x, float y);

  void set_cursor_pos(vsx_vector2f& pos)
  {
    set_cursor_pos(pos.x, pos.y);
  }

  void set_cursor_pos_screen(float x, float y);

  void show_cursor();
  void hide_cursor();

  vsx_application_mouse_control();
};


