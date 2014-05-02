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

#ifndef VSX_WIDGET_POPUP_MENU_H
#define VSX_WIDGET_POPUP_MENU_H

#include "vsx_widget.h"

class vsx_widget_popup_menu : public vsx_widget
{
  bool oversub;
public:
  vsx_command_list menu_items;
  int id_over;
  int over;
  vsx_command_s *current_command_over;
  double row_size;
  void init();
  void command_process_back_queue(vsx_command_s *t);
  void add_commands(vsx_command_s* command);

  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords);
  void i_draw();
  vsx_widget_popup_menu() { target_size.x = size.x = 0.4; }
  void on_delete();
};


#endif // VSX_WIDGET_POPUP_MENU_H
