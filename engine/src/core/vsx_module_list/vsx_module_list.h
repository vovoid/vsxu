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

#ifndef VSX_MODULE_LIST_H
#define VSX_MODULE_LIST_H

#include "vsx_dlopen.h"
// Implementation of Module List Class for Linux

// See vsx_module_list_abs.h for reference documentation for this class

class vsx_module_list : public vsx_module_list_abs
{
private:
  std::vector< vsx_dynamic_object_handle > plugin_handles;
public:
  void init();
  void destroy();
  std::vector< vsx_module_info* >* get_module_list( bool include_hidden = false);
  vsx_module* load_module_by_name(vsx_string name);
  void unload_module( vsx_module* module_pointer );
  bool find( const vsx_string &module_name_to_look_for);
  void print_help();
};

#endif
