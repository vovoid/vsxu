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

#ifndef VSX_WIDGET_SEQ_CHAN_H
#define VSX_WIDGET_SEQ_CHAN_H

#include "sequencer/vsx_widget_sequence.h"
#include "vsx_widget_param_sequence_item.h"

#define VSX_WIDGET_SEQ_CHANNEL_TYPE_PARAMETER 0
#define VSX_WIDGET_SEQ_CHANNEL_TYPE_MASTER 1

#define SEQ_CHAN_BOX_SIZE 0.0014f
#define SEQ_CHAN_BOX_SIZE05 0.0007f


class vsx_widget_seq_channel : public vsx_widget
{

  float view_time_start, view_time_end, cur_time;

  float clicked_time_pos;

  int param_type; // module parameter type
  int index_hit; // for multiple-variable types such as float3, float4
  int extra_hit; // handles for bezier splines
  int index_count; // how many parts are this type made up of?
  int display_exclusive; // dim other than this

  float clicked_item_x_diff;

  void toggle_exclusive(int value) {
    if (display_exclusive == value) { display_exclusive = 0; return; }
    display_exclusive = value;
  }

  // supply a center coordinate
	inline float pos_to_time(float x) {
		return 	(x / size.x + 0.5f)
							* (view_time_end - view_time_start)
							+ view_time_start;
	}
/*
 * B = view_time_end - view_time_start

	t = (x / size.x + 0.5f) * B + view_time_start

  t - view_time_start = (x / size.x + 0.5f) * B

  (t - view_time_start) / B = x / size.x + 0.5f

  (t - view_time_start) / B - 0.5f = x / size.x

  x = size.x * ((t - view_time_start) / B - 0.5f)

	*/

	// returns a center coordinate
	inline float time_to_pos(float t) {
		float B = view_time_end - view_time_start;
		return 	size.x * ( (t-view_time_start) / B - 0.5f);
	}

	vsx_vector passive_mouse_pos;
	vsx_string passive_time;
	vsx_string passive_value;
  vsx_vector parentpos;
  std::vector<vsx_widget_param_sequence_item> items; // the actual sequence
  int cur_x, cur_y;
  int scroll_y;
  int num_rows;
  bool edit_mode;
  vsx_string edit_str;
  void server_message(vsx_string msg,vsx_string data) {
    command_q_b.add_raw(msg+" "+channel_name+" "+param_name+" "+data);
    parent->vsx_command_queue_b(this);
  }
  void i_remove_line(long i_td);

  void send_parent_dump();

  // temp values for color calculations
  static vsx_color col_temp_1;
  static vsx_color col_temp_2;
  //vsx_font myf2;

  vsx_widget* menu_interpolation;
  vsx_widget* value_dialog;

  static vsx_texture* mtex_blob;

public:
	bool hidden_by_sequencer;
  bool is_controller; // true if to act as a controller
  vsx_widget_sequence_editor* owner;
  int channel_type; // 0 = parameter, 1 = master
  vsx_string channel_name; // if master, this is the channel name, if parameter sequence it's the module name
  vsx_string param_name;   // if master this is "[master]" otherwise the parameter name
  float totalsize;
  float ff;
  float levelstart;
  float y_start; // visual y range of the "functional view"
  float y_end;

  vsx_color graph_color; // line graph color
  vsx_color graph_oob_color; // out of bounds

  void init();

  float m_pos;
  int mouse_clicked_id;

  bool event_key_down(signed long key, bool alt, bool ctrl, bool shift);
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords);
  void event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords);
  void event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_wheel(float y);
  void command_process_back_queue(vsx_command_s *t);

  void i_draw();

  float totalysize;
  float dlx,dly;

  void draw_h_line(float y0,float r = 0.5, float g = 0.5, float b = 0.5, float a = 1);
  void draw_red_line(float t0, float t1, float y0);

  void draw_line(float t0, float y0, float t1, float y1, vsx_color& l_color);

  void draw_chan_box(float t0, float y0, float c_size = SEQ_CHAN_BOX_SIZE);
  void draw_selection_box(float t0, float y0);


  void drop_master_channel(vsx_widget_distance distance, vsx_widget_coords coords, vsx_string name);

  void set_view_time(float, float);
  void remove_master_channel_items_with_name(vsx_string name);

  vsx_widget_seq_channel()
    :
      owner(0),
      channel_type(VSX_WIDGET_SEQ_CHANNEL_TYPE_PARAMETER)
  {
  }

};

#endif
