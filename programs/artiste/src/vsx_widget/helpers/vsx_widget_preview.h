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

#ifndef VSX_WIDGET_PREVIEW_H
#define VSX_WIDGET_PREVIEW_H

#include <vsx_widget_window.h>

class vsx_window_texture_viewer : public vsx_widget_window {
  vsx_widget *inspected;
  vsx_widget *label1;
  vsx_widget *server;
  float frame_delta;
  float frame_count;
  float fps;
  void** engine;

  bool fullwindow;
  vsx_string<>modestring;
  vsx_string<>modestring_default;
  bool run;
  vsx_vector3<> pos_, size_;

public:

  void draw() {}
  void draw_2d();
  void init();
  void set_server(vsx_widget* new_server);

  void toggle_run();
  void toggle_fullwindow();
  bool get_fullwindow();
  bool* get_fullwindow_ptr();

  virtual bool event_key_down(uint16_t key);
  virtual void command_process_back_queue(vsx_command_s *t);

  vsx_window_texture_viewer();
};


#endif
