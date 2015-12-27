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

#ifndef INFO_OVERLAY_H_
#define INFO_OVERLAY_H_

#include "vsx_font.h"
#include "vsx_manager.h"
#include "vsx_timer.h"
#include <filesystem/vsx_filesystem.h>
#include <logo_intro.h>


class vsx_overlay {
	vsx_timer time2;
	vsx_font* myf;
	unsigned long frame_counter;
	unsigned long delta_frame_counter;
	float delta_frame_time;
	float delta_fps;
	float total_time;
	float title_timer;	
  float show_randomizer_timer;
	float scroll_pos;
	float dt;
	float intro_timer;
  vsx_manager_abs* manager;
  int help_id;
  float fx_alpha;
  bool first;
  vsx_logo_intro* intro;
	
public:
	
	vsx_overlay();
  void set_manager(vsx_manager_abs* new_manager);
	void reinit();
	void render();
  void set_help(int id);
  void print_help();
	void show_fx_graph();
  void show_randomizer_status();
};

#endif /*INFO_OVERLAY_H_*/
