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

#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "vsx_gl_global.h"
#include "vsx_command.h"
#include "vsx_param.h"
#include <module/vsx_module.h>
#include <texture/vsx_texture.h>
#include <time/vsx_timer.h>
#include <filesystem/vsx_filesystem.h>
#include "vsx_font.h"
#include <vsx_version.h>
#include <vsx_engine.h>
#include <vsx_module_list_factory.h>
#include <debug/vsx_error.h>
#include <debug/vsx_backtrace.h>
#include <vsx_data_path.h>

#include "log/vsx_log_a.h"

#include <vsx_widget.h>

#include "vsx_widget_window.h"
#include "artiste_desktop.h"
#include <vsx_command_client_server.h>
#include "vsx_widget/server/vsx_widget_server.h"
#include "vsx_widget/module_choosers/vsx_widget_module_chooser.h"
#include "vsx_widget/helpers/vsx_widget_object_inspector.h"
#include "vsx_widget/helpers/vsx_widget_preview.h"
#include "logo_intro.h"
//#define NO_INTRO
#include "artiste_application.h"
#include "vsxg.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <profiler/vsx_profiler_manager.h>
#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
#include <time.h>
#endif


void app_char(long key)
{
}

void app_key_down(long key)
{
  req(desktop);
}

void app_key_up(long key)
{
  req(desktop);
  desktop->key_up(key,app_alt, app_ctrl, app_shift);
}

void app_mouse_move_passive(int x, int y) {
  req(desktop);
  desktop->mouse_move_passive((float)x,(float)y);
}

void app_mouse_move(int x, int y)
{
  req(desktop);
  desktop->mouse_move( CLAMP(x, 0, vsx_gl_state::viewport_get_width()), CLAMP(y, 0, vsx_gl_state::viewport_get_height()));
}

void app_mouse_down(unsigned long button,int x,int y)
{
  req(desktop);
  desktop->mouse_down(x,y,button);
}

void app_mouse_up(unsigned long button,int x,int y)
{
  req(desktop);
  desktop->mouse_up(x,y,button);
}

void app_mousewheel(float diff,int x,int y)
{
  req(desktop);
  desktop->mouse_wheel(diff);
}

