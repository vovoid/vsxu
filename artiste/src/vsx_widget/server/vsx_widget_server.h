#ifndef VSX_NO_CLIENT
#ifndef VSX_WIDGET_SERVER_H
#define VSX_WIDGET_SERVER_H
// VSX_WIDGET_SERVER ***************************************************************************************************
// VSX_WIDGET_SERVER ***************************************************************************************************
// VSX_WIDGET_SERVER ***************************************************************************************************
// VSX_WIDGET_SERVER ***************************************************************************************************

#define VSX_WIDGET_SERVER_CONNECTION_TYPE_INTERNAL 1
#define VSX_WIDGET_SERVER_CONNECTION_TYPE_SOCKET 2

class vsx_widget_server : public vsx_widget {
#ifndef VSXU_PLAYER  
  vsx_texture mtex;
#endif
//module selection stuff
	std::map<vsx_string,vsx_module_info*> module_list;
	std::map<vsx_string,vsx_module_info*>::const_iterator module_iter;
	
	bool selection;
	float delta_move,delta_zoom;
	vsx_vector remPointer;
	vsx_vector selection_start,selection_end;

  vsx_command_list dump_commands;
  // dialogs
  vsx_widget* export_dialog_ext;
  vsx_widget* export_dialog_state;
  vsx_widget* connect_dialog;
  vsx_widget* sequencer;
  vsx_widget* seq_pool;
  
  vsx_vector alert_delta;
  vsx_string server_message;
  unsigned int server_type; // 1 is internal, 2 is connected via socket

  vsx_command_list_client* client;
public:
  float server_vtime;
  float server_fps;
  
  // vsx_engine holder
  void *engine;

  vsx_string state_name;
  
  vsx_string connection_id; // the unique id of this connection in the event that multiple clients are connected to one server
  vsx_string server_version; // the server version of vsxu
  std::list<vsx_widget*> selected_list;

  vsx_widget* module_chooser;
  vsx_widget* module_chooser_list;
  vsx_widget* state_chooser;
  vsx_widget* resource_chooser;

  std::map<vsx_string, vsx_widget*> comp_list;
  std::map<vsx_string, vsx_widget*>::const_iterator comp_list_iter;
  std::map<vsx_string, vsx_widget*> note_list;
  std::map<vsx_string, vsx_widget*>::const_iterator note_iter;

  vsx_command_list *cmd_in;
  vsx_command_list *cmd_out;

  // constructor
  vsx_widget_server();
  void init();
  void reinit();
  void on_delete();

  // client server
  void server_connect(vsx_string host, vsx_string port);

  // command processor
  void vsx_command_process_f();
  void vsx_command_process_b(vsx_command_s *t);


  // component selection
  void select(vsx_widget* comp);
  bool select_add(vsx_widget* comp);
  void select_add_gui(vsx_widget* comp);
  
  void param_alias_ok(vsx_string p_def, vsx_string io, vsx_string comp, vsx_string param, vsx_string source_comp, vsx_string source_param, vsx_string seven);
  bool event_key_down(signed long key, bool alt = false, bool ctrl = false, bool shift = false);
  void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords);
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_wheel(float y);
  void draw();
  void undo_s() {cmd_out->add_raw("undo_s");};

  // utility functions
  vsx_string get_unique_name(vsx_string name);
  vsx_string build_comp_helptext(vsx_string path);
  vsx_widget* find_component(vsx_string name);
};

#endif
#endif
