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

#ifndef VSX_COMP_CHANNEL_H
#define VSX_COMP_CHANNEL_H

class vsx_channel;

#include <vector>
#include "vsx_timer.h"

class vsx_comp;

class vsx_engine_param_connection;
class vsx_engine_param;
class vsx_engine_param_list;

class vsx_channel_connection_info {
public:
  // the abstract connection, we need this to update their order id's when we update order
  vsx_engine_param_connection* engine_connection;
/*  // the abstract parameter, we need this to re-create abstract connections when all but the channel connection
  // has been removed when a component is moved from one macro to another
  vsx_engine_param* engine_param;*/
  
  // the source component for our connection
  vsx_comp* src_comp;
  // the module_param of the source component - the "real" param
  vsx_module_param_abs* module_param;
  // constructor
  vsx_channel_connection_info() : src_comp(0),module_param(0) {};
};

class vsx_channel {
public:
	vsx_timer int_timer;
  float channel_execution_time;
  // type id in the vsx_param_abs (module parameters)
	unsigned long type;
	unsigned long max_connections;
	// our connections to other components
	std::vector<vsx_channel_connection_info*> connections;
	// our component
	vsx_comp* component;
	// our module
	vsx_module* my_module;
  // our engine param
	vsx_engine_param* my_param;
	
	
	vsx_channel(vsx_module* module,vsx_engine_param* param, int mcon, vsx_comp* pare);

  // connect to another param
	vsx_channel_connection_info* connect(vsx_engine_param* src);
	bool disconnect(vsx_engine_param* src);
	
	// deal with the orders
	void update_connections_order();

  // deprecated connection functions
	//bool connect(vsx_comp* comp, vsx_string name);
	bool disconnect(vsx_comp* comp, vsx_string name);
	void disconnect();
	bool connections_order(std::vector<int> *order_list);
	
	
	vsx_string get_param_name();
	virtual bool execute() = 0;
	virtual ~vsx_channel();
};


class vsx_channel_render : public vsx_channel {
public:
	bool execute();
	vsx_channel_render(vsx_module* module,vsx_engine_param* param, vsx_comp* pare) : vsx_channel(module,param,100,pare) {	component = pare;}
};


#define NEW_DEFAULT_CHANNEL(name, mcon) \
class name : public vsx_channel { \
public: \
	bool execute(); \
	name(vsx_module* module,vsx_engine_param* param, vsx_comp* pare) : vsx_channel(module,param,mcon,pare) {	component = pare;} \
};
NEW_DEFAULT_CHANNEL(vsx_channel_texture, 1)
NEW_DEFAULT_CHANNEL(vsx_channel_int, 1)
NEW_DEFAULT_CHANNEL(vsx_channel_float, 1)
NEW_DEFAULT_CHANNEL(vsx_channel_float3, 1)
NEW_DEFAULT_CHANNEL(vsx_channel_float4, 1)
NEW_DEFAULT_CHANNEL(vsx_channel_quaternion, 1)
NEW_DEFAULT_CHANNEL(vsx_channel_matrix, 1)
NEW_DEFAULT_CHANNEL(vsx_channel_mesh, 1)
NEW_DEFAULT_CHANNEL(vsx_channel_segment_mesh, 1)
NEW_DEFAULT_CHANNEL(vsx_channel_bitmap, 1)
NEW_DEFAULT_CHANNEL(vsx_channel_particlesystem, 1)
NEW_DEFAULT_CHANNEL(vsx_channel_float_array, 1)
NEW_DEFAULT_CHANNEL(vsx_channel_float3_array, 1)
NEW_DEFAULT_CHANNEL(vsx_channel_quaternion_array, 1)
NEW_DEFAULT_CHANNEL(vsx_channel_string, 1)
NEW_DEFAULT_CHANNEL(vsx_channel_resource, 1)
NEW_DEFAULT_CHANNEL(vsx_channel_sequence, 1)

#endif
