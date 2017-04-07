/**
* Project: VSXu Profiler - Data collection and data visualizer
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

// engine
#include <string/vsx_string.h>
#include <vsx_command_list.h>
#include <time/vsx_timer.h>
#include <debug/vsx_error.h>
#include <profiler/vsx_profiler.h>
#include <vsx_gl_global.h>

// engine_graphics
#include <vsx_font.h>
#include <vsx_gl_state.h>

// generic
#include <vsx_version.h>
#include <vsx_platform.h>

// local
#include "profiler_application.h"
#include "profiler_desktop.h"


// global vars
vsx_string<>fpsstring = "VSX Ultra "+vsx_string<>(VSXU_VERSION)+" - 2012 Vovoid";

// from the perspective (both for gui/server) from here towards the tcp thread
vsx_widget_desktop *desktop = 0;
vsx_font myf;
vsx_command_list system_command_queue;

void load_desktop_a();


#include "vsx_profiler_draw.h"
vsx_profiler_draw* my_draw = 0x0;

void load_desktop_a()
{
  desktop = new vsx_widget_desktop;
  desktop->system_command_queue = &system_command_queue;
  desktop->init();
}


void app_init()
{
  my_draw = new vsx_profiler_draw();

  //---------------------------------------------------------------------------
  vsx::filesystem filesystem;
  myf.load( vsx_string<>(PLATFORM_SHARED_FILES) + "font/font-ascii_output.png", &filesystem );
}

/*
 print out help texts
*/
void app_print_cli_help()
{
  printf(
         "VSXu Profiler command syntax:\n"
         "  -f             fullscreen mode\n"
         "  -s 1920,1080   screen/window size\n"
         "  -p 100,100     window posision\n"
         "\n"
        );
}


void app_pre_draw()
{
  vsx_command_s *c = 0;
  while ( (c = system_command_queue.pop()) )
  {
    vsx_string<>cmd = c->cmd;
    if (cmd == "system.shutdown")
    {
      exit(0);
    }
    c = 0;
  }
}

bool app_draw()
{
  if (!my_draw)
    VSX_ERROR_RETURN_V("my draw is 0x0", false);

  my_draw->draw();
  return true;
}

void app_char(long key)
{
  if (!desktop)
    return;

  desktop->input_key_down(key);
}

void app_key_down(long key)
{
  if (!desktop)
    return;

  desktop->input_key_down( key);
}

void app_key_up(long key)
{
  if (!desktop)
    return;

  desktop->input_key_up(key);
}

void app_mouse_move_passive(int x, int y)
{
  if (!desktop)
    return;
  desktop->input_mouse_move_passive((float)x,(float)y);
}

void app_mouse_move(int x, int y)
{
  if (!desktop)
    return;

  desktop->input_mouse_move(
    clamp(x, 0, vsx_gl_state::get_instance()->viewport_get_width() ),
    clamp(y, 0, vsx_gl_state::get_instance()->viewport_get_height() )
  );
}

void app_mouse_down(unsigned long button,int x,int y)
{
  if (!desktop)
    return;

  desktop->input_mouse_down(x,y,button);
}

void app_mouse_up(unsigned long button,int x,int y)
{
  if (!desktop)
    return;

  desktop->input_mouse_up(x,y,button);
}

void app_mousewheel(float diff,int x,int y)
{
  if (!desktop)
    return;

  desktop->input_mouse_move_passive((float)x,(float)y);
  desktop->input_mouse_wheel(diff);
}

