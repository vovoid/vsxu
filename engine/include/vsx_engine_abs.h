#ifndef VSX_ENGINE_ABS_H
#define VSX_ENGINE_ABS_H


class vsx_engine_abs
{
protected:
// helper function to initialize all values
  void constructor_set_default_values();

// filesystem handler
  vsxf filesystem;
  vsx_engine_environment engine_environment;

//-- current state name
  vsx_string state_name;

//-- state meta information
  vsx_string meta_information;
  vsx_avector<vsx_string> meta_fields; // split meta information

//-- component forge
  std::vector<vsx_comp*> forge;
  std::map<vsx_string,vsx_comp*> forge_map;
  std::map<vsx_string,vsx_comp*>::const_iterator forge_map_iter;
  std::map<vsx_string,vsx_comp*>::reverse_iterator forge_map_riter;
  // auto-increment variable to give components unique names in the event that the user doesn't care
  int component_name_autoinc;

//-- module engine_info struct
  #ifdef VSXU_ENGINE_INFO_STATIC
    static vsx_module_engine_info engine_info;
  #else
    vsx_module_engine_info engine_info;
  #endif

//-- outputs
  vsx_avector<vsx_comp*> outputs;

//-- interpolation list
  vsx_module_param_interpolation_list interpolation_list;

//-- sequencer, sequencer pool
  vsx_param_sequence_list sequence_list;
  vsx_sequence_pool sequence_pool;

//-- notes
  std::map<vsx_string,vsx_note> note_map;
  std::map<vsx_string,vsx_note>::iterator note_iter;

//-- scripting interface
  #ifdef VSX_ENG_DLL
    vsxl_engine* vsxl;
  #else
    void* vsxl;
  #endif


//-- engine state machine controls
  int current_state; // stopped or playing?
  bool valid; // if engine has managed to start, can run at all
  bool first_start;
  bool stopped;
  int modules_left_to_load;
  int modules_loaded;

//-- engine rendering / behaviour hints
  bool render_hint_module_output_only;
  bool render_hint_module_run_only;

//-- module list
  vsx_module_list_abs* module_list;

//-- time/sequencing variables
  // loop endpoint
  float loop_point_end;

  vsx_timer g_timer;
  vsx_timer m_timer;
  vsx_timer frame_timer; // measure rendering time
  float last_frame_time;

  // delta fps with variable measurement scope
  float d_time;

  // timer to track how long the engine has been run
  double frame_start_time; // the starting time of this frame

  // time sending to client state variables
  bool no_send_client_time;
  float lastsent;
  int last_e_state;

  // has the engine been synchronized with module time in control?
  int last_m_time_synch;

  // engine's attempts of keeping up outside framerate (for artiste etc) while loading a state
  bool ignore_per_frame_time_limit; // default: false

  // constant frame progression time
  float frame_cfp_time;

  // engine speed control
  float g_timer_amp;

//-- global frame counter
  double frame_dcount;
  double frame_dtime;
  double frame_dprev;
  double frame_delta_fps;
  int frame_delta_fps_frame_count_interval;

//-- command internals
  // timer that aims to benchmark the message queue in realtime to predict/prevent framedrops
  // due to handling heavy commands.
  vsx_timer vsx_command_timer;
  // internal commands, to perform timing operations on commands to not lock up the system
  vsx_command_list commands_res_internal;
  vsx_command_list commands_out_cache;
  vsx_command_list commands_internal;

//-- undo buffer
  vsx_avector<vsx_command_list> undo_buffer;




//-- internal methods
  void tell_client_time(vsx_command_list *cmd_out);
  int i_load_state(vsx_command_list& load1, vsx_string *error_string, vsx_string info_filename = "[undefined]");
  void i_clear(vsx_command_list *cmd_out = 0, bool clear_critical = false);
  void rename_component();
  int rename_component(vsx_string old_identifier, vsx_string new_base = "$", vsx_string new_name = "$");
  void process_message_queue_redeclare(vsx_command_list *cmd_out_res);
  void redeclare_in_params(vsx_comp* comp, vsx_command_list *cmd_out);
  void redeclare_out_params(vsx_comp* comp, vsx_command_list *cmd_out);
  void send_state_to_client(vsx_command_list *cmd_out);
  int get_state_as_commandlist(vsx_command_list &savelist);
  void message_fail(vsx_string header, vsx_string message);
  // called each frame after engine has rendered
  void reset_input_events();
  #ifdef VSXU_MODULES_STATIC
    void register_static_module(vsx_string name);
  #endif
  // add component to the forge
  vsx_comp* add(vsx_string label);
public:

  // module and parameter interface
  virtual unsigned long get_num_modules() = 0;
  virtual vsx_comp* get_component_by_name(vsx_string label) = 0;
  virtual vsx_comp* get_by_id(unsigned long id) = 0;
  virtual vsx_module_param_abs* get_in_param_by_name(vsx_string module_name, vsx_string param_name) = 0;

  // get a list of all external-exposed parameters (parameters that we want to export from a sub-engine)
  virtual void get_external_exposed_parameters( vsx_avector< vsx_module_param_abs* >* result ) = 0;

  // should be run soon after the GL surface is initialized
  virtual bool start() = 0;

  // should be run when the GL surface is destroyed
  virtual bool stop() = 0;

  // process messages - this should be run once per physical frame
  virtual void process_message_queue(vsx_command_list *cmd_in, vsx_command_list *cmd_out_res, bool exclusive = false, bool ignore_timing = false, float max_time = 0.01f) = 0;

};

#endif // VSX_ENGINE_ABS_H
