/**
* Project: VSXu: Realtime modular visual programming language, music/audio visualizer.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Vovoid Media Technologies AB Copyright (C) 2003-2014
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

#include "vsx_widget_window.h"

#include "widgets/vsx_widget_base_edit.h"
#include "widgets/vsx_widget_button.h"
#include "widgets/vsx_widget_label.h"

class dialog_query_string
  : public vsx_widget_window
{
  vsx_string<>i_hint;
  //std::map<vsx_string<>, vsx_widget*> fields;
  vsx_nw_vector<vsx_widget*> edits;
  vsx_nw_vector<vsx_widget*> labels;

public:

  vsx_string<> extra_value; // appended at the end of the command
  vsx_widget *edit1;

  // if this returns false, don't send message, values have been handled in the event
  std::function<bool(vsx_nw_vector< vsx_string<> >&)> on_ok = [](vsx_nw_vector< vsx_string<> >& values){ return true; };
  void command_process_back_queue(vsx_command_s *t);
  void set_title(vsx_string<> new_title);
  void set_label(size_t index, vsx_string<> title);
  void set_value(vsx_string<>value);
  void show(vsx_string<>value);
  void show();
  void init() {}
  void set_allowed_chars(vsx_string<>ch);
  dialog_query_string(vsx_string<> window_title, vsx_string<>in_fields = "");
};
