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

#include <string/vsx_string.h>
#include <command/vsx_command.h>
#include <command/vsx_command_list.h>
#include <time/vsx_timer.h>
#include "vsx_engine.h"
#include <internal/vsx_comp_channel.h>
#include <internal/vsx_param_abstraction.h>
#include <internal/vsx_comp.h>
#include <internal/vsx_param_interpolation.h>
#include <internal/vsx_param_sequence.h>
#include <internal/vsx_master_sequence_channel.h>
#include <internal/vsx_param_sequence_list.h>
#include <internal/vsx_sequence_pool.h>
#include <filesystem/vsx_filesystem.h>
#include <internal/vsx_param_abstraction.h>
#include "tools/vsx_foreach.h"

#ifndef VSXE_NO_GM
#include "scripting/vsx_param_vsxl.h"
#include "scripting/vsx_comp_vsxl.h"
#endif

#include <internal/vsx_comp_channel.h>
#include <log/vsx_log.h>

vsx_comp::vsx_comp() {
  module = 0;
  module_info = new vsx_module_specification;
  vsxl_modifier = 0;
  internal_critical = false;
  size = 0.05f;
  frame_status = initial_status;
  in_parameters = new vsx_engine_param_list;
  in_parameters->component = this;
  in_parameters->io = -1;
  out_parameters = new vsx_engine_param_list;
  out_parameters->component = this;
  out_parameters->io = 1;
  parent = 0;
  all_valid = true;
  time_multiplier = 1.0f;
  in_module_parameters = new vsx_module_param_list;
  out_module_parameters = new vsx_module_param_list;
  has_run = false;
}

vsx_comp::~vsx_comp()
{
  delete in_module_parameters;
  delete out_module_parameters;
  delete module_info;
  delete in_parameters;
  delete out_parameters;
  for (std::vector <vsx_channel*>::iterator it = channels.begin(); it != channels.end(); ++it) {
    delete *it;
  }
  #ifndef VSXE_NO_GM
    if (vsxl_modifier) delete (vsx_comp_vsxl*)vsxl_modifier;
  #endif
}

void vsx_comp::load_module(const vsx_string<>& module_name, vsx_module_engine_state* engine_info)
{
  vsx_module_list_abs* module_list = ((vsx_engine*)engine_owner)->get_module_list();
  module = module_list->load_module_by_name( module_name );
  r_engine_info = engine_info;

  if (module)
  {
    init_module();
  } else
  {
    printf("vsx_comp::load_module failed\n");
  }
}

void vsx_comp::unload_module()
{
  LOG("before unload1")
  if (module)
  {
    module->on_delete();
    module->destroy_operations( module_operations );
  }
  vsx_module_list_abs* module_list = ((vsx_engine*)engine_owner)->get_module_list();
  module_list->unload_module( module );
  module = 0;
}

void vsx_comp::re_init_in_params() {
  // this doesn't EVER happen on its own (or shouldn't)
  // so we can be a bit sloppy with optimizations here...
  // It's initiated by the user changing stuff in the GUI most likely
  std::map<vsx_string<>, float> float_values_saved;
  std::map<vsx_string<>, int> int_values_saved;

  typedef vsx::sequence::channel<vsx::sequence::value_float> float_sequence;
  std::map<vsx_string<>, float_sequence > float_sequence_values_saved;	// technical assumption:
  // most parameters are usually declared with the same name and type.
  // so: save the easy ones (float, int, string, float3 etc).
  for (unsigned long i = 0; i < in_module_parameters->id_vec.size(); i++)
  {
    vsx_module_param_abs* mparam = in_module_parameters->id_vec[i];
    switch (mparam->type)
    {
      case VSX_MODULE_PARAM_ID_INT:
      {
        int_values_saved[mparam->name] = ((vsx_module_param_int*)mparam)->get();
      }
      break;
      case VSX_MODULE_PARAM_ID_FLOAT:
      {
        float_values_saved[mparam->name] = ((vsx_module_param_float*)mparam)->get();
      }
      break;
      case VSX_MODULE_PARAM_ID_FLOAT_SEQUENCE:
      {
        vsx::sequence::channel<vsx::sequence::value_float> a = ((vsx_module_param_float_sequence*)mparam)->get();
        float_sequence_values_saved[mparam->name] = a;
      }
      break;
      default:
      break;
    };
  }

  delete in_parameters;
  delete in_module_parameters;

  // delete all channels
  for (std::vector <vsx_channel*>::iterator it = channels.begin(); it != channels.end(); ++it) {
    delete *it;
  }
  channels.clear();
  in_module_parameters = new vsx_module_param_list;
  module->redeclare_in_params(*in_module_parameters);
  module->module_info(module_info);
  in_param_spec = process_module_param_spec(module_info->in_param_spec);
  in_parameters = new vsx_engine_param_list;
  in_parameters->component = this;
  in_parameters->io = -1;
  in_parameters->init(in_module_parameters);
  init_channels();

  // restore the values kthx;; this code will prolly compress well
#define RESTORE_FUNC(i_type) \
  for (std::map<vsx_string<>, i_type>::iterator it = i_type##_values_saved.begin(); it != i_type##_values_saved.end();\
      it++)\
  {\
    for (unsigned long i = 0; i < in_module_parameters->id_vec.size(); i++)\
    {\
      if (in_module_parameters->id_vec[i]->name == (*it).first)\
      {\
        ((vsx_module_param_##i_type*)in_module_parameters->id_vec[i])->set((*it).second);\
        ((vsx_module_param_##i_type*)in_module_parameters->id_vec[i])->updates++;\
      }\
    }\
  }
  RESTORE_FUNC(float);
  RESTORE_FUNC(int);
  RESTORE_FUNC(float_sequence);

#undef RESTORE_FUNC

}

void vsx_comp::re_init_out_params() {
  // delete all channels
  delete out_parameters;
  delete out_module_parameters;

  out_module_parameters = new vsx_module_param_list;
  module->redeclare_out_params(*out_module_parameters);
  module->module_info(module_info);
  out_param_spec = process_module_param_spec(module_info->out_param_spec);
  out_parameters = new vsx_engine_param_list;
  out_parameters->component = this;
  out_parameters->io = 1;
  out_parameters->init(out_module_parameters);
}

void vsx_comp::init_channels() {
  // we need to set up channels
  for (std::vector<vsx_engine_param*>::iterator it = in_parameters->param_id_list.begin(); it != in_parameters->param_id_list.end(); ++it) {
    vsx_engine_param* param = *it;
    param->module_param->set_current_as_default();

    if (param->module_param->type == VSX_MODULE_PARAM_ID_RENDER) {
      if (!param->module_param->valid) ((vsx_module_param_render*)param->module_param)->set(0);
      channels.push_back(param->channel = new vsx_channel_render(module,param,this));
    } else
    if (param->module_param->type == VSX_MODULE_PARAM_ID_TEXTURE) {
      channels.push_back(param->channel = new vsx_channel_texture(module,param,this));
    } else
    if (param->module_param->type == VSX_MODULE_PARAM_ID_FLOAT3) {
      if (!param->module_param->valid) {
        ((vsx_module_param_float3*)param->module_param)->set(0.0f,0);
        ((vsx_module_param_float3*)param->module_param)->set(0.0f,1);
        ((vsx_module_param_float3*)param->module_param)->set(0.0f,2);
      }
      channels.push_back(param->channel = new vsx_channel_float3(module,param,this));
    } else
    if (param->module_param->type == VSX_MODULE_PARAM_ID_FLOAT4) {
      channels.push_back(param->channel = new vsx_channel_float4(module,param,this));
      if (!param->module_param->valid) {
        ((vsx_module_param_float4*)param->module_param)->set(0.0f,0);
        ((vsx_module_param_float4*)param->module_param)->set(0.0f,1);
        ((vsx_module_param_float4*)param->module_param)->set(0.0f,2);
        ((vsx_module_param_float4*)param->module_param)->set(0.0f,3);
      }
    } else
    if (param->module_param->type == VSX_MODULE_PARAM_ID_QUATERNION) {
      channels.push_back(param->channel = new vsx_channel_quaternion(module,param,this));
      if (!param->module_param->valid) {
        ((vsx_module_param_quaternion*)param->module_param)->set(0.0f,0);
        ((vsx_module_param_quaternion*)param->module_param)->set(0.0f,1);
        ((vsx_module_param_quaternion*)param->module_param)->set(0.0f,2);
        ((vsx_module_param_quaternion*)param->module_param)->set(1.0f,3);
      }
    } else
    if (param->module_param->type == VSX_MODULE_PARAM_ID_FLOAT) {
      if (!param->module_param->valid) ((vsx_module_param_float*)param->module_param)->set(0.0f);
      channels.push_back(param->channel = new vsx_channel_float(module,param,this));
    } else
    if (param->module_param->type == VSX_MODULE_PARAM_ID_INT) {
      if (!param->module_param->valid) ((vsx_module_param_int*)param->module_param)->set(0);
      channels.push_back(param->channel = new vsx_channel_int(module,param,this));
    } else
    if (param->module_param->type == VSX_MODULE_PARAM_ID_MESH) {
      channels.push_back(param->channel = new vsx_channel_mesh(module,param,this));
    } else
    if (param->module_param->type == VSX_MODULE_PARAM_ID_MATRIX) {
      channels.push_back(param->channel = new vsx_channel_matrix(module,param,this));
    } else
    if (param->module_param->type == VSX_MODULE_PARAM_ID_SEGMENT_MESH) {
      channels.push_back(param->channel = new vsx_channel_segment_mesh(module,param,this));
    } else
    if (param->module_param->type == VSX_MODULE_PARAM_ID_BITMAP) {
      channels.push_back(param->channel = new vsx_channel_bitmap(module,param,this));
    } else
    if (param->module_param->type == VSX_MODULE_PARAM_ID_PARTICLESYSTEM) {
      channels.push_back(param->channel = new vsx_channel_particlesystem(module,param,this));
    } else
    if (param->module_param->type == VSX_MODULE_PARAM_ID_FLOAT_ARRAY) {
      channels.push_back(param->channel = new vsx_channel_float_array(module,param,this));
    } else
    if (param->module_param->type == VSX_MODULE_PARAM_ID_FLOAT3_ARRAY) {
      channels.push_back(param->channel = new vsx_channel_float3_array(module,param,this));
    } else
    if (param->module_param->type == VSX_MODULE_PARAM_ID_QUATERNION_ARRAY) {
      channels.push_back(param->channel = new vsx_channel_quaternion_array(module,param,this));
    } else
    if (param->module_param->type == VSX_MODULE_PARAM_ID_STRING) {
      channels.push_back(param->channel = new vsx_channel_string(module,param,this));
    } else
    if (param->module_param->type == VSX_MODULE_PARAM_ID_RESOURCE) {
      channels.push_back(param->channel = new vsx_channel_resource(module,param,this));
    } else
    if (param->module_param->type == VSX_MODULE_PARAM_ID_FLOAT_SEQUENCE) {
      channels.push_back(param->channel = new vsx_channel_sequence(module,param,this));
    }
  }
}

void vsx_comp::init_module()
{
  module->engine_state = r_engine_info;
  module->declare_params(*in_module_parameters, *out_module_parameters);
  LOG("init_module 1")
  module->module_info(module_info);
  in_param_spec = process_module_param_spec(module_info->in_param_spec);
  out_param_spec = process_module_param_spec(module_info->out_param_spec);
  component_class = module_info->component_class;

  module->declare_operations( module_operations );

  LOG("init_module 2")

  LOG(vsx_string<>("module param spec: ")+in_param_spec.c_str());

  // build up the internal engine_param_list
  in_parameters->init(in_module_parameters);
  LOG("init_module 3")
  out_parameters->init(out_module_parameters);
  init_channels();
  LOG("init_module 4")
  module->init();
  LOG("init_module finished")
}


bool vsx_comp::prepare()
{
  if (parent)
  {
    LOG("comp prepare name: "+name+" of "+((vsx_comp_abs*)parent)->name)
  }
  else
  {
    LOG("comp prepare name: "+name)
  }
  if (frame_status == frame_failed)
  {
    return false;
  }
  if (frame_status != initial_status)
  {
    return true;
  }
  frame_status = prepare_called;
  // it needs to prepare all parameters for the run function
  // this means it has to execute all channels to get texture id's etc
  unsigned long i = 0;
  for (std::vector <vsx_channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
  {
    if ((*it)->my_param->critical && !(*it)->connections.size()) {
      // this channel is critical but not connected! can't run!
      i = 1; break;
    }
  }
  if (i)
  {
    for (i = 0; i < out_module_parameters->id_vec.size(); ++i)
    {
      out_module_parameters->id_vec[i]->valid = false;
    }
    all_valid = false;
  } else
  {
    if (!all_valid) {
      for (i = 0; i < out_module_parameters->id_vec.size(); ++i)
      {
        out_module_parameters->id_vec[i]->valid = true;
      }
      all_valid = true;
    }
  }

  for (std::vector <vsx_channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
  {
    // check time to speed up loading bar
    if (r_engine_info->state == VSX_ENGINE_LOADING)
    {
      //LOG("engine is loading, time to test")
      //double atime = ((vsx_engine*)engine_owner)->g_timer.atime();
      //LOG("atime: "+vsx_string_helper::f2s(atime));
      //LOG("fstarttime: "+vsx_string_helper::f2s(((vsx_engine*)engine_owner)->frame_start_time));
      //printf("frame_start_time: %f\n",vsx_string_helper::f2s(((vsx_engine*)engine_owner)->frame_start_time));
      if
      (
        ((vsx_engine*)engine_owner)->get_frame_elapsed_time() > 0.4
      )
      {
        LOG("timer return")
        return false;
      }
    }
    //---
    //if i is 0 (on the first run) prepare the module
    if (!(*it)->execute())
    {
      frame_status = frame_failed;
      //printf("failed channel execute : %s\n",name.c_str());
      return false;
    }
    #ifdef VSXU_MODULE_TIMING
      new_time_run += (*it)->channel_execution_time;
    #endif
    // run vsxl after other component has set our value
    #ifndef VSXE_NO_GM
      if ((*it)->my_param->module_param->vsxl_modifier)
      {
        void* a = (*it)->my_param->module_param->vsxl_modifier;
        ((vsx_param_vsxl_abs*)a)  -> execute();
      }
    #endif
    ++i;
  }
  if (module_info->output)
  {
    LOG("module->run");
    #ifdef VSXU_MODULE_TIMING
      run_timer.start();
    #endif
      // don't run run() if engine is in output mode
      if ( false == ((vsx_engine*)engine_owner)->get_render_hint_module_output_only() )
      {
        module->run();
      }
    #ifdef VSXU_MODULE_TIMING
      new_time_run += run_timer.dtime();
    #endif
  }
  // special case for output components (screen etc.)
  if (module_info->output)
  {
    frame_status = run_finished;
  }
  frame_status = prepare_finished;
  return true;
}

bool vsx_comp::run(vsx_module_param_abs* param)
{
  LOG(vsx_string<>("run:name=")+name.c_str());

  if (module_info->output) {
    return true;
  }
  if (module_info->tunnel) {
    // very extra param!
    frame_status = initial_status;
    prepare();
  }

  if(frame_status == frame_failed) return false;

  if(frame_status == prepare_finished) {
    //printf("c:%s:module_pre_run\n",name.c_str());
    #ifndef VSXE_NO_GM
      if (vsxl_modifier) {
        ((vsx_comp_vsxl*)vsxl_modifier)->execute();
      }
    #endif
    #ifdef VSXU_MODULE_TIMING
      run_timer.start();
    #endif
    if (
      false == ((vsx_engine*)engine_owner)->get_render_hint_module_output_only()
      ||
      false == has_run
    )
    {
      module->run();
      has_run = true;
    }
    #ifdef VSXU_MODULE_TIMING
      new_time_run += run_timer.dtime();
    #endif
    //printf("%s new_time_run = %f\n",name.c_str(),new_time_run);
    //printf("c:%s:module_post_run\n",name.c_str());

    if (module_info->tunnel)
    frame_status = initial_status; else
    frame_status = run_finished;
  }
  //printf("c:%s:module_pre_output\n",name.c_str());
  #ifdef VSXU_MODULE_TIMING
    run_timer.start();
  #endif
    module->output(param);
  #ifdef VSXU_MODULE_TIMING
    new_time_output += run_timer.dtime();
  #endif
  return true;
}

bool vsx_comp::stop() {
  //printf("stopping %s\n",name.c_str());
  if (module)
  module->stop();
  return true;
}

bool vsx_comp::start()
{
  if (module)
  module->start();
  return true;
}

bool vsx_comp::disconnect(vsx_string<>param_name, vsx_comp_abs* other_component, vsx_string<>other_param_name) {
  for (unsigned long i = 0; i < channels.size(); ++i) {
    if (param_name == channels[i]->get_param_name()) {
      return channels[i]->disconnect((vsx_comp*)other_component,other_param_name);
    }
  }
  return false;
}

void vsx_comp::disconnect(vsx_string<>param_name) {
  for (unsigned long i = 0; i < channels.size(); ++i) {
    if (param_name == channels[i]->get_param_name()) {
        channels[i]->disconnect();
    }
  }
}

vsx_string<>process_module_param_spec(vsx_string<>& input) {
  vsx_string<>ret_val;
  size_t i = 0;
  bool block = false;
  vsx_string<>s_block;
  while (i < input.size()) {
    if (!block) {
      if (input[i] == '`') {
        block = true;
      } else {
        switch (input[i]) {
          case ' ':
          case 0x09:
          case 0x0A:
          case 0x0D:
          break;
          default:
          ret_val.push_back(input[i]);
        }
      }
    } else
    {
      // inside a block
      if (input[i] == '`') {
        // end of block
        ret_val += vsx_string_helper::base64_encode(s_block);
        s_block = "";
        block = false;
      } else
      {
        s_block.push_back(input[i]);
      }
    }
    ++i;
  }
  return ret_val;
}

vsx_string<> vsx_comp::module_operations_as_string()
{
  vsx_string<> ret;
  foreach(module_operations, i)
  {
    if (i)
      ret += '#';
    ret += module_operations[i]->serialize();
  }
  return ret;
}

bool vsx_comp::has_module_operations()
{
  return module_operations.size() > 0;
}

void vsx_comp::module_operation_run(vsx_module_operation& operation)
{
  module->run_operation( operation );
}
