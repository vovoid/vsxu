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


#ifndef VSX_MODULE_PLUGIN_INFO_H_
#define VSX_MODULE_PLUGIN_INFO_H_

#include <vsx_platform.h>

class vsx_module_plugin_info
{
public:

  vsx_module_plugin_info()
  :
    module_id(0),
    hidden_from_gui(false),
    module_info(0x0)
  {}

  ~vsx_module_plugin_info()
  {
    if (module_info)
      delete module_info;
  }

  vsx_dynamic_object_handle plugin_handle;
  int module_id;
  bool hidden_from_gui;
  vsx_module_specification* module_info;

  // cached function to module's constructor/destructor
  vsx_module*(*create_new_module)( unsigned long, void* );
  void(*destroy_module)( vsx_module*, unsigned long );
};

#endif /*VSX_MODULE_PLUGIN_INFO_H_*/
