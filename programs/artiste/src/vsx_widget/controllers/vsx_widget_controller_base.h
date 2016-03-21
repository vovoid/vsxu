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

#ifndef vsx_widget_base_controller_H
#define vsx_widget_base_controller_H

#include "vsx_widget.h"

const float ALMOST_ZERO = 0.00001;
const float sizeunit = 0.014f;

class vsx_widget_controller_base : public vsx_widget
{
  float r_time;
public:
  vsx_string<>command_suffix;
  vsx_widget* hint;

  int mouse_moves;
  double smx, smy; // coordinate where mouse has been clicked
  double mdx, mdy; // delta mouse move
  bool controlling, isolate;
  vsx_vector3<> parentpos;
  vsx_vector2f deltamove;
  vsx_vector2f remPointer;
  vsx_vector2f remWorld;
  float temp;
  std::map<vsx_string<>,vsx_string<> >* param_spec;
//----
  float snap(float newval);
  void set_value(float val,bool cap=true);

  // show the destination value visually?
  bool dest_value;

  vsx_string<>in_param_spec,target_param /*used if sending data to complex anchor*/;
  vsx_color<> bgcolor;
  float value, target_value, presentation_value, send_value;
  float old_snap_value;
  bool capmin, capmax;
  float capminv, capmaxv;
  bool lcapmin, lcapmax;
  float lcapminv, lcapmaxv;
  float min, max;
  float smoothness;
  //float sizeunit;
  float darkshade,lightshade;
  bool drawconnection;
  bool owned;

  vsx_widget_controller_base();
  ~vsx_widget_controller_base();
  virtual void generate_menu();
  virtual void init();
  virtual void before_delete();
  virtual void command_process_back_queue(vsx_command_s *t);
  virtual void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  virtual void event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  //virtual void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  virtual void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords);
  virtual void event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords);
  virtual void event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  virtual void cap_value();
  virtual void get_value();
  virtual void smooth(float newval);
  virtual void get_in_param_spec(std::pair<vsx_string<>,vsx_string<> > parampair); //used in param retrieval for letting child classes deal with unothodox params
  virtual void send_to_server();

  void set_command_suffix(vsx_string<>new_val)
  {
  	command_suffix = new_val;
  }


  //virtual void init();
  virtual void i_draw();
};

//#############################''''''''''##################################
//############################  3D hint   #################################
//#############################..........##################################
class vsx_widget_3d_hint : public vsx_widget
{
double startTime, endTime;

public:
	float secdelay;
  vsx_widget_3d_hint();
  ~vsx_widget_3d_hint();
  void draw();
  void popup();
};

#endif
