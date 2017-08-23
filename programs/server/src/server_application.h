/**
* Project: VSXu: Realtime modular visual programming language, music/audio visualizer.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Vovoid Media Technologies AB Copyright (C) 2003-2013
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

#pragma once

#include <vsx_version.h>
#include <vsx_application.h>
#include <vsx_application_control.h>
#include <vsx_application_input_state_manager.h>

#include <vsx_engine.h>
#include <command/vsx_command_client_server.h>

class server_application
    : public vsx_application
{
  vsx_engine* vxe = 0x0;
  vsx_module_list_abs* module_list = 0x0;
  vsx_command_list_server cl_server;

  vsx_command_list internal_cmd_in;
  vsx_command_list internal_cmd_out;

public:

  server_application()
    :
      internal_cmd_in(false),
      internal_cmd_out(false)
  {}


  vsx_string<> window_title_get()
  {
    char titlestr[ 200 ];
    sprintf( titlestr, "Vovoid VSXu Server %s [%s %d-bit] %s", VSXU_VER, PLATFORM_NAME, PLATFORM_BITS, VSXU_VERSION_COPYRIGHT);
    return vsx_string<>(titlestr);
  }

  void init_graphics()
  {
    module_list = vsx_module_list_factory_create();
    vxe = new vsx_engine(module_list);
    vxe->start();
    cl_server.set_command_lists(&internal_cmd_in,&internal_cmd_out);
    cl_server.start();
  }

  void uninit_graphics()
  {
    vxe->stop();
    delete vxe;
    vsx_module_list_factory_destroy(module_list);
  }

  void draw()
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    vxe->process_message_queue(&internal_cmd_in,&internal_cmd_out);
    vxe->render();
  }

  void event_key_down(long key)
  {
    if (key == VSX_SCANCODE_ESCAPE)
      vsx_application_control::get_instance()->shutdown_request();
  }

};

