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


#ifndef VSX_MODULE_LIST_ABS_H
#define VSX_MODULE_LIST_ABS_H

// Module List Abstract Class

class vsx_module_list_abs
{
private:
protected:
  // holder of dynamically linked object info, by string
  // the pointer should be typecast to vsx_module_plugin_info*
  std::map< vsx_string, void* > module_plugin_list;

  // module list (master)
  // holds module_infos harvested from the modules
  std::vector< vsx_module_info* > module_infos;

  // module list by string - holds all modules, including hidden modules
  std::map< vsx_string, vsx_module_info* > module_list;

  // module list intended for GUI consumption, doesn't contain hidden modules
  std::vector< vsx_module_info* > module_list_public;

public:
  // Init / Build Module List
  //   This method looks on disk for plugins, enumerates them
  //   and stores the results.
  virtual void init() = 0;

  virtual void destroy() = 0;

  // Return a list of the modules
  // Note that you must destroy the return value.
  virtual std::vector< vsx_module_info* >* get_module_list( bool include_hidden = false) = 0;

  // Load Module By Name
  //   Used to load a vsxu module
  //   Parameters:
  //     name:
  //       Module identifier (i.e. path;to;module)
  //   Example use:
  //     load_module_by_name("math;oscillators;oscillator");
  virtual vsx_module* load_module_by_name(vsx_string name) = 0;

  // Unload Module
  virtual void unload_module( vsx_module* module_pointer ) = 0;

  // Check presence of module
  virtual bool find( const vsx_string &module_name_to_look_for) = 0;

  // Print help text for all loaded modules
  virtual void print_help() = 0;
};

#endif
