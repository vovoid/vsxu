/**
* Project: VSXu Engine: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu Engine.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Lesser General Public License (LGPL)
*
* VSXu Engine is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU Lesser General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


#pragma once

#include <internal/vsx_param_sequence_item.h>

class vsx_param_sequence
{
  float last_time; // last time we were called, to see if we should trace back
  float line_time; // current line time (accumulated)
  int line_cur; // current line
  vsx_string<>cur_val, to_val;
  float cur_delay;
  int cur_interpolation;
  float total_time;
public:
  void* engine;
  vsx_comp_abs* comp;
  vsx_engine_param* param;
  bool repeat; // repeat this or stop at end?

  float time_scaler; // global multiplier

  float p_time; // internal time

  float interp_time; // interpolation time for use when feeding the engine interpolator

  std::vector<vsx_param_sequence_item> items; // the actual sequence

  void recalculate_accum_times()
  {
    float accum_time = 0.0f;
    foreach(items, i)
    {
      items[i].accum_time = accum_time;
      accum_time += items[i].total_length;
    }
  }

  void set_time(float stime);
  void execute(float ptime, float blend = 1.0f); // returns command if available
  void update_line(vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix = "");
  void insert_line(vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix = "");
  void insert_line_absolute(vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix = "");
  void remove_line(vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix = "");
  void rescale_time(float start, float scale);
  bool has_keyframe_at_time(float time, float tolerance);

  float calculate_total_time(bool no_cache = false);
  vsx_string<> dump();
  vsx_string<> dump_values();
  void inject(vsx_string<>ij);
  vsx_param_sequence();
  vsx_param_sequence(int p_type,vsx_engine_param* param);
  // no copy constructor needed
};
