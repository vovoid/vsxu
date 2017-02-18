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
#include "vsx_param.h"
#include <module/vsx_module.h>
#include <time/vsx_timer.h>
#include <internal/vsx_comp_channel.h>
#include <internal/vsx_comp_abs.h>
#include <internal/vsx_param_abstraction.h>
#include <internal/vsx_comp.h>

#include <stdio.h>

using namespace	std;

vsx_channel::vsx_channel(vsx_module* module,vsx_engine_param* param, int mcon, vsx_comp* pare)
{
	my_module = module;
	my_param = param;
	type = param->module_param->type;
	max_connections = mcon;
	component = pare;
}
vsx_channel::~vsx_channel()
{
  for (std::vector<vsx_channel_connection_info*>::iterator it = connections.begin(); it != connections.end(); ++it)
  {
    delete *it;
  }  
}
  
vsx_string<>vsx_channel::get_param_name()
{
	return my_param->name;
}

vsx_channel_connection_info* vsx_channel::connect(vsx_engine_param* src)
{
  unsigned long type_id = src->module_param->type;
//  cout << type_id << endl;
//  cout << type << endl;
  if (connections.size() < max_connections && type_id == type)
  {
    for (vector<vsx_channel_connection_info*>::iterator it=connections.begin(); it != connections.end(); ++it)
    {
      if ((*it)->src_comp == src->owner->component) return 0;
    }
    // create the connection
    vsx_channel_connection_info* ci = new vsx_channel_connection_info;
    ci->module_param = src->module_param;
    ci->src_comp = (vsx_comp*)src->owner->component;
    connections.push_back(ci);
    // set both ends module params as connected
    src->module_param->connected = true;
    my_param->module_param->connected = true;
    return ci;
  } else return 0; 
  return 0;
}


bool vsx_channel::disconnect(vsx_engine_param* src)
{
  for (vector<vsx_channel_connection_info*>::iterator it=connections.begin(); it != connections.end(); ++it)
  {
    if ((*it)->module_param == src->module_param)
    {
      // set both ends module params as disconnected
      src->module_param->connected = false;
      my_param->module_param->connected = false;
      // set my module param to invalid
      my_param->module_param->valid = false;
      delete *it;
      *it = 0;
      connections.erase(it);
      return true;
    }
  }  
  return false;
}  

void vsx_channel::update_connections_order()
{
  int i = 0;
  for (vector<vsx_channel_connection_info*>::iterator it=connections.begin(); it != connections.end(); ++it)
  {
    (*it)->engine_connection->connection_order = i;
    ++i;
  }  
}  

//---------------------------------------------------------

bool vsx_channel::disconnect(vsx_comp* comp, vsx_string<>name)
{
//  connections.clear();
//  return true;
  if (connections.size() > 0)
  {
    vsx_engine_param* param = comp->get_params_out()->get_by_name(name);
    for (vector<vsx_channel_connection_info*>::iterator it = connections.begin(); it != connections.end(); ++it)
    {
      if ((*it)->src_comp == (*it)->engine_connection->owner->owner->component && (*it)->module_param == param->module_param)
      {
        connections.erase(it);
        delete *it;
        return true;
      }
		}
		return false;
	} 
	return false;
}

void vsx_channel::disconnect()
{
  if (connections.size())
  {
    //for (vector<vsx_channel_connection_info*>::iterator it=connections.begin(); it != connections.end(); ++it)
    //{
      //(*it)->comp->out_map_channels[(*it)->param].remove(this);
    //}
  	connections.clear();
	}
}


bool vsx_channel::connections_order(std::vector<int> *order_list)
{
  if (order_list->size() != connections.size()) return false;
  std::vector<vsx_channel_connection_info*> new_connections;
  int n = 0;
  for (std::vector<int>::iterator it = order_list->begin(); it != order_list->end(); ++it)
  {
    connections[*it]->engine_connection->connection_order = n;
    new_connections.push_back(connections[*it]);
    ++n;
  }
  connections = new_connections;
  return true;
}

//----------------------------------------------------------------------------------------

bool vsx_channel_render::execute()
{
  #ifdef VSXU_MODULE_TIMING
    channel_execution_time = 0.0f;
    int_timer.start();
  #endif
  if(connections.size() == 0)
  {
    if (my_param->critical) return false; else return true;
  }
  if ( my_param->module_param->run_activate_offscreen && !my_module->activate_offscreen()) return false;

	vector<vsx_channel_connection_info*>::iterator it;
  // printf("channel:render:this-name: %s\n",component->name.c_str());
	// call all components' prepare in our connections list
  #ifdef VSXU_MODULE_TIMING
    channel_execution_time += (float)int_timer.dtime();
  #endif
	for (it = connections.begin(); it < connections.end(); ++it) 
  {
    //printf("channel:render:calling-prepare %s\n",(*it)->comp->name.c_str());
    if (!(((*it)->src_comp)->module_info->tunnel))
		if (!(*it)->src_comp->prepare() && my_param->all_required) return false;
	}
	// call the module's prepare
	// The module will here set up the viewport to render to
	//if (prepare_module == 0)
	//my_module->prepare();
  // printf("channel:render:prepare-complete\n");
  #ifdef VSXU_MODULE_TIMING
    int_timer.start();
  #endif
	// call all [connections] components run functions
	// thiss will make them render onto our now setup surface
	for (it = connections.begin(); it < connections.end(); ++it) 
  {
    //printf("channel:render:pre-module-run\n");
		(*it)->src_comp->run((*it)->module_param);
    //printf("channel:render:post-module-run\n");
    ((vsx_module_param_render*)my_param->module_param)->set_internal(((vsx_module_param_render*)(*it)->module_param)->get());
    //printf("channel:render:post-param_set\n");
	}
  if (my_param->module_param->run_activate_offscreen) my_module->deactivate_offscreen();
  #ifdef VSXU_MODULE_TIMING
    channel_execution_time += (float)int_timer.dtime();
  #endif
	return true;
}

bool vsx_channel_texture::execute() { 
	if(connections.size() == 0) {
    return !my_param->critical;// return false; else return true; 
  }

	vector<vsx_channel_connection_info*>::iterator it; 
	for (it = connections.begin(); it < connections.end(); ++it) 
  { 
    (*it)->src_comp->prepare();
		//if(!(*it)->src_comp->prepare() && my_param->all_required) return false; 
	} 
	//if (prepare_module == 0) 
	//my_module->prepare(); 
	for (it = connections.begin(); it < connections.end(); ++it) 
  { 
    //(*it)->src_comp->run();
		if(!(*it)->src_comp->run((*it)->module_param) && my_param->all_required) 
      return false; 

    ((vsx_module_param_texture*)my_param->module_param)
        ->set_internal_from_param(((vsx_module_param_texture*)(*it)->module_param));
    if (((vsx_module_param_texture*)my_param->module_param)->get_addr() &&
      (unsigned long long)((vsx_module_param_texture*)my_param->module_param)->get()->texture > 0xd000000000000000)
    {
      vsx_printf(L"invalid pointer\n");
    }
	} 
	return true;
}


//----------------------------------------------------------------------------------------
#define NEW_DEFAULT_CHANNEL_EXECUTE(name, type) \
bool name::execute() { \
  \
	if(connections.size() == 0) { \
    if (my_param->critical) return false; else return true; \
  } \
  if (my_param->module_param->run_activate_offscreen && !my_module->activate_offscreen()) return false;\
	vector<vsx_channel_connection_info*>::iterator it; \
	for (it = connections.begin(); it != connections.end(); ++it) \
  { \
		if(!(*it)->src_comp->prepare() && my_param->all_required) return false; \
	} \
	for (it = connections.begin(); it != connections.end(); ++it) \
  { \
		if(!(*it)->src_comp->run((*it)->module_param) && my_param->all_required) return false; \
    ((type*)my_param->module_param)\
        ->set_internal_from_param((type*)(*it)->module_param); \
	} \
  if (my_param->module_param->run_activate_offscreen) my_module->deactivate_offscreen();\
	++my_param->module_param->updates;\
	return true; \
}

#define NEW_DEFAULT_CHANNEL_EXECUTE_INC_PARAM_UPDATES(name, type) \
bool name::execute() { \
  \
	if(connections.size() == 0) { \
    if (my_param->critical) return false; else return true; \
  } \
  if (my_param->module_param->run_activate_offscreen && !my_module->activate_offscreen()) return false;\
	vector<vsx_channel_connection_info*>::iterator it = connections.begin(); \
  if(!(*it)->src_comp->prepare() && my_param->all_required) return false; \
  if(!(*it)->src_comp->run((*it)->module_param) && my_param->all_required) return false; \
  (((type*)my_param->module_param)->set_internal_from_param((type*)(*it)->module_param)); \
	  ++my_module->param_updates;\
	  ++my_param->module_param->updates;\
  if (my_param->module_param->run_activate_offscreen) my_module->deactivate_offscreen();\
	return true; \
}

#define NEW_DEFAULT_CHANNEL_EXECUTE_INC_PARAM_UPDATES_WITH_VALUE_CHECK(name, type) \
bool name::execute() { \
  \
  if(connections.size() == 0) { \
    if (my_param->critical) return false; else return true; \
  } \
  if (my_param->module_param->run_activate_offscreen && !my_module->activate_offscreen()) return false;\
  vector<vsx_channel_connection_info*>::iterator it = connections.begin(); \
  if(!(*it)->src_comp->prepare() && my_param->all_required) return false; \
  if(!(*it)->src_comp->run((*it)->module_param) && my_param->all_required) return false; \
  if (((type*)my_param->module_param)->set_internal_from_param_with_value_check((type*)(*it)->module_param)) \
  {\
    ++my_module->param_updates;\
    ++my_param->module_param->updates;\
  }\
  if (my_param->module_param->run_activate_offscreen) my_module->deactivate_offscreen();\
  return true; \
}

NEW_DEFAULT_CHANNEL_EXECUTE_INC_PARAM_UPDATES_WITH_VALUE_CHECK(vsx_channel_int, vsx_module_param_int)
NEW_DEFAULT_CHANNEL_EXECUTE_INC_PARAM_UPDATES_WITH_VALUE_CHECK(vsx_channel_float, vsx_module_param_float)
NEW_DEFAULT_CHANNEL_EXECUTE_INC_PARAM_UPDATES_WITH_VALUE_CHECK(vsx_channel_float3, vsx_module_param_float3)
NEW_DEFAULT_CHANNEL_EXECUTE_INC_PARAM_UPDATES_WITH_VALUE_CHECK(vsx_channel_float4, vsx_module_param_float4)
NEW_DEFAULT_CHANNEL_EXECUTE_INC_PARAM_UPDATES_WITH_VALUE_CHECK(vsx_channel_quaternion, vsx_module_param_quaternion)
NEW_DEFAULT_CHANNEL_EXECUTE_INC_PARAM_UPDATES(vsx_channel_matrix, vsx_module_param_matrix)
NEW_DEFAULT_CHANNEL_EXECUTE(vsx_channel_mesh, vsx_module_param_mesh)
NEW_DEFAULT_CHANNEL_EXECUTE(vsx_channel_bitmap, vsx_module_param_bitmap)
NEW_DEFAULT_CHANNEL_EXECUTE(vsx_channel_particlesystem, vsx_module_param_particlesystem)
NEW_DEFAULT_CHANNEL_EXECUTE(vsx_channel_float_array, vsx_module_param_float_array)
NEW_DEFAULT_CHANNEL_EXECUTE(vsx_channel_float3_array, vsx_module_param_float3_array)
NEW_DEFAULT_CHANNEL_EXECUTE(vsx_channel_quaternion_array, vsx_module_param_quaternion_array)
NEW_DEFAULT_CHANNEL_EXECUTE(vsx_channel_string, vsx_module_param_string)
NEW_DEFAULT_CHANNEL_EXECUTE(vsx_channel_resource, vsx_module_param_resource)
NEW_DEFAULT_CHANNEL_EXECUTE(vsx_channel_sequence, vsx_module_param_float_sequence)

bool vsx_channel_segment_mesh::execute()
{
  if(connections.size() == 0)
  {
    if (my_param->critical) return false; else return true; 
  } 
  if (my_param->module_param->run_activate_offscreen && !my_module->activate_offscreen()) return false;
	vector<vsx_channel_connection_info*>::iterator it; 
  for (it = connections.begin(); it != connections.end(); ++it)
  { 
		if(!(*it)->src_comp->prepare() && my_param->all_required) return false; 
	} 

  for (it = connections.begin(); it != connections.end(); ++it)
  { 
    if(!(*it)->src_comp->run((*it)->module_param) && my_param->all_required)
    {
      return false; 
    }
    ((vsx_module_param_segment_mesh*)my_param->module_param)->
        set_internal_from_param((vsx_module_param_segment_mesh*)(*it)->module_param);
	} 
  if (my_param->module_param->run_activate_offscreen) my_module->deactivate_offscreen();
	return true; 
}
