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


#ifndef VSX_ENGINE_H
#define VSX_ENGINE_H

#include "vsx_engine.h"
#include <vsx_platform.h>

#include <filesystem/vsx_filesystem.h>
#include <command/vsx_command.h>
#include <command/vsx_command_list.h>
#include "vsx_param.h"
#include <module/vsx_module.h>
#include <time/vsx_timer.h>

#include <internal/vsx_comp_abs.h>
#include <internal/vsx_comp_channel.h>
#include <internal/vsx_param_abstraction.h>
#include <internal/vsx_param_interpolation.h>
#include <internal/vsx_param_sequence.h>
#include <internal/vsx_param_sequence_list.h>
#include <internal/vsx_sequence_pool.h>
#include "vsx_module_list_abs.h"
#include "vsx_module_list_factory.h"
#include <internal/vsx_note.h>

class vsx_timer;
class vsx_comp;
class vsxl_engine;


#include "vsx_engine_abs.h"


//////////////////////////////////////////////////////////////////////
class ENGINE_DLLIMPORT vsx_engine : public vsx_engine_abs
{
public:

//-- accessors
  // module list - the so/dll loaded modules
  vsx_module_list_abs* get_module_list();
  void set_module_list( vsx_module_list_abs* new_module_list );

  //---------------------------------------------------------------------------
  // get engine state - is it playing/stopped/loading?
  int get_engine_state();


  //---------------------------------------------------------------------------
  // get meta information (whole string, or by field)
  vsx_string<>get_meta_information();
  vsx_string<>get_meta_information(size_t index);


  //---------------------------------------------------------------------------
  // get info on modules left to load
  int get_modules_left_to_load();
  int get_modules_loaded();

  //---------------------------------------------------------------------------
  // sequence pool
  vsx_sequence_pool* get_sequence_pool();

  //---------------------------------------------------------------------------
  // control wether engine sends time to client via command lists
  // every frame (minor optimization if you don't need it)
  void set_no_send_client_time(bool new_value);

  //---------------------------------------------------------------------------
  // tells wether or not engine has any commands in the cache to process
  bool get_commands_internal_count();

  //---------------------------------------------------------------------------
  // returns a pointer to the engine info struct (same one used in the modules)
  vsx_module_engine_state* get_engine_info();

  //---------------------------------------------------------------------------
  // prohibit components from calling module's run()
  // This is needed when calling the same instance of engine multiple times per
  // frame. Before the first call, this should be set to false. But before
  // rendering a second+ time, set it to true to optimize away unneeded run()
  // operations in the modules.
  bool get_render_hint_module_output_only();
  void set_render_hint_module_output_only(bool new_value);
  bool get_render_hint_module_run_only();
  void set_render_hint_module_run_only(bool new_value);

  // don't reset frame status when done rendering
  bool get_render_hint_post_render_reset_component_status();
  void set_render_hint_post_render_reset_component_status( bool new_value );



//-- time manipulation and status

  // returns the time the engine has spent on this frame so far
  float get_frame_elapsed_time();

  // reset engine's timer
  void set_constant_frame_progression(float new_frame_cfp_time);
  void set_ignore_per_frame_time_limit(bool new_value);
  void reset_time();
  double get_fps();
  float get_last_frame_time();
  void set_amp(float amp);
  void set_speed(float spd);

  // 0 = reserved for wave data
  // 1 = reserved for frequency data
  void set_float_array_param(int id, vsx_module_engine_float_array* float_array);

  // send keyboard and mouse events through the engine down to the modules
  void set_input_event_queue(vsx_input_event_queue* queue);

  // module and parameter interface
  unsigned long get_num_modules();
  vsx_comp* get_component_by_name(vsx_string<>label);
  vsx_comp* get_by_id(unsigned long id);
  vsx_module_param_abs* get_in_param_by_name(vsx_string<>module_name, vsx_string<>param_name);
  vsx_module_param_abs* get_out_param_by_name(vsx_string<>module_name, vsx_string<>param_name);
  vsx_module* get_module_by_name(vsx_string<>module_name);
  vsx_string<>get_modules_not_loaded();

  // get a list of all external-exposed parameters (parameters that we want to export from a sub-engine)
  void get_external_exposed_parameters( vsx_nw_vector< vsx_module_param_abs* >* result );


//-- engine function / lifecycle presented in the order they should happen
  // constructors
  vsx_engine(vsx_module_list_abs* initial_module_list );

  // should be run soon after the GL surface is initialized
  bool start();

  // load a new state (clearing out the previous one)
  int load_state(vsx_string<>filename, vsx_string<>*error_string = 0);

  // load a new state from external archive, via pre-existing filesystem pointer
  // This is if you don't want engine to maintain its own filesystem, and if you put
  // for instance 3 state files into one .vsx file with vsxz, and want to load them into 3 different engines
  int load_state_filesystem(vsx_string<>filename, vsx_string<>*error_string, vsx::filesystem* filesystem);

  // process messages - this should be run once per physical frame
  void process_message_queue(vsx_command_list *cmd_in, vsx_command_list *cmd_out_res, bool exclusive = false, bool ignore_timing = false, float max_time = 0.01f);

  // sequencer time control
  void time_play();
  void time_stop();
  void time_rewind();

  // render
  //   module_output_only - if you don't want modules to run() - and thus only produce output
  //   good if you want to do multi pass rendering - most mesh and texture operations are done in run()
  bool render();

  // clears out the current state
  void unload_state();

  // should be run when the GL surface is destroyed
  bool stop();

  // destructor
  virtual ~vsx_engine();
};


#include <internal/vsx_comp.h>

#endif

