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

void vsx_param_sequence_list::get_init(vsx_engine_param* param, vsx_command_list* dest,vsx_string<>comp_name, vsx_string<>prefix)
{
  if (parameter_channel_map.find(param) != parameter_channel_map.end())
  {
    vsx_param_sequence* p = (*parameter_channel_map.find(param)).second;
    dest->add_raw(prefix+"pseq_p_ok init "+comp_name+" "+param->name+" "+vsx_string_helper::f2s((float)p->items.size()));
  }
}

void vsx_param_sequence_list::get_contents(vsx_engine_param* param, vsx_command_list* dest,vsx_string<>controller_id) {
  VSX_UNUSED(param);
  VSX_UNUSED(dest);
  VSX_UNUSED(controller_id);
}

vsx_param_sequence_list::vsx_param_sequence_list(void* my_engine) {
  engine = my_engine;
  other_time_source = 0;
  run_on_edit_enabled = true;
  total_time = 0.0f;
  int_vtime = 0.0f;
}

vsx_param_sequence_list::~vsx_param_sequence_list()
{
  for (auto it : parameter_channel_map)
    delete it.second;

  for (auto it : groups)
    delete it.second;
}


// not optimal linkage but OK whatever
vsx_param_sequence_list::vsx_param_sequence_list(const vsx_param_sequence_list &b)
{
  for (
        std::map<vsx_engine_param*,vsx_param_sequence*>::const_iterator it = b.parameter_channel_map.begin();
        it != b.parameter_channel_map.end();
        it++)
  {
    // create a new instance here and copy it from the other list
    vsx_param_sequence* ps = new vsx_param_sequence;
    *ps = *((*it).second);
    parameter_channel_map[(*it).first] = ps;
    parameter_channel_list.push_back(ps);
  }
  // WARNING TO SELF! ONRY INTENDED FOR POOL USAGE, NO MASTER CHANNELS COPIED
  other_time_source = 0;
  total_time = 0.0f;
  int_vtime = 0.0f;
}


void vsx_param_sequence_list::add_param_sequence(vsx_engine_param* param, vsx_comp_abs* comp)
{
  // if found, return
  if (parameter_channel_map.find(param) != parameter_channel_map.end())
    return;

  // add sequence
  vsx_param_sequence* p = new vsx_param_sequence(param->module_param->type, param);
  p->engine = engine;
  p->comp = comp;
  p->param = param;
  param->sequence = true;
  if (engine && run_on_edit_enabled) {
    p->execute(((vsx_engine*)engine)->get_engine_info()->vtime);
  }

  parameter_channel_list.push_back(p);
  parameter_channel_map[param] = p;

}

void vsx_param_sequence_list::remove_param_sequence(vsx_engine_param* param)
{
  // if not found, return
  if (parameter_channel_map.find(param) == parameter_channel_map.end())
    return;

  // delete the sequence
  vsx_param_sequence* p = (*(parameter_channel_map.find(param))).second;
  delete p;
  param->sequence = false;
  parameter_channel_list.remove(p);
  parameter_channel_map.erase(param);
}

vsx_string<>vsx_param_sequence_list::get_sequence_list_dump()
{
  if (parameter_channel_list.size() == 0) return "";
  vsx_nw_vector< vsx_string<> > seq;
  for (std::list<vsx_param_sequence*>::iterator it = parameter_channel_list.begin(); it != parameter_channel_list.end(); ++it)
  {
    seq.push_back((*it)->comp->name+"#"+(*it)->param->name+"#"+(*it)->dump());
  }
  vsx_string<>deli = "&";
  return vsx_string_helper::implode(seq,deli);
}

vsx_string<>vsx_param_sequence_list::get_channel_names()
{
  vsx_nw_vector< vsx_string<> > seq;
  for (std::list<vsx_param_sequence*>::iterator it = parameter_channel_list.begin(); it != parameter_channel_list.end(); ++it)
  {
    seq.push_back((*it)->comp->name+":"+(*it)->param->name);
  }
  for (std::map<vsx_string<>,void*>::iterator it = master_channel_map.begin(); it != master_channel_map.end(); ++it)
  {
    seq.push_back( (*it).first+":[master]");
  }
  vsx_string<>deli = "&";
  return vsx_string_helper::implode(seq,deli);
}


void vsx_param_sequence_list::rescale_time(float start, float scale)
{
  for (std::list<vsx_param_sequence*>::iterator it = parameter_channel_list.begin(); it != parameter_channel_list.end(); ++it)
  {
    (*it)->rescale_time(start, scale);
  }
}

float vsx_param_sequence_list::calculate_total_time(bool no_cache)
{
  if (no_cache)
  {
    total_time = 0.0f;
  }
  else
  {
    if (total_time != 0.0f)
    return total_time;
  }

  total_time = 0.0f;
  for (std::list<vsx_param_sequence*>::iterator it = parameter_channel_list.begin(); it != parameter_channel_list.end(); ++it) {
    if ( (*it)->calculate_total_time(no_cache) > total_time) total_time = (*it)->calculate_total_time();
  }
  return total_time;
}

void vsx_param_sequence_list::get_sequences(vsx_command_list* dest) {
  for (std::list<vsx_param_sequence*>::iterator it = parameter_channel_list.begin(); it != parameter_channel_list.end(); ++it) {
    dest->add_raw("pseq_p_ok init "+(*it)->comp->name+" "+(*it)->param->name);
  }
  for (std::map<vsx_string<>,void*>::iterator it = master_channel_map.begin(); it != master_channel_map.end(); ++it) {
    dest->add_raw("pseq_p_ok init " + (*it).first+" [master]");
  }
}

void vsx_param_sequence_list::update_line(vsx_engine_param* param, vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix) {
  if (parameter_channel_map.find(param) != parameter_channel_map.end()) {

    vsx_param_sequence* p = parameter_channel_map[param];
    p->update_line(dest,cmd_in,cmd_prefix);
    if (engine && run_on_edit_enabled) {
      p->execute(int_vtime);
    }
  }
}

void vsx_param_sequence_list::insert_line(vsx_engine_param* param, vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix)
{
  if (parameter_channel_map.find(param) != parameter_channel_map.end())
  {
    vsx_param_sequence* p = parameter_channel_map[param];
    p->insert_line(dest,cmd_in,cmd_prefix);
    if (engine && run_on_edit_enabled)
    {
      p->execute(int_vtime);
    }
  }
}

void vsx_param_sequence_list::insert_line_absolute(vsx_engine_param* param, vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix)
{
  if (parameter_channel_map.find(param) != parameter_channel_map.end())
  {
    vsx_param_sequence* p = parameter_channel_map[param];
    p->insert_line_absolute(dest,cmd_in,cmd_prefix);
    if (engine && run_on_edit_enabled)
    {
      p->execute(int_vtime);
    }
  }
}


void vsx_param_sequence_list::remove_line(vsx_engine_param* param, vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix)
{
  if (parameter_channel_map.find(param) != parameter_channel_map.end())
  {
    vsx_param_sequence* p = parameter_channel_map[param];
    p->remove_line(dest,cmd_in,cmd_prefix);
    if (engine && run_on_edit_enabled)
    {
      p->execute(int_vtime);
    }
  }
}


void vsx_param_sequence_list::run(float dtime, float blend)
{
  int_vtime += dtime;

  // run normal param sequences
  for (std::list<vsx_param_sequence*>::iterator it = parameter_channel_list.begin(); it != parameter_channel_list.end(); ++it)
  {
    (*it)->execute(dtime, blend);
  }

  // run master channels
  for (std::list<void*>::iterator it = master_channel_list.begin(); it != master_channel_list.end(); it++)
  {
    ((vsx_master_sequence_channel*)(*it))->run(dtime);
  }
}

void vsx_param_sequence_list::run_absolute(float vtime, float blend)
{
  float dtime = vtime - int_vtime;

  int_vtime += dtime;
  for (std::list<vsx_param_sequence*>::iterator it = parameter_channel_list.begin(); it != parameter_channel_list.end(); ++it)
    (*it)->execute(dtime, blend);

  for (std::list<void*>::iterator it = master_channel_list.begin(); it != master_channel_list.end(); it++)
    ((vsx_master_sequence_channel*)(*it))->run(dtime);
}

vsx_string<>vsx_param_sequence_list::dump_param(vsx_engine_param* param) {
  if (parameter_channel_map.find(param) != parameter_channel_map.end())
    return parameter_channel_map[param]->dump();

  return "";
}

vsx_string<>vsx_param_sequence_list::dump_param_values(vsx_engine_param* param) {
  if (parameter_channel_map.find(param) != parameter_channel_map.end())
    return parameter_channel_map[param]->dump_values();

  return "";
}


void vsx_param_sequence_list::inject_param(vsx_engine_param* param, vsx_comp_abs* comp, vsx_string<>data) {
  if (parameter_channel_map.find(param) == parameter_channel_map.end()) {
    // add sequence
    vsx_param_sequence* p = new vsx_param_sequence(param->module_param->type, param);
    p->engine = engine;
    p->comp = comp;
    p->param = param;
    p->inject(data);
    param->sequence = true;
    if (engine)
    {
      // reset time - needed when paused somewhere and re-declaring parameters
      p->execute(((vsx_engine*)engine)->get_engine_info()->vtime);
    }
    parameter_channel_list.push_back(p);
    parameter_channel_map[param] = p;
  }
}

void vsx_param_sequence_list::get_params_with_keyframe_at_time(float time, float tolerance, vsx_nw_vector<vsx_engine_param* >& result)
{
 for (std::list<vsx_param_sequence*>::iterator it = parameter_channel_list.begin(); it != parameter_channel_list.end(); it++)
   if ((*it)->has_keyframe_at_time(time, tolerance))
     result.push_back( (*it)->param );
}


// ******************************** MASTER CHANNEL ********************************
// ******************************** MASTER CHANNEL ********************************
// ******************************** MASTER CHANNEL ********************************
// ******************************** MASTER CHANNEL ********************************

void vsx_param_sequence_list::clear_master_sequences()
{
  for (std::list<void*>::iterator it = master_channel_list.begin(); it != master_channel_list.end(); it++)
  {
    delete (vsx_master_sequence_channel*)(*it);
  }
  master_channel_map.clear();
  master_channel_list.clear();
  total_time = int_vtime = 0.0f;
}

int vsx_param_sequence_list::add_master_channel(vsx_string<>name)
{
  // first check if it already exists a channel with this name
  if (master_channel_map.find(name) != master_channel_map.end()) return 0;
  vsx_master_sequence_channel* mseq = new vsx_master_sequence_channel;
  mseq->set_engine(engine);
  master_channel_map[name] = mseq;
  master_channel_list.push_back(mseq);
  return 1;
}

int vsx_param_sequence_list::remove_master_channel(vsx_string<>name)
{
  if (master_channel_map.find(name) != master_channel_map.end())
  {
    master_channel_list.remove(master_channel_map[name]);
    delete ((vsx_master_sequence_channel*)master_channel_map[name]);
    master_channel_map.erase(name);
    return 1;
  }
  return 0;
}

vsx_string<>vsx_param_sequence_list::dump_master_channel(vsx_string<>channel_name)
{
  if (master_channel_map.find(channel_name) != master_channel_map.end())
  {
    // TODO: dump
    return ((vsx_master_sequence_channel*)master_channel_map[channel_name])->dump();
  }
  return "";
}

void vsx_param_sequence_list::inject_master_channel(vsx_string<>name, vsx_string<>data)
{
  add_master_channel(name);
  ((vsx_master_sequence_channel*)master_channel_map[name])->inject(data);
}


void vsx_param_sequence_list::group_add_param(vsx_string<> group_name, vsx_string<> comp_param)
{
  if (groups.find(group_name) == groups.end())
    groups[group_name] = new vsx_param_sequence_group();

  groups[group_name]->add(comp_param);
}

void vsx_param_sequence_list::group_del_param(vsx_string<> group_name, vsx_string<> comp_param)
{
  if (groups.find(group_name) == groups.end())
    return;

  groups[group_name]->remove(comp_param);
}

void vsx_param_sequence_list::group_del(vsx_string<> group_name)
{
  if (groups.find(group_name) == groups.end())
    return;

  delete groups[group_name];
  groups.erase(group_name);
}

vsx_string<> vsx_param_sequence_list::group_dump(vsx_string<> group_name)
{
  if (groups.find(group_name) == groups.end())
    return "";

  return groups[group_name]->dump();
}

vsx_string<> vsx_param_sequence_list::group_dump_all()
{
  vsx_string<> result;
  for (auto it = groups.begin(); it != groups.end(); it++)
  {
    if (result.size())
      result += "|";
    result += (*it).first+"+"+(*it).second->dump();
  }
  return result;
}

void vsx_param_sequence_list::group_inject(vsx_string<>& data)
{
  vsx_nw_vector< vsx_string<> > group_list;
  vsx_string_helper::explode_single(data, '|', group_list);

  for (auto it : groups)
    delete it.second;

  groups.clear();
  foreach (group_list, i)
  {
    vsx_nw_vector< vsx_string<> > group_components;
    vsx_string_helper::explode_single(group_list[i], '+', group_components);
    vsx_string<> group_name = group_components[0];
    groups[group_name] = new vsx_param_sequence_group();
    groups[group_name]->load(group_components[1]);
  }
}


void vsx_param_sequence_list::dump_master_channels_to_command_list(vsx_command_list &savelist)
{
  //TODO: !!!
  for (std::map<vsx_string<>,void*>::iterator it = master_channel_map.begin(); it != master_channel_map.end(); it++)
  {
    //0=mseq_channel 1=inject 2=[channel_name] 3=[data like time;length;pool_name]
    savelist.add_raw(
          vsx_string<>("mseq_channel ")+  // 0
          "inject "+ // 1
          (*it).first+" "+ // 2
          ((vsx_master_sequence_channel*)((*it).second))->dump()
    );
  }
}

void vsx_param_sequence_list::update_master_channel_line(vsx_string<>channel_name, vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix)
{
  if (master_channel_map.find(channel_name) != master_channel_map.end())
  {
    ((vsx_master_sequence_channel*)master_channel_map[channel_name])->update_line(dest,cmd_in,cmd_prefix);
  }
}

void vsx_param_sequence_list::insert_master_channel_line(vsx_string<>channel_name, vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix)
{
  if (master_channel_map.find(channel_name) != master_channel_map.end())
  {
    ((vsx_master_sequence_channel*)master_channel_map[channel_name])->insert_line(dest,cmd_in,cmd_prefix);
  }
}

void vsx_param_sequence_list::remove_master_channel_line(vsx_string<>channel_name, vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix)
{
  if (master_channel_map.find(channel_name) != master_channel_map.end())
  {
    ((vsx_master_sequence_channel*)master_channel_map[channel_name])->remove_line(dest,cmd_in,cmd_prefix);
  }
}

void vsx_param_sequence_list::remove_master_channel_lines_referring_to_sequence_list(vsx_param_sequence_list* list)
{
  for (std::list<void*>::iterator it = master_channel_list.begin(); it != master_channel_list.end(); it++)
  {
    ((vsx_master_sequence_channel*)(*it))->remove_all_lines_referring_to_sequence_list(list);
  }
}


void vsx_param_sequence_list::time_sequence_master_channel_line(vsx_string<>channel_name, vsx_command_list* dest, vsx_command_s* cmd_in, vsx_string<>cmd_prefix)
{
  if (master_channel_map.find(channel_name) != master_channel_map.end())
  {
    ((vsx_master_sequence_channel*)master_channel_map[channel_name])->time_sequence(dest,cmd_in,cmd_prefix);
  }
}
