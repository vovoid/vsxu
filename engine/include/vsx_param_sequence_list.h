/*
 * vsx_param_sequence_list.h
 *
 *  Created on: Apr 19, 2009
 *      Author: jaw
 */

#ifndef VSX_PARAM_SEQUENCE_LIST_H_
#define VSX_PARAM_SEQUENCE_LIST_H_

class vsx_param_sequence_list {
	void* engine;
  float int_vtime;
  int time_source; // 0 = engine, 1 = int_vtime
  float* other_time_source;
  vsx_command_list commands_send;
  vsx_command_list commands_return;
  bool run_on_edit_enabled;
  float total_time;
  std::list<vsx_param_sequence*> parameter_channel_list;
  std::map<vsx_engine_param*,vsx_param_sequence*> parameter_channel_map;
  std::list<void*> master_channel_list;
  std::map<vsx_string,void*> master_channel_map;
public:
  // parameter sequencer operations
  void add_param_sequence(vsx_engine_param* param, vsx_comp_abs* comp);
  void remove_param_sequence(vsx_engine_param* param);

  void set_run_on_edit(bool new_value) { run_on_edit_enabled = new_value; };

  void get_init(vsx_engine_param* param, vsx_command_list* dest,vsx_string comp_name, vsx_string prefix = "");
  void get_contents(vsx_engine_param* param, vsx_command_list* dest,vsx_string controller_id);

  //void add_line(vsx_engine_param* param, vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string cmd_prefix = "");
  void update_line(vsx_engine_param* param, vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string cmd_prefix = "");
  void insert_line(vsx_engine_param* param, vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string cmd_prefix = "");
  void remove_line(vsx_engine_param* param, vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string cmd_prefix = "");

  vsx_string dump_param(vsx_engine_param* param);
  void inject_param(vsx_engine_param* param, vsx_comp_abs* comp, vsx_string data);

  // master channel operations
  int add_master_channel(vsx_string name);
  int remove_master_channel(vsx_string name);
  vsx_string dump_master_channel(vsx_string channel_name);
  //void add_master_channel_line(vsx_string channel_name, vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string cmd_prefix = "");
  void update_master_channel_line(vsx_string channel_name, vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string cmd_prefix = "");
  void insert_master_channel_line(vsx_string channel_name, vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string cmd_prefix = "");
  void remove_master_channel_line(vsx_string channel_name, vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string cmd_prefix = "");
  void time_sequence_master_channel_line(vsx_string channel_name, vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string cmd_prefix = "");
  void remove_master_channel_lines_referring_to_sequence_list(vsx_param_sequence_list* list);

  void dump_master_channels_to_command_list(vsx_command_list &savelist);
  void inject_master_channel(vsx_string name, vsx_string data);

  // load / save operations
	void get_sequences(vsx_command_list* dest);
  vsx_string get_sequence_list_dump();
  vsx_string get_channel_names();

  // time operations
  void set_time(float new_vtime) { int_vtime = new_vtime; };
  void set_time_source(int new_time_source) { time_source = new_time_source; };
  void rescale_time(float start, float scale);
  float calculate_total_time(bool no_cache = false);

  // run / execute
  void run(float dtime,lamer show_debug = false);
  void run_absolute(float vtime);

  // initialization / de-initialization
  void set_engine(void* s_engine) { engine = s_engine; }
  void clear_master_sequences();
  vsx_param_sequence_list()
  {
  	engine = 0x0;
  	other_time_source = 0x0;
  	total_time = 0.0f;
  	run_on_edit_enabled = true;
  };
  vsx_param_sequence_list(void* my_engine);
  ~vsx_param_sequence_list();
  vsx_param_sequence_list(const vsx_param_sequence_list &b);
};

#endif /* VSX_PARAM_SEQUENCE_LIST_H_ */
