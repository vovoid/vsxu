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

#ifndef VSX_COMP_VSXL_H
#define VSX_COMP_VSXL_H


class vsx_comp_vsxl_driver_abs {
public:
  vsx_module_param_list* my_param_list;
  void* comp;
  vsx_string<>script;
  virtual void* load(vsx_module_param_list* module_list,vsx_string<>program) = 0;
  virtual void run() = 0;
  virtual void unload() = 0;
  virtual ~vsx_comp_vsxl_driver_abs() {};
};

class vsx_comp_vsxl {
  vsx_comp_vsxl_driver_abs* my_driver;
public:
  void* load(vsx_module_param_list* module_list, vsx_string<>program);
  vsx_comp_vsxl_driver_abs* get_driver();
  void execute();
  vsx_comp_vsxl();
  void unload();
  ~vsx_comp_vsxl();
};




#endif
