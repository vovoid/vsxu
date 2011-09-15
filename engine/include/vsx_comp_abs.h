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

#ifndef VSX_COMP_ABS_H
#define VSX_COMP_ABS_H

class vsx_engine_param_list;

class vsx_comp_abs {
public:
  // the name of the component
  vsx_string name;
  // our "real" parameter lists for internal use

  vsx_engine_param_list* in_parameters;
  vsx_engine_param_list* out_parameters;
	// return a pointer to the paramlist
	vsx_engine_param_list* get_params_in() { return in_parameters; }
	vsx_engine_param_list* get_params_out() { return out_parameters; }
	
	void* engine_owner;

  vsx_comp_abs* parent;

  vsx_module_engine_info* r_engine_info;
//vsx_module_engine_info local_engine_info;
  // time warp
  float time_multiplier;
  
  // timing - for performance debugging
#ifdef VSXU_MODULE_TIMING  
  double time_run;
  double time_output;

  double new_time_run;
  double new_time_output;
#endif  
 	vsx_module* module;
  vsx_module_info* module_info;
  vsx_string identifier;
  vsx_string component_class;
  vsx_string in_param_spec;
  vsx_string out_param_spec;
  //virtual bool connect(vsx_string param_name, vsx_comp_abs* other_component, vsx_string other_param_name, int* ord = 0) = 0;
  virtual ~vsx_comp_abs() {};
};

#endif

