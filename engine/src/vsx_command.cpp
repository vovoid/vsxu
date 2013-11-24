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

#include "vsx_command.h"
#include <time.h>

int vsx_command_s::id = 0;

std::list<vsx_command_s*> vsx_command_s::garbage_list;
std::list<vsx_command_s*>::iterator vsx_command_s::it;
std::list<vsx_command_s*>::iterator vsx_command_s::it_2;

void vsx_command_s::process_garbage() {
  it = garbage_list.begin();
  if (garbage_list.size())
  while (it != garbage_list.end()) {
    if ((*it)->type == 0) {
      if ((*it)->iterations != -1)
      ++(*it)->iterations;
      if ((*it)->iterations > VSX_COMMAND_DELETE_ITERATIONS) {
      	//printf("d %d %d %s\n",(*it)->id,(*it)->iterations,(*it)->cmd.c_str());
      	//vsx_command_s* a = (*it);
        it_2 = it;
        ++it;
        garbage_list.erase(it_2);
//        delete (vsx_command_s*)a;
      } else
      ++it;
    } else {
      it_2 = it;
      ++it;
      garbage_list.erase(it_2);
    }
  }

//  for ( it != garbage_list.end(); ++it) {
//    ++(*it)->iterations;
//  }
//  for (std::list<vsx_command*>::iterator it = garbage_list.begin(); it != garbage_list.end(); ++it) {
//    ++(*it)->iterations;
//  }
//  if ((*it)->iterations > VSX_COMMAND_DELETE_ITERATIONS) {

//m  }

}


vsx_string vsx_command_s::get_parts(int start, int end) {
  vsx_string res = "";
  if ((unsigned long)start < parts.size()) {
    int end_;
    if (end == -1) {
      end_ = parts.size();
    } else {
      if ((unsigned long)end > parts.size()) end_ = parts.size();
      else
      end_ = end;
    }
    int i = start;
    bool first = true;
    while (i < end_) {
      if (!first) res.push_back(' ');
      res += parts[i];
      if (first) first = false;
      ++i;
    }
  }
  return res;
}

vsx_command_s::~vsx_command_s()
{
  if (iterations == -1)
  garbage_list.remove(this);
  #ifdef VSXU_DEBUG
    printf("vsx_command_s::destructor %s :::::::: %s\n",cmd.c_str(),raw.c_str());
  #endif
}


void vsx_command_s::parse() {
  if (parsed) return;
  if (raw == "") raw = cmd+" "+cmd_data;
  std::vector <vsx_string> cmdps;
  vsx_string deli = " ";
  split_string(raw, deli, cmdps);
  cmd = cmdps[0];
  if (cmdps.size() > 1)
  {
    cmd_data = cmdps[1];
  }
  parts = cmdps;
  parsed = true;
}


vsx_command_s* vsx_command_parse(vsx_string& cmd_raw) {
  std::vector <vsx_string> cmdps;
  vsx_command_s *t = new vsx_command_s;
  t->raw = cmd_raw;
  //printf("parsing raw: %s\n",cmd_raw.c_str());
  vsx_string deli = " ";
  split_string(cmd_raw, deli, cmdps);
  //printf("parsing2: %s\n",cmd_raw.c_str());
  t->cmd = cmdps[0];
  if (cmdps.size() > 1)
  {
    t->cmd_data = cmdps[1];
  }
  t->parts = cmdps;
  t->parsed = true;
  return t;
}


