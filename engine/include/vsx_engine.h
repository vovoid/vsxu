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

#ifndef VSX_ENGINE_H
#define VSX_ENGINE_H

#ifdef _WIN32
#if VSX_ENG_DLL
# define DLLIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
# define DLLIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */
#else
#define DLLIMPORT
#endif

#include <vsx_platform.h>

#include "vsxfst.h"
#include "vsx_math_3d.h"
#include "vsx_command.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include "vsx_timer.h"

#include "vsx_comp_abs.h"
#include "vsx_comp_channel.h"
#include "vsx_param_abstraction.h"
#include "vsx_param_interpolation.h"
#include "vsx_param_sequence.h"
#include "vsx_param_sequence_list.h"
#include "vsx_sequence_pool.h"
#include "vsx_module_list_abs.h"

class vsx_timer;
class vsx_note;
class vsx_comp;
class vsxl_engine;

//////////////////////////////////////////////////////////////////////
class DLLIMPORT vsx_engine
{
private:

// helper function to initialize all values
  void constructor_set_default_values();

// filesystem handler
  vsxf filesystem;
  vsx_engine_environment engine_environment;

//-- current state name
  vsx_string state_name;

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

//-- module list
  vsx_module_list_abs* module_list;

//-- time/sequencing variables
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
  double frame_dfps;
  int frame_d;

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

//-- accessors
  // module list - the so/dll loaded modules
  vsx_module_list_abs* get_module_list();
  void set_module_list( vsx_module_list_abs* new_module_list );

  // get engine state - is it playing/stopped/loading?
  int get_engine_state();
  // get info on modules left to load
  int get_modules_left_to_load();
  int get_modules_loaded();

  // sequence pool
  vsx_sequence_pool* get_sequence_pool();

  // control wether engine sends time to client via command lists
  // every frame (minor optimization if you don't need it)
  void set_no_send_client_time(bool new_value);

  // tells wether or not engine has any commands in the cache to process
  bool get_commands_internal_status();

  // returns the time the engine has spent on this frame so far
  float get_frame_elapsed_time();

  // returns a pointer to the engine info struct (contains useful information for modules)
  vsx_module_engine_info* get_engine_info();

  // reset engine's timer
  void reset_time();



  vsx_string meta_information;
  vsx_avector<vsx_string> meta_fields; // split meta information

  unsigned long get_num_modules() {return forge.size(); }

  vsx_module_param_abs* get_in_param_by_name(vsx_string module_name, vsx_string param_name);

  void input_event(vsx_engine_input_event &new_input_event);


  vsx_comp* get_component_by_name(vsx_string label);
  vsx_comp* get_by_id(unsigned long id);

  // should be run when the GL surface is destroyed
  bool stop();
  // should be run soon after the GL surface is initialized
  bool start();
  // render
  //   module_output_only - if you don't want modules to run() - and thus only produce output
  //   good if you want to do multi pass rendering - most mesh and texture operations are done in run()
  bool render();

  void process_message_queue(vsx_command_list *cmd_in, vsx_command_list *cmd_out_res, bool exclusive = false, bool ignore_timing = false);

  double get_fps();
  float get_last_frame_time();
  void set_amp(float amp);
  void set_speed(float spd);
  void set_constant_frame_progression(float new_frame_cfp_time);
  void play();

  void set_ignore_per_frame_time_limit(bool new_value);

  // 0 = reserved for wave data
  // 1 = reserved for frequency data
  void set_float_array_param(int id, vsx_engine_float_array* float_array);

  // loads a new state (clearing out the previous one)
  int load_state(vsx_string filename, vsx_string *error_string = 0);
  // clears out the current state
  void unload_state();

  vsx_engine();
  vsx_engine(vsx_string path);
  ~vsx_engine();
};


#include "vsx_comp.h"

#endif

