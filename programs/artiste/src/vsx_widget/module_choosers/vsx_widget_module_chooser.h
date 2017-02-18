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

#ifndef VSX_WIDGET_MODULE_CHOOSER_H
#define VSX_WIDGET_MODULE_CHOOSER_H

extern vsx_command_list module_chooser_colors;
extern std::map<vsx_string<>,vsx_color<> > mc_colors;
extern std::map<vsx_string<>,vsx_color<> > mc_r_colors;

const static double EPSILON = 1.0E-10; // epsilon
const static int ZONE_LENGTH = 4; // size of the zone
const int LINE    = 0;       // draw a line
const int ARC     = 1;       // draw an arc


#include "htcoord.h"
#include "htgeodesic.h"
#include "htsector.h"
#include "htnode.h"
#include "htmodel.h"


#include "vsx_widget_hyperbolic_tree.h"

class vsx_widget_ultra_chooser : public vsx_widget {
 HTModel* mymodel;
 vsx_widget_hyperbolic_tree* treedraw;
 
 HTCoord startpoint, endpoint, clickpoint;
 bool show_, hide_;
 bool first;
 bool moved;
 
 HTCoord anim_startpoint, anim_endpoint, anim_curpoint;
 vsx_vector3<> v_anim_startpoint, v_anim_endpoint;
 double anim_x;
 
 bool animating;
 bool drag_module;
 bool drag_dropped;
 bool allow_move;
 vsx_vector2f drag_pos;
 vsx_vector2f drop_pos;
 vsx_string<>macro_name;
 vsx_vector2f tooltip_pos;
 int draw_tooltip;
 vsx_string<>tooltip_text;
 
 vsx_widget* name_dialog;
 float interpolation_speed;
 
 vsx_widget* object_inspector;
 void cancel_drop();

public:
  HTNode* module_tree;
  
  int mode; // 0 = ordinary, 1 = double-click-returns-string
  vsx_widget* command_receiver;
  vsx_string<>command;
  vsx_string<>message; // friendly message showing the user what to do
  
  vsx_widget* server;
  
  double sx,sy,ex,ey; // internal coordinates
  double smx, smy; // coordinate where mouse has been clicked
  double mdx, mdy; // delta mouse move
  int inside_xy_l(vsx_vector3<> &test, vsx_vector3<> &global);
  void command_process_back_queue(vsx_command_s *t);
  
  void show();
  void hide();
  void build_tree();
  void center_on_item(vsx_string<>target);
  
  void init();
  void reinit();
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords);
  void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords);
  bool event_key_down(uint16_t key);
  void draw_2d();
  vsx_widget_ultra_chooser();
  void on_delete();
};

#endif
