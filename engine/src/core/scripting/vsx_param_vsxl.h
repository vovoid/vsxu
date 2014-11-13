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

#ifndef VSX_PARAM_VSXL_H
#define VSX_PARAM_VSXL_H


class vsx_engine;

//////////////////////////////////////////////////////////////////////

class vsx_param_vsxl_abs
{
  public:
    virtual void* load(vsx_module_param_abs* engine_param, vsx_string<>program, int id = -1) = 0;
    virtual void execute() = 0;
    virtual ~vsx_param_vsxl_abs() {};
};


class vsx_param_vsxl_driver_abs
{

public:
  vsx_module_param_abs* my_param;
  int id;

  vsx_module_param_interpolation_list* interpolation_list;
  vsx_engine* engine;
  void* comp;
  int status;
  int result;
  void* state;
  vsx_string<>script;
  virtual void* load(vsx_module_param_abs* engine_param,vsx_string<>program) = 0;
  virtual void run() = 0;
  virtual void unload() = 0;
};





class vsx_param_vsxl : public vsx_param_vsxl_abs
{
  vsx_param_vsxl_driver_abs* my_driver;
public:
  vsx_engine* engine;
  void* load(vsx_module_param_abs* engine_param, vsx_string<>program, int id = -1);
  vsx_param_vsxl_driver_abs* get_driver();
  void execute();
  vsx_param_vsxl();
  void unload();
};


#endif
