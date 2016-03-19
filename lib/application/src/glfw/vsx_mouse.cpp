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

#include "vsx_mouse.h"
#include "GL/glfw.h"

void vsx_mouse::set_cursor(int id) {
  cursor = id;
}

void vsx_mouse::show_cursor()
{
  // This causes problems with knobs & arcballs, mouse visibility is disabled for now
  //glfwEnable(GLFW_MOUSE_CURSOR);
  //glfwSetMousePos((int)(position.x), (int)(position.y));
}

void vsx_mouse::hide_cursor()
{
  // This causes problems with knobs & arcballs, mouse visibility is disabled for now
  //  glfwDisable(GLFW_MOUSE_CURSOR);
}


void vsx_mouse::set_cursor_pos(float x, float y)
{
  position.set(x,y);
  glfwSetMousePos((int)(position.x), (int)(position.y));
}

vsx_mouse::vsx_mouse()
{
  cursor = 0;
}
