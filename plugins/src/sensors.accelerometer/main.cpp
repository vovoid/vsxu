/**
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
*
* @author Dinesh Manajipet, Vovoid Media Technologies AB Copyright (C) 2003-2013
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

#ifndef _MSC_VER
#include <inttypes.h>
#else
#include <stdint.h>
#endif

#include <string>
#include <stdlib.h>

#include "vsx_param.h"
#include <module/vsx_module.h>
#include "vsx_math_3d.h"

#include "Net.h"

class module_sensors_local_accelerometer : public vsx_module {
  // out
  vsx_module_param_float* ax;
  vsx_module_param_float* ay;
  vsx_module_param_float* az;

public:
  module_sensors_local_accelerometer():
    ax(0),ay(0),az(0)
    {
    }
  ~module_sensors_local_accelerometer(){}

  void module_info(vsx_module_specification* info)
  {
    info->identifier = "sensors;local;accelerometer";
    info->description = "returns [ax,ay,az] values of the local accelerometer, if available.\n";
    info->out_param_spec = "ax:float,"
                          "ay:float,"
                          "az:float";
    info->component_class = "system";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    ax = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"ax");
    ay = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"ay");
    az = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"az");
    ax->set(0);
    ay->set(0);
    az->set(0);
  //--------------------------------------------------------------------------------------------------
  }

  void run() {
    // Usually accelerometers are represented by a device file or a dbus api. need to look for a cleaner api for this.
  }

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class module_sensors_remote_accelerometer : public vsx_module {
  // out
  vsx_module_param_float* ax;
  vsx_module_param_float* ay;
  vsx_module_param_float* az;
  // internal
  static int nextPort;
  const int port;
  Net::Socket socket;

public:
  module_sensors_remote_accelerometer():port(nextPort){
    nextPort++;
    Net::InitializeSockets();
  }

  ~module_sensors_remote_accelerometer(){
    Net::ShutdownSockets();
  }

  bool init()
  {
    if(!socket.Open(port)){
      printf("Failed to create a Socket on Port : %d\n", port);
      return false;
    }
    printf("*Listening for a remote accelerometer on Port : %d\n", port);
    return true;
  }

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "sensors;remote;accelerometer";

    info->description =
      "returns [ax,ay,az] values of a remote accelerometer connected to port 5555\n";

    info->out_param_spec =
      "ax:float,"
      "ay:float,"
      "az:float"
    ;

    info->component_class =
      "system";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    ax = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"ax");
    ay = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"ay");
    az = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"az");
    ax->set(0);
    ay->set(0);
    az->set(0);
  //--------------------------------------------------------------------------------------------------
  }

  void run() {
    Net::Address sender;
    char buffer[128];
    int bytes_read = socket.Receive (sender, buffer, sizeof (buffer));
    if (bytes_read <= 0)
      return;

    std::string data(buffer);
    int comma1 = data.find(",");
    int comma2 = data.rfind(",");

    ax->set(atof(data.substr(0,comma1).c_str()));
    ay->set(atof(data.substr(comma1,comma2).c_str()));
    az->set(atof(data.substr(comma2).c_str()));

  }
};

int module_sensors_remote_accelerometer::nextPort = 5555;
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//*** F A C T O R Y ************************************************************
//******************************************************************************

#ifndef _WIN32
#define __declspec(a)
#endif

extern "C" {
__declspec(dllexport) vsx_module* create_new_module(unsigned long module);
__declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
__declspec(dllexport) unsigned long get_num_modules(vsx_module_engine_environment* environment);
}

vsx_module* MOD_CM(unsigned long module) {
  switch (module) {
    case 0:  return (vsx_module*)(new module_sensors_local_accelerometer);
    case 1:  return (vsx_module*)(new module_sensors_remote_accelerometer);
  }
  return 0;
}

void MOD_DM(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: if(m)delete (module_sensors_local_accelerometer*)m; break;
    case 1: if(m)delete (module_sensors_remote_accelerometer*)m; break;
  }
}

unsigned long MOD_NM(vsx_module_engine_environment* environment) {
  return 2;
}


//module_vector_float4_to_4float
