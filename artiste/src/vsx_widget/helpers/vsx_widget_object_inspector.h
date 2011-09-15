/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies Copyright (C) 2003-2011
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

#ifndef VSX_NO_CLIENT
#ifndef VSX_WIDGET_OBJECT_INSPECTOR_H
#define VSX_WIDGET_OBJECT_INSPECTOR_H


class vsx_window_object_inspector : public vsx_widget_window {
  vsx_widget *inspected;
  vsx_widget *label1,*label2,*label3;
//-- component inspection
  vsx_widget *component_rename_edit;
  vsx_widget *component_rename_button;
//-- chooser inspection
  //vsx_widget *chooser_
  
//-----------------------------------------------
  std::vector<vsx_widget*> component_list;
#ifndef VSXU_PLAYER  
  vsx_texture texture;
#endif
  bool texture_loaded;
  vsx_string filename_loaded;
  int view_type;
  vsx_vector pos_, size_;
  float vsx_status_timer;
public:
  void draw_2d();
  void draw();
  void vsx_command_process_b(vsx_command_s *t);
  void init() {
    if (!init_run) vsx_widget_window::init();
  }
  void load_file(vsx_string filename);
  void unload();
  
  virtual void init_children() {
  }
  
  void show();

  vsx_window_object_inspector();
};



#endif
#endif
