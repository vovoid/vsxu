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

#include <vsx_widget_window.h>

class vsx_widget_seq_pool_manager : public vsx_widget_window {
  vsx_string<>i_hint;

  vsx_widget* server;


  // widgets
  vsx_widget *edit;
  vsx_widget *search;
  vsx_widget *button_add;
  vsx_widget *button_del;
  vsx_widget *button_clone;
  vsx_widget *button_import;
  vsx_widget *button_export;
  vsx_widget *button_propagate;
  vsx_widget *button_toggle_edit;
  // dialogs
  vsx_widget *name_dialog;
  vsx_widget *clone_name_dialog;
  vsx_widget *import_name_dialog;
  vsx_widget *export_name_dialog;
  // sequencer
  vsx_widget *sequencer;

public:
  void command_process_back_queue(vsx_command_s *t);

  void show(vsx_string<>value);
  void show();
  void init();
  void i_draw();
  void event_text(wchar_t character_wide, char character);
  bool event_key_down(uint16_t key);
  void set_server(vsx_widget* serv);
  vsx_widget_seq_pool_manager();
};
