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

  vsx_string<> organization_name;
  vsx_string<> application_name;
  vsx_string<> window_title;

public:

  /**
   * @brief init_no_graphics
   * Called before the graphics subsystem has been initialized
   * Things to initialize here:
   *  - managed singletons in correct orderf
   *  - filesystem constructs
   */
  virtual void init_no_graphics()
  {
  }

  /**
   * @brief init_graphics
   * Called after the graphics subsystem has been initialized
   * You can expect an graphics context to be present
   */
  virtual void init_graphics()
  {
  }

  /**
   * @brief uninit_graphics
   * Uninit while graphics subsystem (a context) is still present
   */
  virtual void uninit_graphics()
  {
  }

  /**
   * @brief uninit_no_graphics
   * Uninit after graphics subsystem has been torn down
   */
  virtual void uninit_no_graphics()
  {
  }


  virtual void pre_draw()
  {
  }

  virtual void draw()
  {
  }

  virtual void print_help()
  {
    vsx_printf(
      L"Window mode / settings: \n"
       "    -p x,y                    Window position \n"
       "    -f                        True fullscreen (video mode change) \n"
       "\n"
       "    -s [x-res]x[y-res]        Fullscreen resolution or window size \n"
       "                              Example: -s 1920x1080\n"
       "\n"
       "    -bl                       Borderless window. Not usable together\n"
       "                              with -f\n"
       "\n"
       "    -d [id]                   Show borderless fullwindow on display [id]\n"
       "    -dq                       Print how many displays are available and exit\n"
       "                              [id] is a value between 1 and number of displays\n"
       "    -gl_debug                 Enable OpenGL debug callback\n"
       "\n"
       "    Examples:\n"
       "      -f -s 1920x1080         Fullscreen, Full HD resolution\n"
       "      -d 2                    Show bordless window taking up all of  display #2\n"
       "      -s 500x500 -bl          Borderless window, 500 by 500 pixels\n"
       "\n"
       "\n"
       "Application settings:\n"
    );
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

  vsx_string<>& organization_get() {
    return organization_name;
  }

  vsx_string<>& application_name_get() {
    return application_name;
  }

  vsx_string<>& window_title_get() {
    return window_title;
  }

};
