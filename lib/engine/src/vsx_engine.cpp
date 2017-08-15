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

#include <vsx_platform.h>

#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
  #include <io.h>
#endif

#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
  #include <dlfcn.h>
  #include <syslog.h>
#endif

#include <sys/types.h>
#include <string/vsx_string.h>
#include <log/vsx_log.h>
#include "vsx_engine.h"
#include <internal/vsx_master_sequence_channel.h>

#include "vsx_module_list_factory.h"
#include "vsx_data_path.h"
#include <string/vsx_string_helper.h>
#include <vsx_param_helper.h>
#include <filesystem/vsx_filesystem_helper.h>
#include <filesystem/archive/vsx/vsx_filesystem_archive_vsx_writer.h>
#include <filesystem/archive/vsxz/vsx_filesystem_archive_vsxz_writer.h>

#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
  #include <dirent.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string>
  #include <sys/stat.h>
  #include <unistd.h>
#endif

#ifndef VSXE_NO_GM
#include "gm/gmThread.h"    // Game monkey script machine and thread
#include "gm/gmMachine.h"
#include "binds/gmCall.h"
#include "binds/gmMathLib.h"
#include "scripting/vsx_param_vsxl.h"
#include "scripting/vsx_comp_vsxl.h"
#include "scripting/vsxl_engine.h"
#endif


#include <vector>



#ifdef VSXU_ENGINE_INFO_STATIC
vsx_module_engine_state vsx_engine::engine_info;
#endif


vsx_engine::vsx_engine(vsx_module_list_abs* initial_module_list)
{
  module_list = initial_module_list;
  engine_info.module_list = (void*) module_list;
  constructor_set_default_values();
  loop_point_end = -1.0f;
}


vsx_engine::~vsx_engine()
{
  stop();
  commands_internal.clear_normal();
  commands_res_internal.clear_normal();
  commands_out_cache.clear_normal();
  i_clear(0,true, true);

  if (filesystem.get_archive()->is_archive())
    filesystem.get_archive()->close();

  for (size_t i = 0; i < undo_buffer.size(); i++)
  {
    undo_buffer[i]->clear_delete();
    delete undo_buffer[i];
  }
}


vsx_module_list_abs* vsx_engine::get_module_list()
{
  return module_list;
}

void vsx_engine::set_module_list( vsx_module_list_abs* new_module_list )
{
  module_list = new_module_list;
  engine_info.module_list = (void*) new_module_list;
}

int vsx_engine::get_engine_state()
{
  return current_state;
}

vsx_string<>vsx_engine::get_meta_information()
{
  return meta_information;
}

vsx_string<>vsx_engine::get_meta_information(size_t index)
{
  if (index >= meta_fields.size())
    return "";
  return meta_fields[index];
}



int vsx_engine::get_modules_left_to_load()
{
  return modules_left_to_load;
}

int vsx_engine::get_modules_loaded()
{
  return modules_loaded;
}



vsx_sequence_pool* vsx_engine::get_sequence_pool()
{
  return &sequence_pool;
}

void vsx_engine::set_no_send_client_time(bool new_value)
{
  no_send_client_time = new_value;
}

bool vsx_engine::get_commands_internal_count()
{
  if (commands_internal.count() == 0)
    return true;
  return false;
}

float vsx_engine::get_frame_elapsed_time()
{
  return (float)(g_timer.atime() - frame_start_time);
}

vsx_module_engine_state* vsx_engine::get_engine_info()
{
  return &engine_info;
}

bool vsx_engine::get_render_hint_post_render_reset_component_status()
{
  return render_hint_post_render_reset_component_status;
}

void vsx_engine::set_render_hint_post_render_reset_component_status( bool new_value )
{
  render_hint_post_render_reset_component_status = new_value;
}

bool vsx_engine::get_render_hint_module_output_only()
{
  return render_hint_module_output_only;
}

void vsx_engine::set_render_hint_module_output_only(bool new_value)
{
  render_hint_module_output_only = new_value;
}

bool vsx_engine::get_render_hint_module_run_only()
{
  return render_hint_module_run_only;
}

void vsx_engine::set_render_hint_module_run_only(bool new_value)
{
  render_hint_module_run_only = new_value;
}


void vsx_engine::reset_time()
{
  g_timer.start();
}

vsx_module_param_abs* vsx_engine::get_in_param_by_name(vsx_string<>module_name, vsx_string<>param_name)
{
  reqrv(valid, 0x0);
  vsx_comp* c = get_component_by_name(module_name);
  reqrv(c, 0x0);
  vsx_engine_param* p = c->get_params_in()->get_by_name(param_name);
  reqrv(p, 0x0);
  return p->module_param;
}


vsx_module_param_abs* vsx_engine::get_out_param_by_name(vsx_string<>module_name, vsx_string<>param_name)
{
  reqrv(valid, 0x0);
  vsx_comp* c = get_component_by_name(module_name);
  reqrv(c, 0x0);
  vsx_engine_param* p = c->get_params_out()->get_by_name(param_name);
  reqrv(p, 0x0);
  return p->module_param;
}


void vsx_engine::get_external_exposed_parameters( vsx_nw_vector< vsx_module_param_abs* >* result )
{
  // iterate through all modules
  for (forge_map_iter = forge_map.begin(); forge_map_iter != forge_map.end(); ++forge_map_iter)
  {
    vsx_comp* comp = (*forge_map_iter).second;
    // iterate through all parameters
    for (unsigned long i = 0; i < comp->get_params_in()->param_id_list.size(); ++i)
    {
      vsx_engine_param* param = comp->get_params_in()->param_id_list[i];
      // only return those that are exposed
      if (param->external_expose)
      {
        result->push_back( param->module_param );
      }
    }
  }
}

unsigned long vsx_engine::get_num_modules()
{
  return (unsigned long)forge.size();
}

vsx_comp* vsx_engine::get_component_by_name(vsx_string<>label)
{
  if (forge_map.find(label) != forge_map.end())
  {
    return forge_map[label];
  }
  return 0;
}

vsx_comp* vsx_engine::get_by_id(unsigned long id)
{
  return forge[id];
}


vsx_module* vsx_engine::get_module_by_name(vsx_string<>module_name)
{
  if (forge_map.find(module_name) != forge_map.end())
  {
    return forge_map[module_name]->module;
  }
  return 0x0;
}

vsx_string<>vsx_engine::get_modules_not_loaded()
{
  vsx_string<>res;
  for (std::vector<vsx_comp*>::iterator it = forge.begin(); it < forge.end(); ++it) {
    if ((*it)->module) {
      if (!(*it)->module->loading_done) {
        res += " " + (*it)->name + " ";
      }
    }
  }
  return res;
}

void vsx_engine::set_input_event_queue(vsx_input_event_queue* queue)
{
  engine_info.event_queue = queue;
}

int vsx_engine::load_state(vsx_string<>filename, vsx_string<>* error_string)
{
  if (!valid)
    return 2;

  filesystem.set_base_path("");

  if (filesystem.get_archive()->is_archive())
    filesystem.get_archive()->close();

  vsx_command_list load1(true);
  load1.set_filesystem(&filesystem);
  vsx_string<> i_filename = filename;

  bool is_archive = false;
  if (filename.size() >= 4)
  {
    if (filename.substr((int)filename.size()-4,4) == ".vsx")
    {
      filesystem.get_archive()->load(filename.c_str(), false);
      if (filesystem.get_archive()->is_archive_populated())
      {
        is_archive = true;
        i_filename = "_states/_default";
      } else
      {
        filesystem.get_archive()->close();
        return 0;
      }
    }
  }
  load1.load_from_file(i_filename,true);
  load1.garbage_collect();

  if (!is_archive)
    filesystem.set_base_path( vsx_data_path::get_instance()->data_path_get());

  int res = i_load_state(load1,error_string,filename);
  load1.clear_normal();

  return res;
}


int vsx_engine::load_state_filesystem(vsx_string<>filename, vsx_string<>*error_string, vsx::filesystem* fs)
{
  if (!valid)
    return 2;

  engine_info.filesystem = fs;

  vsx_command_list load1(true);
  load1.set_filesystem( fs );

  load1.load_from_file(filename, true);

  int res = i_load_state(load1, error_string, filename);

  return res;
}




// set engine speed
void vsx_engine::set_speed(float spd)
{
  if (!valid)
    return;

  engine_info.speed = spd;
  g_timer_amp = spd;
}

// set internal float parameter
void vsx_engine::set_float_array_param(int id, vsx_module_engine_float_array* float_array)
{
  if (!valid)
    return;
  engine_info.param_float_arrays[id] = float_array;
}

// set FX level amplification (sound, etc)
void vsx_engine::set_amp(float amp)
{
  if (!valid)
    return;

  engine_info.amp = amp;
}

// start the engine and sending all the modules the start signal
bool vsx_engine::start()
{
  // a few assertions
  if (0x0 == module_list)
  {
    vsx_printf(L"vsx_engine::start() error: not starting; module_list is 0x0h\n");
    return false;
  }

  if (!disabled)
    return false;

  if (disabled)
    disabled = false;

  if (first_start)
  {
    valid = true;
    sequence_list.set_engine(this);
    first_start = false;

    // create a new component for the screen
    vsx_comp* comp = new vsx_comp;
    comp->internal_critical = true;
    comp->engine_owner = (void*)this;
    comp->identifier = "outputs;screen";
    comp->load_module("outputs;screen", &engine_info);
    comp->component_class += ":critical";
    comp->name="screen0";

    // add this to our forge and forge_map
    forge.push_back(comp);
    forge_map["screen0"] = comp;
    // add to outputs
    outputs.push_back(comp);
    // set validity
  }
  for (std::vector<vsx_comp*>::iterator it = forge.begin(); it != forge.end(); ++it)
  {
    (*it)->start();
  }
  m_timer.start();
  g_timer.start();
  lastsent = 0;
  return true;
}

// stop the engine
bool vsx_engine::stop()
{
  if (!valid)
    return false;

  if (!disabled)
  {
    for (unsigned long i = 0; i < forge.size(); ++i)
    {
      forge[i]->stop();
    }
    disabled = true;
    return true;
  }
  return false;
}

void vsx_engine::set_constant_frame_progression(float new_frame_cfp_time)
{
  frame_cfp_time = new_frame_cfp_time;
}

void vsx_engine::time_play()
{
  if (!valid) return;
  current_state = VSX_ENGINE_PLAYING;
  engine_info.request_play = 0;
  g_timer.start();
}

void vsx_engine::time_stop()
{
  if (!valid) return;
  current_state = VSX_ENGINE_STOPPED;
  engine_info.request_stop = 0;
  g_timer.start();
}

void vsx_engine::time_rewind()
{
  if (!valid) return;
  current_state = VSX_ENGINE_REWIND;
  engine_info.request_rewind = 0;
  g_timer.start();
}

//############## R E N D E R #######################################################################
bool vsx_engine::render()
{
  if (!valid)
    return false;

  // check for time control requests from the modules
  if
  (
    engine_info.request_play == 1
    &&
    current_state != VSX_ENGINE_LOADING
  )
  {
    time_play();
  }

  if (engine_info.request_stop == 1)
  {
    time_stop();
  }

  if (engine_info.request_rewind == 1)
  {
    time_rewind();
  }

  if
  (
      current_state == VSX_ENGINE_STOPPED
      &&
      engine_info.request_set_time > 0.0f
  )
  {
    float dd = engine_info.vtime - engine_info.request_set_time;
    if (dd > 0)
    {
      engine_info.dtime = -dd;
    }
    else
    {
      engine_info.dtime = fabs(dd);
    }
  }

  if (!disabled)
  {
    frame_timer.start();

    float gtime = (float)g_timer.dtime();

    if (frame_cfp_time != 0.0f)
    {
      gtime = frame_cfp_time;
    }
    d_time = gtime * g_timer_amp;
    engine_info.real_dtime = d_time;
    engine_info.real_vtime += d_time;

    if (current_state == VSX_ENGINE_LOADING)
    {
      frame_start_time = g_timer.atime();
    }

    float d_time_i = d_time;
    float dt = 0;

    // this is the fmod time synchronizer
    if (frame_cfp_time == 0.0f)
    {
      for (unsigned long i = 0; i < outputs.size(); i++)
      {
        vsx_engine_param* param;
        param = outputs[i]->get_params_out()->get_by_name("_st");
        if (param) {
          vsx_module_param_float* fp = (vsx_module_param_float*)param->module_param;
          dt = fp->get();
          if (dt != -1.0f)
          {
            // we're getting some time from the module
            if (current_state == VSX_ENGINE_PLAYING)
            {
              if (last_m_time_synch == 0)
              {
                g_timer.start();
                if (engine_info.vtime == 0)
                d_time_i = dt;//dt-0.06;//dt - frame_prev_time;
                last_m_time_synch = 1;
              } else
              {
                d_time_i = d_time;
              }
            } else
            {
              d_time_i = 0;
            }
          } else
          {
            d_time_i = 0;
          }
        }
      }
    }

    // maintain time if state has changed
    if (current_state == VSX_ENGINE_REWIND)
    {
      engine_info.dtime = -engine_info.vtime;
      engine_info.vtime = 0;
      g_timer.start();
      current_state = VSX_ENGINE_STOPPED;
      goto post_state_change;
    }
    if (current_state == VSX_ENGINE_PLAYING)
    {
      engine_info.dtime = d_time_i;
    }
    engine_info.vtime += engine_info.dtime;

    post_state_change:

    // check if the loop point has been passed
    if (
        loop_point_end > 0.0f
        &&
        engine_info.vtime > loop_point_end
        )
    {
      engine_info.dtime += -loop_point_end;
      engine_info.vtime += -loop_point_end;
    }


    if (current_state == VSX_ENGINE_STOPPED)
      last_m_time_synch = 0;

    // propagate current state so modules can read it
    engine_info.state = current_state;

    // maintain the timer which keeps track of how often to send
    // a command to client with current time in it
    lastsent += engine_info.dtime;

    // maintain the fps counter data
    if (frame_dcount++ > frame_delta_fps_frame_count_interval)
    {
      frame_dcount = 0;
      frame_dtime = 0;
    }
    else
    {
      frame_dtime += (double)engine_info.vtime - frame_dprev;
      frame_delta_fps = 1/(frame_dtime/frame_dcount);
    }
    // save the vtime for next call to render()
    frame_dprev = engine_info.vtime;

    // advance the sequencer
    sequence_list.run(engine_info.dtime);

    // advance the sequence pool
    sequence_pool.run(engine_info.dtime);

    if (engine_time_from_sequence_pool)
      engine_info.vtime = sequence_pool.get_vtime();

    // run the parameter interpolators
    interpolation_list.run( (float)m_timer.dtime() );


    // render the state by iterating over the outputs
    for (unsigned long i = 0; i < outputs.size(); i++) {
      outputs[i]->prepare();
    }
    
    // post-rendering reset frame status of the components
    if (render_hint_post_render_reset_component_status)
    {
      for(std::vector<vsx_comp*>::iterator it = forge.begin(); it < forge.end(); ++it)
      {
        (*it)->reset_has_run_status();
      }
    }

    for(std::vector<vsx_comp*>::iterator it = forge.begin(); it < forge.end(); ++it)
    {
      (*it)->reset_frame_status();
    }


    // when we're loading, we need to reset every component
    if (current_state == VSX_ENGINE_LOADING)
    {
      modules_left_to_load = 0;
      modules_loaded = 0;
      for(std::vector<vsx_comp*>::iterator it = forge.begin(); it < forge.end(); ++it)
      {
        if ((*it)->component_class != "macro")
        if ((*it)->module)
        {
          if (!(*it)->module->loading_done)
          {
            vsx_module_specification foom;
            (*it)->module->module_info(&foom);

            LOG3(vsx_string<>("waiting for module: ")+foom.identifier+" with name: "+(*it)->name);

            ++modules_left_to_load;
          } else
          {
            ++modules_loaded;
          }
        }
      }

      if (modules_left_to_load == 0 && commands_internal.count() == 0)
      {
        current_state = VSX_ENGINE_PLAYING;
      }
    }

    //printf("MODULES LEFT TO LOAD: %d\n",i);
    last_frame_time = (float)frame_timer.dtime();

    if (current_state == VSX_ENGINE_STOPPED)
    {
      engine_info.dtime = 0.0f;
    }
    return true;
  }
  return false;
}


void vsx_engine::set_ignore_per_frame_time_limit(bool new_value)
{
  VSX_UNUSED(new_value);
}

void vsx_engine::process_message_queue(vsx_command_list *cmd_in, vsx_command_list *cmd_out_res, bool exclusive, bool ignore_timing, float max_time)
{
  if (!valid)
    return;
  // service commands
  LOG("process_message_queue 1")

  commands_res_internal.clear_normal();
  tell_client_time(cmd_out_res);
  vsx_command_s *c = 0;
  if (!exclusive) {
    while ( (c = commands_out_cache.pop()) )
      cmd_out_res->add(c);
  }
  // check for module requests

  if (current_state == VSX_ENGINE_LOADING)
  {
    process_message_queue_redeclare(cmd_out_res);
  }

  // handle exclusivity
  if (exclusive) cmd_in->set_type(1);
  // add the incoming commands to our own list for buffering (to continue on next frame if we don't have time to do them all)
  while ( (c = cmd_in->pop()) ) {
    commands_internal.add(c);
  }
  //commands_internal.reset();

  //---------------------------------------
  double total_time = 0.0;

  #define FAIL(header, message) 	cmd_out->add_raw(vsx_string<>("alert_fail ") + vsx_string_helper::base64_encode(#header)+" Error " + vsx_string_helper::base64_encode(#message))

  vsx_command_timer.start();

  vsx_command_list* cmd_out = cmd_out_res;

  max_time = 120.0f;

  while (total_time < (double)max_time || ignore_timing)
  {
    c = commands_internal.pop();

    // handle null pointers
    if (!c)
      break;

    // break command
    if (c->cmd == "break")
    {
      if (!c->garbage_collected)
        delete c;
      return;
    }

    // internal command
    if (c->type == 1)
      cmd_out = &commands_res_internal;

    // process main message
    #define cmd c->cmd
    #define cmd_data c->cmd_data

    #include "vsx_engine_messages/vsx_saveload.h"
    #include "vsx_engine_messages/vsx_em_comp.h"
    #include "vsx_engine_messages/vsx_connections.h"
    #include "vsx_engine_messages/vsx_parameters.h"
    #include "vsx_engine_messages/vsx_em_sequencer.h"
    #include "vsx_engine_messages/vsx_em_macro.h"
    #include "vsx_engine_messages/vsx_em_seq_pool.h"
    #include "vsx_engine_messages/vsx_engine_time.h"
    #include "vsx_engine_messages/vsx_em_script.h"
    #include "vsx_engine_messages/vsx_note.h"
    #include "vsx_engine_messages/vsx_em_system.h"
    #include "vsx_engine_messages/vsx_em_module_operation.h"

    if (cmd == "help")
      goto process_message_queue_end;

    // this shouldn't be reached unless no command is performed and thus jumped
    // to process_message_queue_end
    cmd_out->add_raw("invalid_command unknown_command");


    #undef cmd
    #undef cmd_data

    process_message_queue_end:

    if (current_state != VSX_ENGINE_LOADING)
      process_message_queue_redeclare(cmd_out_res);

    if (!c->garbage_collected)
      delete c;

    total_time += vsx_command_timer.dtime();
  }

} // process_comand_queue


float vsx_engine::get_last_frame_time()
{
  return last_frame_time;
}

double vsx_engine::get_fps()
{
  return frame_delta_fps;
}


void vsx_engine::unload_state()
{
  i_clear();

  if (filesystem.get_archive()->is_archive())
    filesystem.get_archive()->close();
}



// VSX_ENGINE
