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

#ifndef VSX_MODULE_CHOOSER_LIST_H
#define VSX_MODULE_CHOOSER_LIST_H

#include <vsx_widget_window.h>


class vsx_module_chooser_list : public vsx_widget_window {
  vsx_string<>i_hint;
  vsx_widget *widget_list;
  vsx_widget *widget_search;
  
  std::vector <vsx_string<> > i_rows;

public:
  void command_process_back_queue(vsx_command_s *t);
  
  void show(vsx_string<>value);
  void show();
  void init() {}
  void i_draw();
  bool event_key_down(uint16_t key);
  void event_text(wchar_t character_wide, char character);
	void add_item(vsx_string<>name,vsx_module_specification* m_info);
	void build_tree();
	void set_server(vsx_widget* serv);
  vsx_module_chooser_list();
};

#endif
