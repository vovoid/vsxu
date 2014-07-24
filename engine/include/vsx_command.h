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

#ifndef VSX_COMMAND_H
#define VSX_COMMAND_H

#include <vsx_platform.h>
#include <map>
#include <list>
#include <vector>
#include "vsxfst.h"
#include <vsx_string_helper.h>
#include "vsx_engine_dllimport.h"


// different types of commands
#define VSX_COMMAND_PARAM_SET 1
#define VSX_COMMAND_MENU 2
#define VSX_COMMAND 3

#define VSX_COMMAND_MAX_ITERATIONS 100
#define VSX_COMMAND_DELETE_ITERATIONS 120

// command specification (container class)
// this is used within the whole system. that is both the client, who has both a vsx_engine and an instance of the gui
// widget system, and in the server.
// it works somewhat the way windows messages works, like "do this"

// Each instance of vsxu program running has a global command processing queue.
// The vsx_command_list is there for this, it acts as a FIFO buffer for commands as well as reading and managing
// possible commands that appear in menus etc.

// cut'n'paste examples:
// commands.adds(VSX_COMMAND_MENU, "Connect to server", "desktop.server.connect","");
//            ^-- adds sys item ^-- type menu

//**********************************************************************************************************************

ENGINE_DLLIMPORT class vsx_command_s
{
  ENGINE_DLLIMPORT static std::list<vsx_command_s*> garbage_list; // the parts of the command
  ENGINE_DLLIMPORT static std::list<vsx_command_s*>::iterator it;
  ENGINE_DLLIMPORT static std::list<vsx_command_s*>::iterator it_2;
public:
  ENGINE_DLLIMPORT void process_garbage();
  std::list<vsx_command_s*>* garbage_pointer;
  ENGINE_DLLIMPORT static int id;
  bool parsed;
  int owner; // for color-coding this command
  int type; // type of command
  int iterations;
  vsx_string title; // Title - for internal GUI stuff like menus and stuff
  vsx_string cmd; // the first part of the command, the actual command
  vsx_string cmd_data; // the second parameter (for simple commands)
  vsx_avector<char> cmd_data_bin; // the binary part of the command
  vsx_string raw; // the unparsed command, empty when binary command
  std::vector <vsx_string> parts; // the parts of the command


  void copy (vsx_command_s *t) {
    owner = t->owner;
    type = t->type;
    title = t->title;
    cmd = t->cmd;
    cmd_data = t->cmd_data;
    raw = t->raw;
    parts = t->parts;
    iterations = t->iterations;
  }
  vsx_string str() {
    if (raw.size())
    return raw; else
    return cmd + " " + cmd_data;
  }
  ENGINE_DLLIMPORT void parse();
  vsx_command_s() {
    parsed = false;
    type = 0;
    iterations = 0;
    ++id;
    //#ifndef VSX_CMD_GARBAGE_DISABLED
    garbage_list.push_back(this);
    garbage_pointer = &garbage_list;
    //#endif
  }

  vsx_command_s(bool garbage_collectable) {
    parsed = false;
    type = 0;
    iterations = 0;
    ++id;
    if (garbage_collectable)
    {
      //#ifndef VSX_CMD_GARBAGE_DISABLED
      garbage_list.push_back(this);
      garbage_pointer = &garbage_list;
      //#endif
    }
  }

  // returns a string like "part1 part2 part3" if start was 1 and end was 3
  ENGINE_DLLIMPORT vsx_string get_parts(int start = 0, int end = -1);

  void dump_to_stdout()
  {
  	for (size_t i = 0; i < parts.size(); i++)
  	printf("%s ",parts[i].c_str());
  	printf("\n");
  }

  ENGINE_DLLIMPORT ~vsx_command_s();
};

ENGINE_DLLIMPORT vsx_command_s* vsx_command_parse(vsx_string& cmd_raw);



#endif
