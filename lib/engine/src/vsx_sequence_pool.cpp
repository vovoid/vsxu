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

#include "vsx_engine.h"
#include <internal/vsx_sequence_pool.h>
#include "vsx_data_path.h"

void vsx_sequence_pool::set_engine(void* new_engine)
{
  engine = new_engine;
}


void vsx_sequence_pool::remove_param_sequence(vsx_engine_param* param)
{
  for (std::map<vsx_string<>, vsx_param_sequence_list*>::iterator it = sequence_lists.begin(); it != sequence_lists.end(); it++)
  {
    (*it).second->remove_param_sequence(param);
  }
}

int vsx_sequence_pool::add(vsx_string<>name) // 1 = success, 0 = fail
{
  //printf("%d\n", __LINE__);
  if (sequence_lists.find(name) != sequence_lists.end()) return 0;
  vsx_param_sequence_list* new_sequence_list = new vsx_param_sequence_list(engine);
  new_sequence_list->set_run_on_edit(false);
  sequence_lists[name] = new_sequence_list;
  //printf("%d\n", __LINE__);
  return 1;
}

int vsx_sequence_pool::del(vsx_string<>name)
{
  if (sequence_lists.find(name) != sequence_lists.end())
  {
    //printf("%d\n", __LINE__);
    delete sequence_lists[name];
    sequence_lists.erase(name);
    cur_sequence_list = 0;
    return 1;
  }
  return 0;
}

int vsx_sequence_pool::clone(vsx_string<>name, vsx_string<>new_name)
{
  // insanity check
  if (sequence_lists.find(name) != sequence_lists.end() && sequence_lists.find(new_name) == sequence_lists.end())
  {
    vsx_param_sequence_list* np = new vsx_param_sequence_list(*(sequence_lists[name]));
    sequence_lists[new_name] = np;//sequence_lists[name]; // thank you friendly copy constructor
    return 1; // YEAH
  }
  return 0; // BOO
}

int vsx_sequence_pool::select(vsx_string<>name)
{
  //printf("%d\n", __LINE__);
  if (sequence_lists.find(name) != sequence_lists.end()) {
    //printf("setting cur_sequence_list\n");
    cur_sequence_list = sequence_lists[name];
    active = name;
    return 1;
  }
  return 0;
}

vsx_param_sequence_list* vsx_sequence_pool::get_selected()
{
  return cur_sequence_list;
}

bool vsx_sequence_pool::toggle_edit()
{
  edit_enabled = !edit_enabled;

  for (std::map<vsx_string<>, vsx_param_sequence_list*>::iterator it = sequence_lists.begin(); it != sequence_lists.end(); it++)
  {
    (*it).second->set_run_on_edit(edit_enabled);
  }

  return edit_enabled;
}

void vsx_sequence_pool::set_play_override(bool n)
{
  play_override_enabled = n;
}

bool vsx_sequence_pool::get_edit_enabled()
{
  return edit_enabled;
}

vsx_string<>vsx_sequence_pool::dump_names()
{
  vsx_string<>names;
  int i = 0;
  for (std::map<vsx_string<>, vsx_param_sequence_list*>::iterator it = sequence_lists.begin(); it != sequence_lists.end(); it++)
  {
    if (i != 0) names += ";";
    names += (*it).first;
    i++;
  }
  return names;
}

void vsx_sequence_pool::run(float dtime, bool run_from_channel)
{
  VSX_UNUSED(run_from_channel);
  // 1. run all sequences if we're enabled
  // 2. compare all engine's hints and send the changed values up to the GUI which will then handle eventual movements
  //printf("edit enabled: %d\n",(int)edit_enabled);
  if (!edit_enabled && !play_override_enabled)
    return;

  // if no pool selected
  if (!cur_sequence_list)
    return;

  if (current_state == 1)
  {
    vtime += dtime;
    if (loop_point > 0.0f)
      vtime = fmod(vtime, loop_point);
  }
  cur_sequence_list->run_absolute(vtime);
}


int vsx_sequence_pool::get_state()
{
  return current_state;
}

float vsx_sequence_pool::get_vtime()
{
  return vtime;
}

void vsx_sequence_pool::set_time(float time)
{
  vtime = time;
  if (edit_enabled)
  {
    if (cur_sequence_list)
    {
      cur_sequence_list->run_absolute(vtime);
    }
  }
}


void vsx_sequence_pool::play()
{
  current_state = 1;
}

void vsx_sequence_pool::stop()
{
  current_state = 0;
}

void vsx_sequence_pool::rewind()
{
  current_state = 0;
  vtime = 0.0f;
}

void vsx_sequence_pool::set_loop_point(float new_loop_point)
{
  loop_point = new_loop_point;
}


vsx_param_sequence_list* vsx_sequence_pool::get_sequence_list_by_name(vsx_string<>name)
{
  if (sequence_lists.find(name) != sequence_lists.end())
  {
    return (*(sequence_lists.find(name))).second;
  }
  return 0;
}

bool vsx_sequence_pool::export_to_file(vsx_string<>filename)
{
  vsx_param_sequence_list* sequence_list = cur_sequence_list;
  if (!sequence_list) return false;

  vsx_command_list savelist(true);

  vsx_string<> sequence_dump = sequence_list->get_sequence_list_dump();
  vsx_string<> deli = "&";
  vsx_nw_vector <vsx_string<> > parts;
  vsx_string_helper::explode(sequence_dump, deli, parts);
#if VSXU_DEBUG
  printf("sequence dump: %s\n", sequence_dump.c_str() );
#endif
  if (sequence_dump != "")
  {
    for (size_t i = 0; i < parts.size(); i++)
    {
      vsx_string<>i_deli = "#";
      vsx_nw_vector <vsx_string<> > i_parts;
      vsx_string_helper::explode(parts[i], i_deli, i_parts);
      // 0=pseq_inject 1=[component] 2=[param] 3=[data]
      savelist.add_raw("pseq_inject "+i_parts[0]+" "+i_parts[1]+" "+i_parts[2]);
    }
  }
  vsx_string<> list = savelist.get_as_string();
  vsx_string_helper::write_to_file(
      vsx_data_path::get_instance()->data_path_get()+"animations/"+filename,
      list
    );
  return true;
}

bool vsx_sequence_pool::import_from_file(vsx_string<>filename)
{
  vsx_command_list import_list(true);
  import_list.load_from_file( vsx_data_path::get_instance()->data_path_get() + "animations/"+filename);
  while (vsx_command_s* c = import_list.pop())
  {
    c->parse();
    if (c->parts[0] == "pseq_inject")
    {
      vsx_comp* component = ((vsx_engine*)engine)->get_component_by_name(c->parts[1]);
      if (component) {
        vsx_engine_param* param = component->get_params_in()->get_by_name(c->parts[2]);
        if (param)
        {
          cur_sequence_list
            ->
              inject_param(param, component, c->parts[3]);
        }
      }
    }
  }
  return true;
}

bool vsx_sequence_pool::export_values_to_file(vsx_string<>filename)
{
  vsx_param_sequence_list* sequence_list = cur_sequence_list;
  if (!sequence_list) return false;

  vsx_string<> result;
  vsx_string<> sequence_dump = sequence_list->get_sequence_list_dump();
  vsx_string<> deli = "&";
  vsx_nw_vector <vsx_string<> > parts;
  vsx_string_helper::explode(sequence_dump, deli, parts);

  if (sequence_dump != "")
  {
    for (size_t i = 0; i < parts.size(); i++)
    {
      vsx_string<>i_deli = "#";
      vsx_nw_vector <vsx_string<> > i_parts;
      vsx_string_helper::explode(parts[i], i_deli, i_parts);
      // 0=pseq_inject 1=[component] 2=[param] 3=[data]
      result += i_parts[2] + "\n";
    }
  }
  vsx_string_helper::write_to_file(
      vsx_data_path::get_instance()->data_path_get()+"animations/"+filename,
      result
    );
  return true;
}


void vsx_sequence_pool::dump_to_command_list(vsx_command_list &savelist)
{
  int reinit_edit = 0;
  if (edit_enabled) {
    toggle_edit();
    reinit_edit = 1;
  }
  for (std::map<vsx_string<>, vsx_param_sequence_list*>::iterator it = sequence_lists.begin(); it != sequence_lists.end(); it++)
  {
    savelist.add_raw("seq_pool add "+(*it).first);
    vsx_string<>sequence_dump = (*it).second->get_sequence_list_dump();
    vsx_string<>deli = "&";
    vsx_nw_vector <vsx_string<> > parts;
    vsx_string_helper::explode(sequence_dump, deli, parts);
#if VSXU_DEBUG
    printf("sequence dump: %s\n", sequence_dump.c_str() );
#endif
    if (sequence_dump != "")
    {
      foreach(parts, i)
      {
        vsx_string<>i_deli = "#";
        vsx_nw_vector <vsx_string<> > i_parts;
        vsx_string_helper::explode(parts[i], i_deli, i_parts);
        // 0=seq_pool 1=pseq_inject 2=[seq_pool_name] 3=[component] 4=[param] 5=[data]
        savelist.add_raw("seq_pool pseq_inject "+(*it).first+" "+i_parts[0]+" "+i_parts[1]+" "+i_parts[2]);
      }
    }

    vsx_string<> groups = (*it).second->group_dump_all();
    if (groups.size())
      savelist.add_raw("seq_pool group_inject "+(*it).first+" "+(*it).second->group_dump_all());
  }
  if (reinit_edit) {
    toggle_edit();
  }
}


void vsx_sequence_pool::clear()
{
  cur_sequence_list = 0x0;
  for (std::map<vsx_string<>, vsx_param_sequence_list*>::iterator it = sequence_lists.begin(); it != sequence_lists.end(); it++)
  {
    delete (*it).second;
  }
  sequence_lists.clear();
}



vsx_sequence_pool::vsx_sequence_pool()
{
  cur_sequence_list = 0;
  edit_enabled = false;
  play_override_enabled = false;
  loop_point = -1.0f;
  current_state = 0;
  vtime = 0.0f;
}

