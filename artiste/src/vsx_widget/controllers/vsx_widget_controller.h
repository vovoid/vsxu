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

#ifndef VSX_NO_CLIENT
#ifndef vsx_widget_slider_H
#define vsx_widget_slider_H

#define sizeunit 0.014f //the basic building block unit multiplier

//#############################''''''##################################
//############################  Knob  #################################
//#############################......##################################

class vsx_widget_knob : public vsx_widget_base_controller 
{
public:
  vsx_widget* editor;
  float multiplier;

  virtual void on_delete();
  vsx_widget_knob();
  void init();

  void vsx_command_process_b(vsx_command_s *t);
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords);
  virtual void draw();
};

class vsx_widget_slider : public vsx_widget_base_controller
{
public:
  float amp, ofs;
  vsx_vector clickpos,deltamove,remPointer,remWorld;
  vsx_color marker_color;
  float handlesize;
  vsx_widget* hint;
  vsx_widget* editor;
  void init(); 
  void draw();
  void cap_value();
  int inside_xy_l(vsx_vector &test, vsx_vector &global);
  void vsx_command_process_b(vsx_command_s *t);
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords);

  double getYPos();
  vsx_widget_slider();
};


class vsx_widget_channel : public vsx_widget_base_controller 
{
public:
  vsx_widget* amp;
  vsx_widget* offset;
  vsx_widget* slider;
  vsx_widget* label;
  vsx_widget_channel();
  int inside_xy_l(vsx_vector &test, vsx_vector &global) { if (owned) return 0; else return vsx_widget::inside_xy_l(test,global);};
  void smooth(float newval);
  void init();
  void draw();
  void vsx_command_process_b(vsx_command_s *t);
};

class vsx_widget_mixer : public vsx_widget_base_controller 
{
public:
  vsx_string capmaxv_s, capminv_s;
  std::vector<vsx_widget*> mixers;
  int nummixers;
  vsx_widget_mixer();
  void smooth(float newval);
  virtual void get_in_param_spec(std::pair<vsx_string,vsx_string> parampair);
  void init();
  void draw();
  void vsx_command_process_b(vsx_command_s *t);
};

//#############################''''''''''##################################
//############################  3D label  #################################
//#############################..........##################################

/*class vsx_widget_3d_label : public vsx_widget
{
vsx_string cursor;
public:
  int numchars;
	int halign;
	vsx_color bgcolor;
	bool editable,editing,changed;
	int cursorpos;
	vsx_string editvalue,allowedchars;
  bool inside_xyd(vsx_vector world, vsx_vector screen);
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  bool event_key_down(signed long key, bool alt, bool ctrl, bool shift);	
  vsx_widget_3d_label();
  ~vsx_widget_3d_label();
	
	void draw();
};

*/

class vsx_widget_dialog:public vsx_widget_base_controller
{
public:
  void init();
	void draw();
  virtual void get_in_param_spec(std::pair<vsx_string,vsx_string> parampair); //used in param retrieval for letting child classes deal with unothodox params
};  


#endif
#endif
