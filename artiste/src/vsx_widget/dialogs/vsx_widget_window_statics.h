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

#ifndef VSX_WINDOW_STATICS_H
#define VSX_WINDOW_STATICS_H

#include "window/vsx_widget_window.h"

class dialog_query_string : public vsx_widget_window {
  vsx_string i_hint;
  //std::map<vsx_string, vsx_widget*> fields;
  vsx_avector<vsx_widget*> edits;
public:
  vsx_string extra_value; // appended at the end of the command
  vsx_widget *edit1;
  void command_process_back_queue(vsx_command_s *t);
  
  void set_value(vsx_string value);
  void show(vsx_string value);
  void show();
  void init() {};
  void set_allowed_chars(vsx_string ch);
  dialog_query_string(vsx_string window_title, vsx_string in_fields = "");
};

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

class dialog_messagebox : public vsx_widget_window {
public:
  bool allow_resize_y;

  void command_process_back_queue(vsx_command_s *t) {
    if (t->cmd == "ok") {
      k_focus = parent;
      a_focus = parent;
      _delete();
    }
    visible = 0;
  }
  void init() {
    if (!init_run) vsx_widget_window::init();
  }
  
  virtual void init_children() {
  }
  
  void show(vsx_string value)
  {
    VSX_UNUSED(value);

    show();
  }
  

  virtual bool event_key_down(signed long key, bool alt, bool ctrl, bool shift)
  {
    VSX_UNUSED(key);
    VSX_UNUSED(alt);
    VSX_UNUSED(ctrl);
    VSX_UNUSED(shift);

    k_focus = parent;
    a_focus = parent;
    _delete();
    return true;
  }

  void show() {
    visible = 1;
    pos.x = 0.5-size.x/2;
    pos.y = 0.5-size.y/2;
  }

  dialog_messagebox(vsx_string title_,vsx_string hint);
};

#endif
