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

#include <filesystem/vsx_filesystem.h>
#include "vsx_param.h"
#include <module/vsx_module.h>
#include <time/vsx_timer.h>
#include <list>
#include <command/vsx_command.h>
#include <command/vsx_command_list.h>
#include <string/vsx_string_helper.h>
#include <internal/vsx_comp_abs.h>
#include <internal/vsx_param_abstraction.h>
#include <internal/vsx_param_sequence.h>
#include <internal/vsx_master_sequence_channel.h>
#include <internal/vsx_param_sequence_list.h>
#include <internal/vsx_param_interpolation.h>
#include <internal/vsx_sequence_pool.h>

#include "vsx_engine.h"
#include <math/quaternion/vsx_quaternion.h>
#include <math/vsx_bezier_calc.h>

vsx_sequence_master_channel_item::vsx_sequence_master_channel_item()
{
  pool_sequence_list = 0x0;
  time_sequence.set_string("1.000000;1.000000;"+vsx_string_helper::base64_encode("0.0")+"|0.000000;1.000000;"+vsx_string_helper::base64_encode("1.0"));
}

void vsx_sequence_master_channel_item::run(float abs_time)
{
  //we're getting time in between 0.0f and 1.0f, REAL TIME
  if (!pool_sequence_list)
    return;

  // first get the time from the time_sequence
  // say abs = 4.0 and length = 5.0
  if (length > 0.0f && length  <  0.001f)
    return;

  float time_sequence_pos = abs_time / length;
  float time_pos = 0.0f;
  if (time_sequence_pos < 1.0f)
    time_pos = time_sequence.execute_absolute(time_sequence_pos).get_float();

  pool_sequence_list->run_absolute( pool_sequence_list->calculate_total_time() * time_pos);
}

void vsx_master_sequence_channel::run(float dtime)
{
  line_time += dtime;
  i_vtime += dtime;

  while (line_time < 0)
  {
    --line_cur;
    if (line_cur < 0) {
      line_cur = 0;
      line_time = 0;
    } else {
      line_time +=  items[line_cur]->total_length;
    }
  }

  if ((size_t)line_cur+1 < items.size())
  {
    if (line_time >= items[line_cur]->total_length)
    {
      line_time -= items[line_cur]->total_length;
      line_cur++;
    }
  }

  if (line_time <= items[line_cur]->length)
    items[line_cur]->run(line_time);
}



//*********************************************************************************************************
//*********************************************************************************************************
//*********************************************************************************************************



void vsx_master_sequence_channel::update_line(vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix)
{
  VSX_UNUSED(dest);
  VSX_UNUSED(cmd_prefix);

  // ____COMMAND STRUCTURE______________________
  // 0=mseq_channel
  // 1=row
  // 2=update
  // 3=[channel_name]
  // 4=[item_action_id]
  // 5=[local_time_distance if -1, don't set it]
  // 6=[length - if -1 don't set it]

  float p_vtime = i_vtime;

  vsx_sequence_master_channel_item* pa = (items[ vsx_string_helper::s2i(cmd_in->parts[4]) ]);

  // time distance (to next item)
  float local_time_distance;
  if ( (local_time_distance = vsx_string_helper::s2f(cmd_in->parts[5])) != -1.0f)
  {
    pa->total_length = local_time_distance;
  }

  // time length (internal length)
  float local_length;
  if ( (local_length = vsx_string_helper::s2f(cmd_in->parts[6]))  !=  -1.0f )
  {
    pa->length = local_length;
  }
  line_cur = 0;
  last_time = 0.0f;
  line_time = 0.0f;
  i_vtime = 0.0f;
  run(p_vtime);

}

// 0=mseq_channel 1=row 2=insert 3=[channel_name] 4=[item_action_id] 5=[local_time_distance] 6=[length] 7=[seq_pool_name]
void vsx_master_sequence_channel::insert_line(vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix)
{
  float p_vtime = i_vtime;
  long after_this_id = vsx_string_helper::s2i(cmd_in->parts[4]); // item_action_id
  float time_diff = vsx_string_helper::s2f(cmd_in->parts[5]);
  vsx_sequence_master_channel_item* pa;
  std::vector<vsx_sequence_master_channel_item*>::iterator it = items.begin();
  float item_length = 0.1f;
  if (after_this_id == (long)items.size()-1)
  {
    items[items.size()-1]->total_length = time_diff;
    pa = new vsx_sequence_master_channel_item;
    pa->pool_sequence_list = ((vsx_engine*)engine)->get_sequence_pool()->get_sequence_list_by_name(cmd_in->parts[7]);
    item_length = pa->pool_sequence_list->calculate_total_time(true);
    if (item_length == 0.0f) item_length = 0.1f;
    pa->total_length = item_length;
    pa->length = item_length;
    pa->time_sequence.set_string("1.000000;1.000000;"+vsx_string_helper::base64_encode("0.0")+"|0.000000;1.000000;"+vsx_string_helper::base64_encode("1.0"));
    items.push_back(pa);
  } else {
    int i;
    for (i = 0; i < after_this_id; ++i) {
      ++it;
    }

    pa = new vsx_sequence_master_channel_item();
    pa->pool_sequence_list = ((vsx_engine*)engine)->get_sequence_pool()->get_sequence_list_by_name(cmd_in->parts[7]);
    item_length = pa->pool_sequence_list->calculate_total_time(true);
    pa->total_length = (*it)->total_length - time_diff;
    if (pa->total_length > item_length)
    {
      pa->length = item_length;
    }
    else
    {
      pa->length = pa->total_length;
    }
    (*it)->total_length = time_diff;
    ++it;

    // TODO: maybe: cut off the time of the default sequence here rather than scaling it?
    items.insert(it, pa);
  }
  pa->pool_name = cmd_in->parts[7];

  line_cur = 0;
  last_time = 0.0f;
  line_time = 0.0f;
  i_vtime = 0.0f;
  run(p_vtime);


  // 0=mseq_channel_ok 1=row 2=insert 3=[channel_name] 4=[item_action_id] 5=[local_time_distance] 6=[length] 7=[name]
  dest->add_raw(
        cmd_prefix+
        "mseq_channel_ok "+
        "row "+
        "insert "+
        cmd_in->parts[3]+" "+
        cmd_in->parts[4]+" "+
        cmd_in->parts[5]+" "+
        vsx_string_helper::f2s(pa->length) +" "+ // [length]
        cmd_in->parts[7]//+" "+

  );
}

void vsx_master_sequence_channel::i_remove_line(int pos)
{
  float p_vtime = i_vtime;
  std::vector<vsx_sequence_master_channel_item*>::iterator it = items.begin();
  if (pos != 0)
  {
    for (size_t i = 0; i < (size_t)pos; ++i)
    {
      ++it;
    }

    if (pos < (long)items.size()-1)
    {
      items[pos-1]->total_length += items[pos]->total_length;
    }
    items.erase(it);
  }
  line_cur = 0;
  last_time = 0.0f;
  line_time = 0.0f;
  i_vtime = 0.0f;
  run(p_vtime);
}

// 0=mseq_channel 1=row 2=remove 3=[channel_name] 4=[item_action_id]
void vsx_master_sequence_channel::remove_line(vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix)
{
  long pos = vsx_string_helper::s2i(cmd_in->parts[4]);

  i_remove_line(pos);

  if (dest)
  dest->add_raw(cmd_prefix+"mseq_channel_ok row remove "+cmd_in->parts[3]+" "+cmd_in->parts[4]);
}

void vsx_master_sequence_channel::remove_all_lines_referring_to_sequence_list(vsx_param_sequence_list* list)
{
  int run = 1;
  size_t i = 0;
  while (run)
  {
    if (items[i]->pool_sequence_list == list)
    {
      i_remove_line((int)i);
      i = 0;
    } else
    {
      i++;
    }
    if (i == items.size()) run = 0;
  }
}

// 0=mseq_channel 1=row 2=time_sequence 3=[name] 4=[item_id] 5=[get]/[set] 6(optional)=[sequence_dump]
void vsx_master_sequence_channel::time_sequence(vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix)
{
  vsx_sequence_master_channel_item* pa = (items[ vsx_string_helper::s2i(cmd_in->parts[4]) ]);

  if (cmd_in->parts[5] == "set")
  {
    pa->time_sequence.set_string(cmd_in->parts[6]);
  } else
  if (cmd_in->parts[5] == "get")
  {
    dest->add_raw(
          cmd_prefix+
          "mseq_channel_ok "
          "row "
          "time_sequence "+
          cmd_in->parts[3]+" "+
          cmd_in->parts[4]+" "+
          cmd_in->parts[5]+" "+
          pa->time_sequence.get_string()
    );
  }
}




void vsx_master_sequence_channel::set_time(float new_time)
{
  VSX_UNUSED(new_time);
}

vsx_string<>vsx_master_sequence_channel::dump()
{
  vsx_string<>res = "";
  vsx_nw_vector< vsx_string<> > ml;
  size_t i = 0;
  for (std::vector<vsx_sequence_master_channel_item*>::iterator it = items.begin(); it != items.end(); ++it)
  {
    ml.push_back(
          vsx_string_helper::f2s((*it)->total_length)+";"+
          vsx_string_helper::f2s((*it)->length)+";"+
          vsx_string_helper::base64_encode((*it)->pool_name)+";"+
              vsx_string_helper::base64_encode((*it)->time_sequence.get_string())
    );
    i++;
  }
  vsx_string<>deli = "|";
  res = vsx_string_helper::implode(ml, deli);
  return res;
}


void vsx_master_sequence_channel::inject(vsx_string<>inject_string)
{
  // the reverse of the dump method above
  delete (vsx_sequence_master_channel_item*)(items[0]);
  items.clear();
  vsx_string<>deli = "|";
  vsx_nw_vector< vsx_string<> > parts;
  vsx_string_helper::explode(inject_string, deli, parts);
  for (size_t i = 0; i < parts.size(); i++)
  {
    vsx_string<>ideli = ";";
    vsx_nw_vector< vsx_string<> > iparts;
    vsx_string_helper::explode(parts[i], ideli, iparts);
    // 0 = total_length, 1 = internal_length, 2 = pool_name, 3 = time_sequence_data
    vsx_sequence_master_channel_item* pa = new vsx_sequence_master_channel_item;
    pa->total_length = vsx_string_helper::s2f(iparts[0]);
    pa->length = vsx_string_helper::s2f(iparts[1]);
    pa->pool_sequence_list = ((vsx_engine*)engine)->get_sequence_pool()->get_sequence_list_by_name(vsx_string_helper::base64_decode(iparts[2]));
    pa->pool_name = vsx_string_helper::base64_decode(iparts[2]);

    // TODO: remove the following line, it's obsolete
    if (iparts.size() > 3)
      pa->time_sequence.set_string(vsx_string_helper::base64_decode(iparts[3]));

    items.push_back(pa);
  }
}

vsx_master_sequence_channel::vsx_master_sequence_channel()
{
  i_vtime = 0.0f;
  line_cur = 0;
  line_time = 0.0f;
  vsx_sequence_master_channel_item* item = new vsx_sequence_master_channel_item;
  item->total_length = 1.0f;
  item->length = 0.0f;
  items.push_back(item);
}
