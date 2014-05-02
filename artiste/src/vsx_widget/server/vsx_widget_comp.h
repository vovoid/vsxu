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

#ifndef VSX_WIDGET_COMP_H
#define VSX_WIDGET_COMP_H

#include "vsx_widget.h"
#include "vsx_widget_connector_info.h"

#define COMPONENT_MOVE 1
#define COMPONENT_SCALE 2

class vsx_widget_component : public vsx_widget {
// for making it "ethereal" when putting it in a macro
  //vsx_font myf;
  vsx_vector<> message_pos;
  vsx_texture mtex_blob;
public:
  bool macro;
  bool scaled;
  int deleting;
  // for saving macros
  vsx_widget* name_dialog;
  vsx_command_list macro_commands;

  // is this critical to the vme? if so, can't be deleted.
  bool internal_critical;

  bool vsxl_filter;
  vsx_vector<> real_pos;
  static bool ethereal_all;
  bool ethereal;
  static bool show_titles;

  bool selected; // is this in the selected-list?
  bool not_movable; // may be moved or not?

  int anchor_order[2]; // to keep track of how many anchors have been created as children
  vsx_widget* server;
  vsx_string component_type;
  vsx_string real_name; // the real name - that is, macro names omitted.
  vsx_string parent_name; // in case part of a macro this will be set to "macroxyz."
  vsx_string module_path;

  // message from server
  vsx_string message;
  float message_time;
  vsx_texture mtex;

  // macro specific stuff
  bool open; // for containers - macros.
  vsx_vector<> old_size; // the previous size to return to..
  vsx_texture mtex_overlay;
  float macro_overlay_opacity;

  // info used when moving
  bool is_moving;



  // to keep track of the anchors matched against the names in the vsxu server (not the gui component)
  // for easy access.
  std::map<vsx_string, vsx_widget*> p_l_list_in;
  std::map<vsx_string, vsx_widget*>::const_iterator p_l_list_in_iter;
  std::map<vsx_string, vsx_widget*> p_l_list_out;
  std::map<vsx_string, vsx_widget*>::const_iterator p_l_list_out_iter;
  std::map<vsx_string, vsx_widget*> t_list; // temporary list for rescuing of re-inited anchors

  void command_process_back_queue(vsx_command_s *t);
  void vsx_command_process_f();
  void init();
  void reinit();
  void draw();

  void macro_fix_anchors(bool override = false);
  void macro_toggle();
  void macro_close();
  void before_delete();
  void begin_delete();
  void perform_delete();
  void rename(vsx_string new_name,bool partial_name = true);
  void rename_add_prefix(vsx_string prefix, vsx_string old_name_remove = "");
  void get_connections_in_abs(std::list<vsx_widget_connector_info*>* mlist);
  void get_connections_out_abs(std::list<vsx_widget_connector_info*>* mlist);
  void hide_all_complex_anchors_but_me(vsx_widget* open_anchor);
  void disconnect_abs();
  vsx_string alias_get_unique_name_in(vsx_string base_name, int tried = 0); // find a free name for the alias
  vsx_string alias_get_unique_name_out(vsx_string base_name, int tried = 0); // find a free name for the alias


  // to keep track of our position and send it to the server
  float move_time;

  int inside_xy_l(vsx_vector<> &test, vsx_vector<> &global);
  int transform_state;
  void move(double x, double y, double z);
  void resize_to(vsx_vector<> to_size);
  void server_move_notify();
  void server_scale_notify();
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords);
  void event_mouse_wheel(float y);

  bool event_key_down(signed long key, bool alt, bool ctrl, bool shift);

};

#endif
