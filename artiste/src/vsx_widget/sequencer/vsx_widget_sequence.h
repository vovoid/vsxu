#ifndef VSX_NO_CLIENT
#ifndef VSX_WIDGET_SEQUENCE_H
#define VSX_WIDGET_SEQUENCE_H

class vsx_widget_sequence_editor : public vsx_widget {
  vsx_widget *server;

  vsx_vector parentpos;
  vsx_widget* but_rew;
  vsx_widget* but_play;
  vsx_widget* but_stop;
  vsx_widget* but_add_master_channel;
  vsx_widget* but_remove_master_channel;
  std::vector<vsx_widget*> channels;
  std::map<vsx_string,vsx_widget*> channels_map;
  float channels_visible;
  int channels_start;
  vsx_widget* sequence_list;
  void update_list();
  // dialogs
  vsx_widget *name_dialog;
public:
	bool disable_master_channel;

  vsx_widget* timeline;
  void check_timeline();
  void set_timeline_show_wave_data(bool value);
  virtual void load_sequence_list();
  bool update_time_from_engine;
  int engine_status;
  float tstart;
  float tend;
  float curtime;

	virtual void init();
  void i_draw();
  void toggle_channel_visible(vsx_string name);
  // removes all sequence channels, used when switching seqpool etc.
  void clear_sequencer();
  virtual void vsx_command_process_b(vsx_command_s *t);
  virtual bool event_key_down(signed long key, bool alt, bool ctrl, bool shift);
  virtual void interpolate_size();
  void set_server(vsx_widget* new_server);
  vsx_widget* get_server();
  void remove_master_channel_items_with_name(vsx_string name);
  vsx_widget_sequence_editor() {
  	disable_master_channel = false;
  	but_add_master_channel = 0;
  }
};





#endif
#endif
