/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies Copyright (C) 2003-2011
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

/*
 * vsx_sequence_pool.h
 *
 *  Created on: Apr 19, 2009
 *      Author: jaw
 */

#ifndef VSX_SEQUENCE_POOL_H_
#define VSX_SEQUENCE_POOL_H_


// There is one main sequence list (old sequencer) which is enabled by default.
// If you enable the sequence manager and project sequencer (will be enabled when the manager-commands are run from state)
// it will completely override the default sequence.
// Also, if you accidentally use 2 channels for the same parameter, the latter (with higher channel id) will
// simply overwrite the value set by the first one.
//
//  Parameters who have sequence manager control are not prohibited from having connections,

// sequence pool is where all small snippets are stored
class vsx_sequence_pool {
	void* engine;
	bool edit_enabled;
	vsx_param_sequence_list* cur_sequence_list;
	vsx_string active;
	std::map<vsx_string, vsx_param_sequence_list*> sequence_lists;
	float vtime;
	int state; // 0 = stopped, 1 = playing
public:

	// sequence list operations
	int add(vsx_string name); // 1 = success, 0 = fail
	int del(vsx_string name);
	int clone(vsx_string name, vsx_string new_name);
	int select(vsx_string name);
	vsx_param_sequence_list* get_selected();

	bool toggle_edit();
	bool get_edit_enabled();

	// sequence operations on current active list
	int add_sequence(vsx_engine_param* param, vsx_comp_abs* comp);

	void set_engine(void* new_engine);
	vsx_string dump_names();
	void run(float dtime,bool run_from_channel = false); // if enabled, it'll run all sequences and set values
	//void run_from_channel(float vtime); // run this from a channel, i.e. totally abstracted time
	void set_time(float time);
	vsx_param_sequence_list* get_sequence_list_by_name(vsx_string name);
	void dump_to_command_list(vsx_command_list &savelist);

	// deinitialization
  void clear();

	vsx_sequence_pool();
};

// sequence manager holds all sequence channels.
/*class vsx_sequence_manager {
public:
	std::map<vsx_string, vsx_sequence_channel*> channels;
	void run(float dtime);
};
*/

#endif /* VSX_SEQUENCE_POOL_H_ */
