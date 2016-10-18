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

#ifndef VSX_WIDGET_ASSISTANT_H
#define VSX_WIDGET_ASSISTANT_H



class vsxu_assistant : public vsx_widget {
  vsx_widget *inspected;
//-----------------------------------------------
  unsigned long timestamp;
  bool reload;
  bool auto_;
  float tt,alpha;
  vsx_widget* pager;
  std::unique_ptr<vsx_texture<>> texture;
  vsx_vector3<> pos_, size_;
  vsx_string<>text;
  vsx_widget* cur_focus;
  float size_multiplier;
  vsx_vector3<> clickpoint, endpoint, cursize, endsize;
  std::vector <vsx_string<> > course;
  
  float temp_size;
  
public:
  void temp_hide();
  void temp_show();
  void i_draw();
  void command_process_back_queue(vsx_command_s *t);
  void init();
  void reinit();
  void on_delete();
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void toggle_size();
  vsxu_assistant();
};

#endif
