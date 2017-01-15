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

#include <internal/vsx_param_sequence_list.h>

#pragma once

// There is one main sequence list (old sequencer) which is enabled by default.
// If you enable the sequence manager and project sequencer (will be enabled when the manager-commands are run from state)
// it will completely override the default sequence.
// Also, if you accidentally use 2 channels for the same parameter, the latter (with higher channel id) will
// simply overwrite the value set by the first one.
//
//  Parameters who have sequence manager control are not prohibited from having connections,

// sequence pool is where all small snippets are stored
class vsx_sequence_pool
{
	void* engine;
	bool edit_enabled;
  bool play_override_enabled; // for playing a pool without editing it
	vsx_param_sequence_list* cur_sequence_list;
	vsx_string<>active;
	std::map<vsx_string<>, vsx_param_sequence_list*> sequence_lists;
	float vtime;
  int current_state; // 0 = stopped, 1 = playing
  float loop_point; // vtime is a modulus of this
public:
  // global parameter operations
  // removes the parameter from all internal sequence lists
  void remove_param_sequence(vsx_engine_param* param);

	// sequence list operations
	int add(vsx_string<>name); // 1 = success, 0 = fail
	int del(vsx_string<>name);
	int clone(vsx_string<>name, vsx_string<>new_name);
	int select(vsx_string<>name);
	vsx_param_sequence_list* get_selected();

	bool toggle_edit();
  void set_play_override(bool n);
	bool get_edit_enabled();

	// sequence operations on current active list
	int add_sequence(vsx_engine_param* param, vsx_comp_abs* comp);
  vsx_param_sequence_list* get_sequence_list_by_name(vsx_string<>name);

	void set_engine(void* new_engine);

  void run(
    float dtime,
    bool run_from_channel = false
  ); // if enabled, it'll run all sequences and set values

  // time manipulation
  int get_state();
  float get_vtime();
	void set_time(float time);
  void play();
  void stop();
  void rewind();
  void set_loop_point(float new_loop_point);

  // serialize functions
  vsx_string<>dump_names();
  void dump_to_command_list(vsx_command_list &savelist);
  // save/load from file
  bool export_to_file(vsx_string<>filename);
  bool import_from_file(vsx_string<>filename);

  bool export_values_to_file(vsx_string<> filename);

	// deinitialization
  void clear();

	vsx_sequence_pool();
};

