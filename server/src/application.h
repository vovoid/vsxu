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

#ifndef VSX_APPLICATION_H
#define VSX_APPLICATION_H

#include <sys/time.h>


// TM wrapper
#include "vsx_tm.h"

// GL state
#include "vsx_gl_state.h"

//extern vsx_engine* vme;
//#define vsxu_version "VSXu 0.1.14 beta"
//extern vsx_string vsxu_name;
//extern vsx_string vsxu_version;
//extern char* vsxu_ver;


extern bool app_ctrl;
extern bool app_alt;
extern bool app_shift;
extern bool dual_monitor;
extern vsx_tm* tm;
extern vsx_gl_state gl_state;

extern vsx_argvector app_argv;


void app_init(int id);

void app_print_cli_help();

void app_pre_draw();

bool app_draw(int id);

void app_char(long key);
void app_key_down(long key);
void app_key_up(long key);

void app_mouse_move_passive(int x, int y);
void app_mouse_move(int x, int y);

// buttons: 0 = left, 1 = middle, 2 = right
void app_mouse_down(unsigned long button,int x,int y);
void app_mouse_up(unsigned long button,int x,int y);

// -1 to -5 or whatever up to +1
void app_mousewheel(float diff,int x,int y);

#endif
