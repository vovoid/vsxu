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

#include <vsx_platform.h>
#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <vsx_string.h>
#include <vsx_command.h>
#include "vsx_command_list.h"

#define VSX_COMMAND_CLIENT_NEVER_CONNECTED 0
#define VSX_COMMAND_CLIENT_CONNECTED 1
#define VSX_COMMAND_CLIENT_DISCONNECTED 2

class vsx_command_list_server
{
  pthread_t         worker_t;
  pthread_attr_t    worker_t_attr;
  vsx_command_list* cmd_in;
  vsx_command_list* cmd_out;

  // internal worker method
  static void* server_worker(void *ptr);

public:
  vsx_command_list_server();
  // set the command lists on which the server class operates
  void set_command_lists(vsx_command_list* new_in, vsx_command_list* new_out);
  // start the server (after setting command_lists)
  bool start();
};


class vsx_command_list_client
{
  pthread_t         worker_t;
  pthread_attr_t    worker_t_attr;
  vsx_command_list cmd_in;
  vsx_command_list cmd_out;

  // internal server address
  vsx_string server_address;
  
  // internal worker method
  static void* client_worker(void *ptr);
  int connected;
public:
  vsx_command_list_client();
  // set the command lists on which the server class operates
  vsx_command_list* get_command_list_in();
  vsx_command_list* get_command_list_out();

  // start the server (after setting command_lists)
  bool client_connect(vsx_string &server_a);

  // get connection status
  int get_connection_status();
};

#endif
