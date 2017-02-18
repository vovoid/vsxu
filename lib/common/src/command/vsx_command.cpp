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

#include <command/vsx_command.h>
#include <time.h>

int vsx_command_s::id = 0;

std::vector<vsx_command_s*> vsx_command_garbage_list;

void vsx_command_process_garbage()
{
  if (!vsx_command_garbage_list.size())
    return;

  std::vector<vsx_command_s*>::iterator it;
  std::vector<vsx_command_s*> save;

  it = vsx_command_garbage_list.begin();
  while (it != vsx_command_garbage_list.end())
  {
    vsx_command_s* a = (*it);

    (*it)->iterations++;

    if ((*it)->iterations > VSX_COMMAND_DELETE_ITERATIONS)
    {
      delete a;
    } else
    {
      save.push_back(*it);
    }
    it++;
  }
  vsx_command_garbage_list = save;
}

void vsx_command_process_garbage_exit()
{
  std::vector<vsx_command_s*>::iterator it;
  it = vsx_command_garbage_list.begin();
  while (it != vsx_command_garbage_list.end())
  {
    vsx_command_s* a = (*it);
    delete a;
    it++;
  }
  vsx_command_garbage_list.clear();
}

vsx_string<>vsx_command_s::get_parts(int start, int end) {
  vsx_string<>res = "";
  if ((unsigned long)start < parts.size()) {
    
    int end_;
    if (end == -1) {
      end_ = (int)parts.size();
    } else {
      if ((unsigned long)end > parts.size())
		    end_ = (int)parts.size();
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



void vsx_command_s::parse() {
  if (parsed) return;
  if (raw == "") raw = cmd+" "+cmd_data;
  vsx_nw_vector< vsx_string<> > cmdps;
  vsx_string<>deli = " ";
  vsx_string_helper::explode(raw, deli, cmdps);
  cmd = cmdps[0];
  if (cmdps.size() > 1)
  {
    cmd_data = cmdps[1];
  }
  parts = cmdps;
  parsed = true;
}



