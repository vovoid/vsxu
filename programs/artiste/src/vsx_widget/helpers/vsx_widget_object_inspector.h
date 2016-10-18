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

#ifndef VSX_WIDGET_OBJECT_INSPECTOR_H
#define VSX_WIDGET_OBJECT_INSPECTOR_H


#include <vsx_widget_window.h>

class vsx_window_object_inspector : public vsx_widget_window {
  vsx_widget *inspected;
  vsx_widget *label1,*label2,*label3;

  //-- component inspection
  vsx_widget *component_rename_edit;
  vsx_widget *component_rename_button;
  
  std::vector<vsx_widget*> component_list;
  std::unique_ptr<vsx_texture<>> texture;
  bool texture_loaded;
  vsx_string<>filename_loaded;
  int view_type;
  vsx_vector3<> pos_, size_;
  float vsx_status_timer;
public:

  void draw_2d();
  void draw();
  void command_process_back_queue(vsx_command_s *t);

  void init()
  {
    if (!init_run)
      vsx_widget_window::init();
  }
  void load_file(vsx_string<>filename);
  
  virtual void init_children() {}

  void unload();
  
  void show();

  vsx_window_object_inspector();
};



#endif
