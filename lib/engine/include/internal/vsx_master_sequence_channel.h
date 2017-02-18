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


#ifndef VSX_MASTER_SEQUENCE_CHANNEL_H_
#define VSX_MASTER_SEQUENCE_CHANNEL_H_


class vsx_param_sequence_list;

// sequence channel entry is a block entry in the chain of blocks
class vsx_sequence_master_channel_item {
public:
  vsx::sequence::channel<vsx::sequence::value_float> time_sequence; // how to translate real time into sequence_list-time, this one is a controller-like so runs from 0 to 1

	vsx_param_sequence_list* pool_sequence_list;
	float start_time;
	float length; // real-time length of this chunk
	float total_length; // actual length, to next item
	vsx_string<>pool_name;
	void run(float abs_time);
	vsx_sequence_master_channel_item();
};

// sequence channel is a flow of sequence_lists in the project sequencer, time is abstracted into the channel
class vsx_master_sequence_channel {
	void* engine;
  float last_time; // last time we were called, to see if we should trace back
  float line_time; // current line time (accumulated)
  int line_cur; // current line
	float i_vtime;
	std::vector<vsx_sequence_master_channel_item*> items;
	void i_remove_line(int pos);
public:

	//void add_line(vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix = "");
  void update_line(vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix = "");
  void insert_line(vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix = "");
  void remove_line(vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix = "");
  void time_sequence(vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix = "");
  void remove_all_lines_referring_to_sequence_list(vsx_param_sequence_list* list);

  vsx_string<>dump();
	void set_time(float new_time);
	void set_engine(void* new_engine) { engine = new_engine; };
	void run(float dtime);
	void inject(vsx_string<>inject_string);
	vsx_master_sequence_channel();
};

#endif /* VSX_MASTER_SEQUENCE_CHANNEL_H_ */
