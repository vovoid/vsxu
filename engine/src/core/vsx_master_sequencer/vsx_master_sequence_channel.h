/*
 * vsx_master_sequence_channel.h
 *
 *  Created on: May 4, 2009
 *      Author: jaw
 */

#ifndef VSX_MASTER_SEQUENCE_CHANNEL_H_
#define VSX_MASTER_SEQUENCE_CHANNEL_H_


class vsx_param_sequence_list;

// sequence channel entry is a block entry in the chain of blocks
class vsx_sequence_master_channel_item {
public:
	vsx_sequence time_sequence; // how to translate real time into sequence_list-time, this one is a controller-like so runs from 0 to 1

	vsx_param_sequence_list* pool_sequence_list;
	float start_time;
	float length; // real-time length of this chunk
	float total_length; // actual length, to next item
	vsx_string pool_name;
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

	//void add_line(vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string cmd_prefix = "");
  void update_line(vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string cmd_prefix = "");
  void insert_line(vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string cmd_prefix = "");
  void remove_line(vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string cmd_prefix = "");
  void time_sequence(vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string cmd_prefix = "");
  void remove_all_lines_referring_to_sequence_list(vsx_param_sequence_list* list);

  vsx_string dump();
	void set_time(float new_time);
	void set_engine(void* new_engine) { engine = new_engine; };
	void run(float dtime);
	void inject(vsx_string inject_string);
	vsx_master_sequence_channel();
};

#endif /* VSX_MASTER_SEQUENCE_CHANNEL_H_ */
