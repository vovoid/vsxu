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

class vsx_timer;
class module_dll_info;
class vsx_note;
class vsx_comp;
class vsxl_engine;

//////////////////////////////////////////////////////////////////////
class DLLIMPORT vsx_engine {
private:
  vsx_string vsxu_base_path;
  vsxf filesystem; // our master filesystem handler
  vsx_engine_environment engine_environment;
  //-- component list
  std::vector<vsx_comp*> forge;
  std::map<vsx_string,vsx_comp*> forge_map;
  std::map<vsx_string,vsx_comp*>::const_iterator forge_map_iter;
  std::map<vsx_string,vsx_comp*>::reverse_iterator forge_map_riter;
  //-- notes
  std::map<vsx_string,vsx_note> note_map;
  std::map<vsx_string,vsx_note>::iterator note_iter;
  //-- engine state
  bool first_start;
  bool stopped;

  //-- open dll/so's
  #if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
    std::vector<HMODULE> module_handles;
  #endif
  #if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
    std::vector<void*> module_handles;
  #endif

  //-- available modules
  std::vector<vsx_module_info*> module_infos;
  std::map<vsx_string,vsx_module_info*> module_list;
  std::map<vsx_string,vsx_module_info*>::const_iterator module_iter;
  std::map<vsx_string,module_dll_info*> module_dll_list;
  void build_module_list(vsx_string sound_type = ""); // (re)builds the module_list

  vsx_avector<vsx_comp*> outputs;

  // Time/sequencing variables
  // global frame counter (mostly for fun)
  vsx_timer m_timer;
  vsx_timer frame_timer;
  float last_frame_time;
  float d_time;
  // delta fps with variable measurement scope
  int last_m_time_synch; // has the engine been synchronized with module time in control?
  double frame_dcount;
  double frame_dtime;
  double frame_dprev;
  double frame_dfps;
  int frame_d;
  // engine's attempts of keeping up outside framerate (for artiste etc) while loading a state
  bool ignore_per_frame_time_limit; // default: false

  float lastsent;
  int last_e_state;
  void tell_client_time(vsx_command_list *cmd_out);

  // timer that aims to benchmark the message queue in realtime to predict/prevent framedrops
  // due to handling heavy commands.
  vsx_timer vsx_command_timer;
  // internal commands, to perform timing operations on commands to not lock up the system
  vsx_string state_name;
  vsx_command_list commands_res_internal;
  vsx_command_list commands_out_cache;
  // auto-increment variable to give components unique names in the event that the user doesn't care
  int component_name_autoinc;
  void set_default_values();
  int i_load_state(vsx_command_list& load1, vsx_string *error_string);
  void i_clear(vsx_command_list *cmd_out = 0, bool clear_critical = false);
  void rename_component();
  int rename_component(vsx_string old_identifier, vsx_string new_base = "$", vsx_string new_name = "$");
  void process_message_queue_redeclare(vsx_command_list *cmd_out_res);
  void redeclare_in_params(vsx_comp* comp, vsx_command_list *cmd_out);
  void redeclare_out_params(vsx_comp* comp, vsx_command_list *cmd_out);
  void send_state_to_client(vsx_command_list *cmd_out);
  int get_state_as_commandlist(vsx_command_list &savelist);
  void message_fail(vsx_string header, vsx_string message);
  vsx_module_param_interpolation_list interpolation_list;

  vsx_avector<vsx_command_list> undo_buffer;
  float g_timer_amp;

#ifdef VSXU_MODULES_STATIC
  void register_static_module(vsx_string name);
#endif
  float frame_cfp_time;

public:
  // scripting interface
  #ifdef VSX_ENG_DLL
    vsxl_engine* vsxl;
  #else
    void* vsxl;
  #endif


  //debug refcounter
/*  #ifdef VSXU_DEBUG
    static int engine_counter;
    int engine_id;
  #endif*/
  //--

  bool dump_modules_to_disk;
  bool no_client_time;
  vsx_command_list commands_internal;
  vsx_timer g_timer;
  double frame_start_time;

  int e_state; // stopped or playing?

// Main Sequencer
  vsx_param_sequence_list sequence_list;

// Sequence Pool
  vsx_sequence_pool sequence_pool;
// - - - - - - -

  int modules_left_to_load;
  int modules_loaded;

  // pointer to this is info for the modules
#ifdef VSXU_ENGINE_INFO_STATIC
  static vsx_module_engine_info engine_info;
#else
  vsx_module_engine_info engine_info;
#endif

  vsx_comp* add(vsx_string label);
  //void add(vsx_string label, bool auto_naming);

  vsx_string meta_information;
  vsx_avector<vsx_string> meta_fields; // split meta information

  unsigned long get_num_modules() {return forge.size(); }

  vsx_module_param_abs* get_in_param_by_name(vsx_string module_name, vsx_string param_name);

  vsx_comp* get_by_name(vsx_string label) {
    if (forge_map.find(label) != forge_map.end()) {
    return forge_map[label];
    } else return 0;
  }
  vsx_comp* get_by_id(unsigned long id) {
    return forge[id];
  }

  // should be run when the GL surface is destroyed
  bool stop();
  // should be run soon after the GL surface is initialized
  bool start();
  bool render();

  void process_message_queue(vsx_command_list *cmd_in, vsx_command_list *cmd_out_res, bool exclusive = false);

  double get_fps();
  void set_amp(float amp);
  void set_speed(float spd);
  void set_constant_frame_progression(float new_frame_cfp_time);
  void play();

  void set_ignore_per_frame_time_limit(bool new_value);

  // 0 = reserved for wave data
  // 1 = reserved for frequency data
  void set_float_array_param(int id, vsx_engine_float_array* float_array);

  // run once the gfx engine is initialized
  void init(vsx_string sound_type = "");
  // loads a new state (clearing out the previous one)
  int load_state(vsx_string filename, vsx_string *error_string = 0);
  // clears out the current state
  void unload_state();

  void destroy();

  vsx_engine();
  vsx_engine(vsx_string path);
  ~vsx_engine();
};


#include "vsx_comp.h"

#endif

