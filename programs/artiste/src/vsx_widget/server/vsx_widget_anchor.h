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

#ifndef VSX_WIDGET_ANCHOR_H
#define VSX_WIDGET_ANCHOR_H

#include "vsx_widget.h"
#include "vsx_widget_connector_info.h"

// VSX_WIDGET_ANCHOR ************************************************************************************************
// VSX_WIDGET_ANCHOR ************************************************************************************************
// VSX_WIDGET_ANCHOR ************************************************************************************************
// VSX_WIDGET_ANCHOR ************************************************************************************************
class vsx_widget_anchor : public vsx_widget
{
  vsx_widget *temp_drag_connector;
  vsx_string<>display_value;
  vsx_nw_vector< vsx_string<> > enum_items;
  bool draw_glow;
  void set_glow(bool glow_status);
  //
  static bool drag_status;
  static bool clone_value;
  float drag_size_mul;
  static vsx_vector3<> drag_position;
  static vsx_widget_anchor* drag_anchor;
  static vsx_widget_anchor* drag_clone_anchor;
  float ax,ay,sx,sy;
  vsx_widget_coords drag_coords;

  std::unique_ptr<vsx_texture<>> mtex_d;
  std::unique_ptr<vsx_texture<>> mtex_blob;
  std::unique_ptr<vsx_texture<>> mtex_blob_small;

  vsx_vector3<> pp;
  vsx_vector3<> myf_size, myf_pos;
  void delete_controllers();
  void init_menu(bool include_controllers);

  void enumerate_children_get(int override = -1);

  vsx_widget* search_anchor;

public:
  float text_size;
  float display_value_t;
  // aliasing stuff
  bool alias;
  vsx_string<>alias_for_component;
  vsx_string<>alias_for_anchor;
  vsx_widget_anchor* alias_parent;
  vsx_widget_anchor* alias_owner;
  //
  vsx_widget* component;
  int anchor_order[2]; // to keep track of how many anchors have been created as children

  bool vsxl_filter; // is this param filtered?
  bool sequenced; // is this param sequenced?
  bool forbid_connections;

  int a_order; // in wich position of our parent we are - 0 is the first, 1 is the second and so on
  int io; // -1 = in, 1 = out  - the type (and position) of the anchor
  bool tree_open; // true = open, false = closed
  bool conn_open; // is the connections showing?
  vsx_string<> p_spec; // original specification (for future use)
  vsx_string<> p_type;
  vsx_string<> p_type_suffix; // type=[suffix] - extra information for the type, used for enumerations etc.
  vsx_string<> p_desc; // a nice description
  vsx_string<> p_def; // the whole string that identifies this and its children
  vsx_string<> default_controller;
  // anchors connected to us
  std::list <vsx_widget*> connectors;
  std::list <vsx_widget*> connections;

  std::map<vsx_string<>,vsx_string<> > dialogs;
  std::map<vsx_string<>,vsx_string<> > options;

  virtual void init();
  void reinit();

  virtual void command_process_back_queue(vsx_command_s *temp_drag_connector);

  bool connect(vsx_widget* other_anchor);

  void update_connection_order();

  void get_connections_abs(vsx_widget* base, std::list<vsx_widget_connector_info*>* mlist);
  void disconnect_abs();
  void param_connect_abs(vsx_string<>c_component, vsx_string<>c_param, int c_order);
  void connect_far(vsx_widget_anchor* src, int corder, vsx_widget_anchor* referrer = 0);
  vsx_widget_anchor* alias_to_level(vsx_widget_anchor* dest);

  void event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords);
  void event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords);
  void event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  bool event_key_down(uint16_t key);


  void before_delete();
  void on_delete();
  void toggle(int override = 0); // 1 = close, 2 = open
  void get_value();
  void fix_anchors();
  bool get_drag_status();
  void clone_our_value_to_other_anchor_via_server( const vsx_widget_anchor* other_anchor );


  virtual void hide_children() {
    vsx_widget::hide_children();
    tree_open = false;
  };

  virtual vsx_vector3<> get_pos_p();

  virtual void pre_draw();
  virtual void draw();
  vsx_widget_anchor();
};

#endif

