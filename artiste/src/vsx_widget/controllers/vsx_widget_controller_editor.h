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

#ifndef VSX_WIDGET_CONTROLLER_EDITOR_H
#define VSX_WIDGET_CONTROLLER_EDITOR_H

#include "vsx_widget_controller_base.h"

class vsx_widget_controller_editor : public vsx_widget_controller_base
{
  vsx_widget* button_close;
  vsx_widget* button_save;
  vsx_vector3<> pos_, size_;
  
  int longest_line;
  int characters_width;
  int characters_height;
  
  //vsx_widget* scrollbar_horiz;
  //vsx_widget* scrollbar_vertical;
  bool no_move; // if user clicked in the text or not
  
  void save();
  
  vsx_widget* panel;
  vsx_widget* editor;
  vsx_widget* e_log;

public:
  vsx_widget_controller_editor();
  void i_draw();

  vsx_string<>return_command; // will return a command like [return_command] [base64-encoded-text]
  vsx_string<>target_param;
  vsx_widget* return_component;
  int scroll_x, scroll_y;
  // vsx widget methods
  void init();
  
  virtual void command_process_back_queue(vsx_command_s *t);
  
  // custom methods
  bool load_text(vsx_string<>new_text);
};  


#endif

