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

#include <math/vector/vsx_vector3_helper.h>
#include "vsx_widget.h"
#include "vsx_widget_dllimport.h"
#include <gl_helper.h>

class WIDGET_DLLIMPORT vsx_widget_popup_menu : public vsx_widget
{
  bool oversub = false;

public:

  vsx_command_list menu_items;
  vsx_command_s* current_command_over = 0x0;
  int id_over = 0;
  int over = 0;

  std::function<void(vsx_command_s&)> on_selection = [](vsx_command_s& command){};


  float row_size = 0.02f;

  vsx_widget_popup_menu();
  void init();
  void clear();
  void init_extra_commands();
  void show();
  void row_size_by_font_size(float font_size);
  void command_process_back_queue(vsx_command_s *t);
  void add_commands(vsx_command_s* command);
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords);
  void i_draw();
  void on_delete();

};
