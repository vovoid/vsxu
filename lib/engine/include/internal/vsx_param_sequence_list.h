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

#include <internal/vsx_param_abstraction.h>
#include <internal/vsx_param_sequence.h>
#include "vsx_param_sequence_group.h"

class vsx_param_sequence_list {
	void* engine;
  float int_vtime;
  int time_source; // 0 = engine, 1 = int_vtime
  float* other_time_source;
  bool run_on_edit_enabled;
  float total_time;
  std::list<vsx_param_sequence*> parameter_channel_list;
  std::map<vsx_engine_param*,vsx_param_sequence*> parameter_channel_map;
  std::list<void*> master_channel_list;
  std::map<vsx_string<>,void*> master_channel_map;
  std::map<vsx_string<>, vsx_param_sequence_group* > groups;

public:

  // parameter sequencer operations
  void add_param_sequence(vsx_engine_param* param, vsx_comp_abs* comp);
  void remove_param_sequence(vsx_engine_param* param);

  void set_run_on_edit(bool new_value) { run_on_edit_enabled = new_value; }

  void get_init(vsx_engine_param* param, vsx_command_list* dest, vsx_string<>comp_name, vsx_string<>prefix = "");
  void get_contents(vsx_engine_param* param, vsx_command_list* dest, vsx_string<>controller_id);

  //void add_line(vsx_engine_param* param, vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix = "");
  void update_line(vsx_engine_param* param, vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix = "");
  void insert_line(vsx_engine_param* param, vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix = "");
  void insert_line_absolute(vsx_engine_param* param, vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix = "");
  void remove_line(vsx_engine_param* param, vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix = "");

  vsx_string<> dump_param(vsx_engine_param* param);
  vsx_string<> dump_param_values(vsx_engine_param* param);
  void inject_param(vsx_engine_param* param, vsx_comp_abs* comp, vsx_string<>data);

  void get_params_with_keyframe_at_time(float time, float tolerance, vsx_nw_vector<vsx_engine_param* >& result);

  // master channel operations
  int add_master_channel(vsx_string<>name);
  int remove_master_channel(vsx_string<>name);
  vsx_string<>dump_master_channel(vsx_string<>channel_name);
  void update_master_channel_line(vsx_string<>channel_name, vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix = "");
  void insert_master_channel_line(vsx_string<>channel_name, vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix = "");
  void remove_master_channel_line(vsx_string<>channel_name, vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix = "");
  void time_sequence_master_channel_line(vsx_string<>channel_name, vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix = "");
  void remove_master_channel_lines_referring_to_sequence_list(vsx_param_sequence_list* list);

  void dump_master_channels_to_command_list(vsx_command_list &savelist);
  void inject_master_channel(vsx_string<>name, vsx_string<>data);

  // parameter group operations
  void group_add_param(vsx_string<> group_name, vsx_string<> comp_param);
  void group_del_param(vsx_string<> group_name, vsx_string<> comp_param);
  void group_del(vsx_string<> group_name);
  vsx_string<> group_dump(vsx_string<> group_name);
  vsx_string<> group_dump_all();
  void group_inject(vsx_string<>& data);

  // load / save operations
  void get_sequences(vsx_command_list* dest);
  vsx_string<> get_sequence_list_dump();
  vsx_string<> get_channel_names();

  // time operations
  void set_time(float new_vtime) { int_vtime = new_vtime; }
  void set_time_source(int new_time_source) { time_source = new_time_source; }
  void rescale_time(float start, float scale);
  float calculate_total_time(bool no_cache = false);

  // run / execute
  void run(float dtime, float blend = 1.0f);
  void run_absolute(float vtime, float blend = 1.0f);

  // initialization / de-initialization
  void set_engine(void* s_engine) { engine = s_engine; }
  void clear_master_sequences();
  vsx_param_sequence_list()
  {
  	engine = 0x0;
  	other_time_source = 0x0;
  	total_time = 0.0f;
  	run_on_edit_enabled = true;
  }
  vsx_param_sequence_list(void* my_engine);
  ~vsx_param_sequence_list();
  vsx_param_sequence_list(const vsx_param_sequence_list &b);
};
