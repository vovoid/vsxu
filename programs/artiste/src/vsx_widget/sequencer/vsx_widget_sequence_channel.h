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

#include "sequencer/vsx_widget_sequence_editor.h"
#include "vsx_widget_param_sequence_item.h"
#include <math/vector/vsx_vector3_helper.h>

#define vsx_widget_sequence_channel_TYPE_PARAMETER 0
#define vsx_widget_sequence_channel_TYPE_MASTER 1

#define SEQ_CHAN_BOX_SIZE 0.0014f
#define SEQ_CHAN_BOX_SIZE05 0.0007f


class vsx_widget_sequence_channel : public vsx_widget
{

  float view_time_start, view_time_end, cur_time;

  float clicked_time_pos;

  int param_type; // module parameter type
  int index_hit; // for multiple-variable types such as float3, float4
  int extra_hit; // handles for bezier splines
  int index_count; // how many parts are this type made up of?
  int display_exclusive; // dim other than this
  bool can_move;

  float hover_value_pos;
  float hover_time_pos;

  float clicked_item_x_diff;

  void toggle_exclusive(int value)
  {
    if (display_exclusive == value) { display_exclusive = 0; return; }
    display_exclusive = value;
  }

  // supply a center coordinate
  inline float pos_to_time(float x)
  {
    return 	(x / size.x + 0.5f)
              * (view_time_end - view_time_start)
              + view_time_start;
  }

  // returns a center coordinate
  inline float time_to_pos(float t)
  {
    float B = view_time_end - view_time_start;
    return 	size.x * ( (t-view_time_start) / B - 0.5f);
  }

  vsx_vector3<> passive_mouse_pos;
  vsx_string<> passive_time;
  vsx_string<> passive_value;
  vsx_vector3<> parentpos;
  std::vector<vsx_widget_param_sequence_item> items; // the actual sequence
  int cur_x, cur_y;
  int scroll_y;
  int num_rows;
  bool edit_mode;
  vsx_string<>edit_str;


  // temp values for color calculations
  static vsx_color<> col_temp_1;
  static vsx_color<> col_temp_2;

  vsx_widget* menu_interpolation;
  vsx_widget* manual_value_input_dialog;
  vsx_widget* manual_time_input_dialog;

  static vsx_texture<>* mtex_blob;


  void server_message(vsx_string<>msg,vsx_string<>data)
  {
    command_q_b.add_raw(msg+" "+channel_name+" "+param_name+" "+data);
    parent->vsx_command_queue_b(this);
  }

  void i_remove_line(long i_td);

  void send_parent_dump();

  void recalculate_accum_times()
  {
    float accum_time = 0.0f;
    foreach(items, i)
    {
      items[i].set_accum_time( accum_time );
      accum_time += items[i].get_total_length();
    }
  }

  void set_value_bezier(vsx_string<> value, vsx_widget_param_sequence_item& pa)
  {
    vsx_nw_vector<vsx_string<> > pld_l;
    vsx_string<> pdeli_l = ":";
    vsx_string_helper::explode(value, pdeli_l, pld_l);
    pa.set_value( pld_l[0] );
    pa.set_handle1( vsx_vector3_helper::from_string<float>(pld_l[1]) );
    pa.set_handle2( vsx_vector3_helper::from_string<float>(pld_l[2]) );
  }

  void insert_absolute(float time, vsx_string<>& value, size_t interpolation)
  {
    recalculate_accum_times();

    vsx_widget_param_sequence_item pa;
    pa.set_interpolation( interpolation );
    pa.set_value( value );


    if (pa.get_interpolation() == VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_BEZIER)
      set_value_bezier( value, pa );

    bool in_between = false;
    for (auto it = items.begin(); it != items.end(); it++)
    {
      if ( (*it).get_accum_time() + (*it).get_total_length() < time )
        continue;

      float prev_total_length = (*it).get_total_length();
      (*it).set_total_length( time - (*it).get_accum_time() );

      pa.set_total_length( prev_total_length - (*it).get_total_length() );
      it++;
      items.insert(it, pa);
      in_between = true;
      break;
    }

    if (!in_between)
    {
      items[items.size() - 1].set_total_length( time - items[items.size() - 1].get_accum_time() );
      pa.set_total_length( 3.0 );
      items.push_back(pa);
    }

    recalculate_accum_times();
  }


public:
  bool hidden_by_sequencer;
  bool is_controller; // true if to act as a controller
  vsx_widget_sequence_editor* owner;
  int channel_type; // 0 = parameter, 1 = master
  vsx_string<> channel_name; // if master, this is the channel name, if parameter sequence it's the module name
  vsx_string<> param_name;   // if master this is "[master]" otherwise the parameter name
  float totalsize;
  float ff;
  float levelstart;
  float y_start; // visual y range of the "functional view"
  float y_end;
  float m_pos;
  int mouse_clicked_id;
  float totalysize;
  float dlx,dly;

  vsx_color<> graph_color; // line graph color
  vsx_color<> graph_oob_color; // out of bounds

  void init();

  void set_param_type(int n)
  {
    param_type = n;
  }



  bool event_key_down(uint16_t key);
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords);
  void event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords);
  void event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_wheel(float y);
  void command_process_back_queue(vsx_command_s *t);

  void i_draw();

  void draw_h_line(float y0,float r = 0.5, float g = 0.5, float b = 0.5, float a = 1);
  void draw_red_line(float t0, float t1, float y0);

  void draw_line(float t0, float y0, float t1, float y1, vsx_color<>& l_color);

  void draw_chan_box(float t0, float y0, vsx_string<> display_value, vsx_color<> box_color, float c_size = SEQ_CHAN_BOX_SIZE);
  void draw_selection_box(float t0, float y0);
  void create_keyframe(float time, vsx_string<> value, size_t interpolation_type);
  void get_keyframe_value(float time_start, float time_end, vsx_nw_vector< vsx_string<> >& values, vsx_ma_vector<float>& time_offsets, vsx_ma_vector<size_t>& interpolation_types);
  void clear_selection(float time_start, float time_end);

  void drop_master_channel(vsx_widget_distance distance, vsx_widget_coords coords, vsx_string<>name);

  void set_view_time(float, float);
  void remove_master_channel_items_with_name(vsx_string<>name);
  void set_bezier_time_aligned_handles();
  void align_bezier_time_handles();

  vsx_widget_sequence_channel()
    :
      view_time_start(0.0f),
      view_time_end(0.0f),
      cur_time(0.0f),
      clicked_time_pos(0.0f),
      param_type(0),
      index_hit(0),
      extra_hit(0),
      index_count(0),
      display_exclusive(0),
      hover_value_pos(0.0f),
      hover_time_pos(0.0f),
      clicked_item_x_diff(0.0f),
      cur_x(0),
      cur_y(0),
      scroll_y(0),
      num_rows(0),
      edit_mode(false),
      menu_interpolation(0x0),
      manual_value_input_dialog(0x0),
      manual_time_input_dialog(0x0),
      hidden_by_sequencer(false),
      is_controller(false),
      owner(0),
      channel_type(vsx_widget_sequence_channel_TYPE_PARAMETER),
      totalsize(0.0f),
      ff(0.0f),
      levelstart(0.0f),
      y_start(0.0f),
      y_end(0.0f),
      m_pos(0.0f),
      mouse_clicked_id(0),
      totalysize(0.0f),
      dlx(0.0f),
      dly(0.0f)
  {
  }

};

#endif
