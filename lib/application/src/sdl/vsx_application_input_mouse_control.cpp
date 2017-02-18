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

#include <vsx_application_input_mouse_control.h>
#include "vsx_application_sdl_window_holder.h"
#include <vsx_platform.h>
#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

void vsx_application_mouse_control::set_cursor(int id) {
  cursor = id;
}

void vsx_application_mouse_control::show_cursor()
{
  SDL_ShowCursor(1);
}

void vsx_application_mouse_control::hide_cursor()
{
  SDL_ShowCursor(0);
}


void vsx_application_mouse_control::set_cursor_pos(float x, float y)
{
  int w;
  int h;
  SDL_GetWindowSize(vsx_application_sdl_window_holder::get_instance()->window, &w, &h);
  SDL_WarpMouseInWindow(vsx_application_sdl_window_holder::get_instance()->window, (int)(x*(float)w), (int)(y*(float)h));
}

void vsx_application_mouse_control::set_cursor_pos_screen(float x, float y)
{
  SDL_WarpMouseInWindow(vsx_application_sdl_window_holder::get_instance()->window, (int)x, (int)y);
}

vsx_application_mouse_control::vsx_application_mouse_control()
{
  cursor = 0;
}
