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

#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
  #include <io.h>
#endif

#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
  #include <dlfcn.h>
  #include <syslog.h>
#endif

#include <sys/types.h>
#include <string/vsx_string.h>
#include <log/vsx_log.h>
#include "vsx_engine.h"
#include <internal/vsx_master_sequence_channel.h>
#include <internal/vsx_note.h>
#include <vsx_data_path.h>

#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#endif

#ifndef VSXE_NO_GM
#include "gm/gmThread.h"    // Game monkey script machine and thread
#include "gm/gmMachine.h"
#include "binds/gmCall.h"
#include "binds/gmMathLib.h"
#include "scripting/vsx_param_vsxl.h"
#include "scripting/vsx_comp_vsxl.h"
#include "scripting/vsxl_engine.h"
#endif

#include <vector>

void vsx_engine_abs::constructor_set_default_values()
{
  valid = false;
  no_send_client_time = false;
  g_timer_amp = 1.0f;
  engine_info.filesystem = &filesystem;
  vsxl = 0;
  lastsent = 0;
  sequence_pool.set_engine((void*)this);
  last_e_state = current_state = VSX_ENGINE_STOPPED;
  filesystem.set_base_path( vsx_data_path::get_instance()->data_path_get() );
  engine_time_from_sequence_pool = false;
  frame_cfp_time = 0.0f;
  last_m_time_synch = 0;
  first_start = true;
  disabled = true;
  // rendering hints
  render_hint_module_output_only = false;
  render_hint_module_run_only = false;
  render_hint_post_render_reset_component_status = true;
  frame_dcount = 0;
  frame_dtime = 0;
  frame_dprev = -1;
  frame_delta_fps = 0;
  frame_delta_fps_frame_count_interval = 50;
  component_name_autoinc = 0;
}

int vsx_engine_abs::i_load_state(vsx_command_list& load1,vsx_string<>*error_string, vsx_string<>info_filename)
{
  if (!valid) return 2;
  vsx_command_list loadr2(true);
  load1.reset();
  vsx_command_s* mc = 0;
  // check the macro list to verify the existence of the componente we need for this macro
  vsx_string<>failed_component = "";
  while ( (mc = load1.get()) )
  {
    if (mc->cmd == "component_create")
    {
      // verify that the module is present and can be loaded
      if
      (
          !module_list->find( mc->parts[1] )
      )
      {
        failed_component = mc->parts[2];
        //components_existing = false;
        if (error_string) *error_string = "VSX Engine could not find or load module: "+mc->parts[1];
        vsx_printf( L"%hs\n",vsx_string<>(
                  "**************************************************\n"
                  "Notice: \n\tVSX Engine could not load module: "
                  "'"+mc->parts[1]+"'"
                  "\n\tThis prevented the project: \n\t\t'"+
                  info_filename+"'\n"+
                  "\tfrom loading.\n"
                  "\tThis is most likely from your GPU/drivers do not support some feature needed.\n"
                  "\tPlease consider upgrading your hardware or drivers.\n"
                  "\tIf you are a developer, this might mean some modules didn't compile properly.\n"
                  "**************************************************\n\n").c_str() );

        LOG3("Module missing in engine: "+mc->parts[1]);
        return 1;
      }
    }
  }
  static vsx_string<>sld("state_load_done");
  load1.add_raw(sld);
  load1.reset();

  LOG("i_load_state: all modules are available, proceeding with load")

  LOG("stopping")
  stop();
  LOG("i_clear")
  i_clear();
  LOG("start()")

  start();
  LOG("i_load_state pre processing_message_queue")

  process_message_queue(&load1,&loadr2,true,true);
  LOG("i_load_state post processing_message_queue")
  loadr2.clear_normal();

  current_state = VSX_ENGINE_LOADING;
  g_timer.start();
  modules_loaded = 0;
  modules_left_to_load = 0;
  return 0;
}

vsx_comp* vsx_engine_abs::add(vsx_string<>label)
{
  if (!valid) return 0x0;
  if (!forge_map[label])
  {
    vsx_comp* comp = new vsx_comp;
    comp->engine_owner = (void*)this;
    comp->name = label;
    forge.push_back(comp);

    // is this a child of a macro?
    vsx_nw_vector< vsx_string<> > c_parts;
    vsx_string<> deli = ".";
    vsx_string_helper::explode(label, deli, c_parts);
    if (c_parts.size() > 1) {
      // ok, we have a macro
      vsx_string<>macro_name = vsx_string_helper::implode(c_parts, deli, 0, 1);
      if (vsx_comp* macro_comp = get_component_by_name(macro_name)) {
        comp->parent = macro_comp;
        macro_comp->children.push_back(comp);
      }
    }
    forge_map[label] = comp;
    return comp;
  }
  return 0x0;
}


// send our current time to the client
void vsx_engine_abs::tell_client_time(vsx_command_list *cmd_out)
{
  if (!valid)
    return;

  if (no_send_client_time)
    return;


  bool send = false;

  if (lastsent < 0.0f || lastsent > 0.01f )
  {
    send = true;
    lastsent = 0;
  }
  if (current_state != last_e_state)
    send = true;

  if (send)
  {
    cmd_out->add_raw("time_upd " + vsx_string_helper::f2s(engine_info.vtime)+" "+vsx_string_helper::i2s(current_state), VSX_COMMAND_GARBAGE_COLLECT);
    // check to see if we should send our sequence pool too
    cmd_out->add_raw("seq_pool time_upd " + vsx_string_helper::f2s(sequence_pool.get_vtime())+" "+vsx_string_helper::i2s(sequence_pool.get_state()), VSX_COMMAND_GARBAGE_COLLECT);
  }

  last_e_state = current_state;
}

void vsx_engine_abs::redeclare_in_params(vsx_comp* comp, vsx_command_list *cmd_out) {
  // 1. get all connections in a list
  //printf("+++redeclaring\n");
  std::list<vsx_engine_param_connection_info*> abs_connections_in;
  vsx_engine_param_list* in = comp->get_params_in();
  in->get_abs_connections(&abs_connections_in);

  // dump out the sequences for those params that have such
  std::map<vsx_string<>, vsx_string<> > sequences;
  std::map<vsx_string<>, vsx_string<> > values;
  for (unsigned long i = 0; i < in->param_id_list.size(); ++i) {
    if (in->param_id_list[i]->sequence) {
      sequences[in->param_id_list[i]->name] = sequence_list.dump_param(in->param_id_list[i]);
      sequence_list.remove_param_sequence(in->param_id_list[i]);
    } else
    {
      values[in->param_id_list[i]->name] = in->param_id_list[i]->get_string();
    }
  }

  comp->re_init_in_params();
  cmd_out->add_raw("in_param_spec "+comp->name+" "+comp->in_param_spec+" c", VSX_COMMAND_GARBAGE_COLLECT);
  comp->module->redeclare_in = false;
  in = comp->get_params_in();

  // repopulate the sequences / values
  for (unsigned long i = 0; i < in->param_id_list.size(); ++i) {
    if (sequences.find(in->param_id_list[i]->name) != sequences.end()) {
      sequence_list.inject_param(in->param_id_list[i], comp, sequences[in->param_id_list[i]->name]);
    } else
    {
      if (values.find(in->param_id_list[i]->name) != values.end()) {
        in->param_id_list[i]->set_string(values[in->param_id_list[i]->name]);
      }
    }
  }

  for (std::list<vsx_engine_param_connection_info*>::iterator it2 = abs_connections_in.begin(); it2 != abs_connections_in.end(); ++it2)
  {
    vsx_engine_param* dparam = in->get_by_name((*it2)->dest_name);

    if (dparam)
    {
      int order = dparam->connect((*it2)->src);
      cmd_out->add_raw("param_connect_volatile "+comp->name+" "+dparam->name+" "+(*it2)->src->owner->component->name+" "+(*it2)->src->name+" "+vsx_string_helper::i2s(order), VSX_COMMAND_GARBAGE_COLLECT);
    }
  }
}

void vsx_engine_abs::redeclare_out_params(vsx_comp* comp, vsx_command_list *cmd_out)
{
  // 1. get all connections in a list
  std::list<vsx_engine_param_connection_info*> abs_connections_out;
  vsx_engine_param_list* out = comp->get_params_out();
  out->get_abs_connections(&abs_connections_out);

  // will nuke all the internal params.
  comp->re_init_out_params();
  cmd_out->add_raw("out_param_spec "+comp->name+" "+comp->out_param_spec+" c", VSX_COMMAND_GARBAGE_COLLECT);
  comp->module->redeclare_out = false;
  out = comp->get_params_out();

  for (std::list<vsx_engine_param_connection_info*>::iterator it2 = abs_connections_out.begin(); it2 != abs_connections_out.end(); ++it2)
  {
    vsx_engine_param* dparam = out->get_by_name((*it2)->src_name);

    if (!dparam)
      continue;

    int order = (*it2)->dest->connect(dparam);
    vsx_string<>dest_comp_name = (*it2)->dest->owner->component->name;
    vsx_string<>srcn = (*it2)->src_name;
    vsx_string<>cn = comp->name;
    vsx_string<>dpn = dparam->name;
    vsx_string<>os = vsx_string_helper::i2s(order);
    cmd_out->add_raw("param_connect_volatile "+dest_comp_name+" "+srcn+" "+cn+" "+dpn+" "+os, VSX_COMMAND_GARBAGE_COLLECT);
  }
}

void vsx_engine_abs::process_message_queue_redeclare(vsx_command_list *cmd_out_res)
{
  for (std::vector<vsx_comp*>::iterator it = forge.begin(); it < forge.end(); ++it)
  {
    if ((*it)->module) {
      if ((*it)->module->redeclare_in) {
        redeclare_in_params(*it,cmd_out_res);
      }
      if ((*it)->module->redeclare_out) {
        redeclare_out_params(*it,cmd_out_res);
      }
      if ((*it)->module->message.size()) {
        cmd_out_res->add_raw("c_msg "+(*it)->name+" "+vsx_string_helper::base64_encode((*it)->module->message), VSX_COMMAND_GARBAGE_COLLECT);
        (*it)->module->message = "";
      }
    }
  }
}

void vsx_engine_abs::send_state_to_client(vsx_command_list *cmd_out)
{
//  if (filesystem.is_archive())
//  {
//    cmd_out->add_raw("server_message "+base64_encode("Packages not possible to edit, ask author for state!"));
//    return;
//  }

  vsx_command_list temp_conn(true);
  vsx_command_list temp_conn_alias(true);
  for (unsigned long i = 0; i < forge.size(); ++i)
  {
    vsx_string<>xs,ys;
    xs = vsx_string_helper::f2s(forge[i]->position.x);
    ys = vsx_string_helper::f2s(forge[i]->position.y);
    vsx_string<>command = "component_create_ok "+forge[i]->name+" "+forge[i]->component_class+" "+xs+" "+ys+" ";

    if (forge[i]->component_class == "macro")
      command += vsx_string_helper::f2s(forge[i]->size);
      else
      command += forge[i]->module_info->identifier;
    if (forge[i]->module_info->output) command += " out";
    if (forge[i]->component_class != "macro") {
      forge[i]->get_params_in()->dump_aliases_and_connections_rc(&temp_conn);
      forge[i]->get_params_out()->dump_aliases_rc(&temp_conn_alias);
    }
    cmd_out->add_raw(command, VSX_COMMAND_GARBAGE_COLLECT);
    cmd_out->add_raw("in_param_spec "+forge[i]->name+" "+forge[i]->in_param_spec, VSX_COMMAND_GARBAGE_COLLECT);
    cmd_out->add_raw("out_param_spec "+forge[i]->name+" "+forge[i]->out_param_spec, VSX_COMMAND_GARBAGE_COLLECT);
    if (forge[i]->has_module_operations())
      cmd_out->add_raw("module_operation_spec " + forge[i]->name + " " + forge[i]->module_operations_as_string(), VSX_COMMAND_GARBAGE_COLLECT);
    //send vsxl presence
    for (unsigned long ii = 0; ii < forge[i]->in_parameters->count(); ++ii) {
      vsx_engine_param* param = forge[i]->get_params_in()->get_by_id(ii);
      if (param->module_param->vsxl_modifier) {
        cmd_out->add_raw("vsxl_pfi_ok "+forge[i]->name+" "+param->name, VSX_COMMAND_GARBAGE_COLLECT);
      }
    }
    if (forge[i]->vsxl_modifier) {
      cmd_out->add_raw("vsxl_cfi_ok "+forge[i]->name, VSX_COMMAND_GARBAGE_COLLECT);
    }
  }

  // conns
  vsx_command_s* outc;
  temp_conn_alias.reset();
  while ( (outc = temp_conn_alias.get()) )
    cmd_out->addc(outc, VSX_COMMAND_GARBAGE_COLLECT);

  sequence_list.get_sequences(&temp_conn);

  temp_conn.reset();
  while ( (outc = temp_conn.get()) )
    cmd_out->addc( outc, VSX_COMMAND_GARBAGE_COLLECT);

  // notes
  for (note_iter = note_map.begin(); note_iter != note_map.end(); note_iter++)
    cmd_out->add_raw(vsx_string<>((*note_iter).second.serialize()), VSX_COMMAND_GARBAGE_COLLECT);
}

void vsx_engine_abs::i_clear(vsx_command_list *cmd_out,bool clear_critical)
{
  std::map<vsx_string<>,vsx_comp*> forge_map_save;
  std::vector<vsx_comp*> forge_save;
  for (std::map<vsx_string<>,vsx_comp*>::iterator fit = forge_map.begin(); fit != forge_map.end(); ++fit) {
    if (!(*fit).second->internal_critical || clear_critical)
    {
      LOG("component deleting: "+(*fit).second->name);


      std::map<vsx_module_param_abs*, std::list<vsx_channel*> >::iterator out_map_channels_iter;
      std::map<vsx_module_param_abs*, std::list<vsx_channel*> > temp_map = (*fit).second->out_map_channels;
      for (out_map_channels_iter = temp_map.begin(); out_map_channels_iter != temp_map.end(); ++out_map_channels_iter) {
        std::list<vsx_channel*>::iterator it;
        for (it = (*out_map_channels_iter).second.begin(); it != (*out_map_channels_iter).second.end(); ++it) {
          (*it)->component->disconnect((*it)->my_param->name,(*fit).second,(*out_map_channels_iter).first->name);
        }
      }
    LOG("delete step 2")
      // ! 3:: remove aliases of other components that have aliased our params and connections (this does this)
      for (std::vector<vsx_engine_param*>::iterator it = (*fit).second->get_params_in()->param_id_list.begin(); it != (*fit).second->get_params_in()->param_id_list.end(); ++it) {

        if ((*it)->sequence) {
          sequence_list.remove_param_sequence((*it));
          //cmd_out->add_raw("pseq_p_ok remove "+(*fit).second->name+" "+(*it)->name);
        }
        interpolation_list.remove(*it);
      }
      // remove aliases AND connections
    LOG("delete step 2b");
      (*fit).second->get_params_in()->unalias_aliased();
      (*fit).second->get_params_out()->unalias_aliased();
    LOG("delete step 3");
      if ((*fit).second->module_info->output)
      outputs.remove_value((*fit).second);
    LOG("delete step 4\n");
      LOG("del "+(*fit).second->name);
      if ((*fit).second->component_class != "macro")
      if ( module_list->find((*fit).second->identifier) )
      {
        LOG("unloading "+(*fit).second->name);
        (*fit).second->unload_module();
      }
      delete ((*fit).second);
      LOG("done deleting"´)
    }
    else
    {
      (*fit).second->position.x = 0;
      (*fit).second->position.y = 0;
      forge_save.push_back((*fit).second);
      forge_map_save[(*fit).first] = (*fit).second;
    }
  }
  LOG("clearing forge");
  forge.clear();
  LOG("clearing forge map");
  forge_map.clear();
  note_map.clear();
  forge = forge_save;
  forge_map = forge_map_save;

  sequence_pool.clear();
  sequence_list.clear_master_sequences();

  loop_point_end = -1.0f;
  last_m_time_synch = 0;
  engine_info.vtime = 0;
  engine_info.dtime = 0;
  engine_info.real_vtime = 0;
  current_state = VSX_ENGINE_STOPPED;
  if (filesystem.get_archive()->is_archive())
  {
    if (cmd_out)
    {
      filesystem.get_archive()->close();
      send_state_to_client(cmd_out);
    }
  }
}


int vsx_engine_abs::get_state_as_commandlist(vsx_command_list &savelist)
{
  vsx_command_list tmp_comp(true);
  vsx_command_list tmp_param_set(true);
  vsx_command_list tmp_connections(true);
  vsx_command_list tmp_aliases(true);
  tmp_aliases.add_raw("break");

  if (meta_information.size())
    tmp_comp.add_raw("meta_set "+vsx_string_helper::base64_encode(meta_information));

  for (forge_map_iter = forge_map.begin(); forge_map_iter != forge_map.end(); ++forge_map_iter)
  {
    vsx_comp* comp = (*forge_map_iter).second;

    if (comp->component_class == "macro")
      tmp_comp.add_raw(vsx_string<>("macro_create ")+(*forge_map_iter).first+" "+vsx_string_helper::f2s(comp->position.x)+" "+vsx_string_helper::f2s(comp->position.y)+" "+vsx_string_helper::f2s((*forge_map_iter).second->size));
    else
    {
      if ((*forge_map_iter).first != "screen0")
        tmp_comp.add_raw("component_create "+comp->identifier+" "+comp->name+" "+vsx_string_helper::f2s(comp->position.x)+" "+vsx_string_helper::f2s(comp->position.y));
      else
        tmp_comp.add_raw("cpp screen0 "+vsx_string_helper::f2s(((*forge_map_iter).second)->position.x)+" "+vsx_string_helper::f2s(((*forge_map_iter).second)->position.y));

      comp->get_params_in()->dump_aliases_and_connections("", &tmp_connections);
      comp->get_params_out()->dump_aliases("", &tmp_aliases);
    }


    for (unsigned long i = 0; i < comp->get_params_in()->param_id_list.size(); ++i)
    {
      bool run = true;
      if (comp->get_params_in()->param_id_list[i]->channel)
      if ((comp->get_params_in()->param_id_list[i]->channel->connections.size()))
      run = false;
      vsx_engine_param* param = comp->get_params_in()->param_id_list[i];
      if (run)
      {
        // check for sequence
        vsx_string<>ss = sequence_list.dump_param(param);
        if (ss != "")
        {
          // sequence is controlling, we need no init value.
          tmp_comp.add_raw(vsx_string<>("pseq_p inject ")+comp->name+" "+param->name+" "+ss);
        } else
        {
          // or dump the value
          vsx_string<>pval = param->get_string();
          if (!param->alias) {
            if (
              pval !=
              param->get_default_string()
            )
            {
              if (
                  param->module_param->type == VSX_MODULE_PARAM_ID_STRING
                  ||
                  param->module_param->type == VSX_MODULE_PARAM_ID_RESOURCE
                )
              {
                tmp_comp.add_raw(vsx_string<>("ps64 ")+comp->name+" "+param->name+" "+vsx_string_helper::base64_encode(pval));
              }
              else
              {
                tmp_comp.add_raw(vsx_string<>("param_set ")+comp->name+" "+param->name+" "+pval);
              }
            }
          }
        }
        // dump pflags
        param->dump_pflags(&tmp_comp);
      }
      #ifndef VSXE_NO_GM
      if (param->module_param->vsxl_modifier) {
        //printf("vsxl modifier present\n");
        vsx_param_vsxl_driver_abs* driver;
        driver = (vsx_param_vsxl_driver_abs*)((vsx_param_vsxl*)param->module_param->vsxl_modifier)->get_driver();
        tmp_comp.add_raw(vsx_string<>("vsxl_pfi ")+comp->name+" "+param->name+" "+vsx_string_helper::i2s(driver->id)+" "+base64_encode(driver->script));
      }
      #endif
    }
    #ifndef VSXE_NO_GM
    if (comp->vsxl_modifier) {
      vsx_comp_vsxl_driver_abs* driver;
      driver = (vsx_comp_vsxl_driver_abs*)((vsx_comp_vsxl*)comp->vsxl_modifier)->get_driver();
      tmp_comp.add_raw(vsx_string<>("vsxl_cfi ")+comp->name+" "+base64_encode(driver->script));
    }
    #endif
  }
  for (note_iter = note_map.begin(); note_iter != note_map.end(); note_iter++)
  {
    tmp_connections.add_raw((*note_iter).second.serialize_state());
  }

  vsx_command_s* outc;
  tmp_comp.reset();
  while ( (outc = tmp_comp.pop()) ) {
    savelist.addc(outc);
    delete outc;
  }
  tmp_aliases.add_raw("break");
  tmp_aliases.reset();
  while ( (outc = tmp_aliases.pop_back()) ) {
    savelist.addc(outc);
    delete outc;
  }
  tmp_connections.reset();
  while ( (outc = tmp_connections.pop_back()) ) {
    savelist.addc(outc);
    delete outc;
  }

  // dump the sequence pool
  sequence_pool.dump_to_command_list(savelist);

  // dump the master sequences with their connections to the sequence pools
  sequence_list.dump_master_channels_to_command_list(savelist);



  // dump the loop point
  if (loop_point_end > 0.0f)
  {
    savelist.add_raw("time_set_loop_point "+vsx_string_helper::f2s(loop_point_end));
  }
  return 0;
}

// [base].[name]
// empty.comp1
// comp1

int vsx_engine_abs::rename_component(vsx_string<>old_identifier, vsx_string<>new_base, vsx_string<>new_name)
{
  // first we need to split up the name so we have the old base and the old name
  vsx_string<>old_base;
  vsx_string<>old_name;
  vsx_nw_vector<vsx_string<> > parts;
  vsx_string<>deli = ".";
  vsx_string_helper::explode(old_identifier,deli,parts);
  old_name = parts[parts.size()-1];

  if (parts.size())
    old_base = vsx_string_helper::implode(parts,deli, 0, 1);


  // we have the basic names set up now find the component
  vsx_comp* old_identifier_component = get_component_by_name(old_identifier);
  if (!old_identifier_component)
    return 0;

  // if we don't want to either move or rename
  if (new_base == "$")
    new_base = old_base;

  if (new_name == "$")
    new_name = old_name;

  vsx_string<> look_for_new_name;
  if (new_base.size())
    look_for_new_name = new_base + ".";
  look_for_new_name +=  new_name;
  vsx_comp* new_possibly_colliding_name = get_component_by_name(look_for_new_name);
  if (new_possibly_colliding_name)
    return 0;



  // if we don't move anything, no reason to change parent
  bool assign_first = (new_base != old_base);
  vsx_comp* dest = 0;
  if (assign_first)
    dest = get_component_by_name(new_base);

  int max_loop = 1;
  if (old_identifier_component->component_class == "macro")
    max_loop = 0;

  std::list< vsx_string<> > macro_comps;
  std::list<vsx_comp*> macro_comp_p;
  std::map<vsx_string<>,vsx_comp*>::iterator m_i = forge_map.find(old_identifier);
  bool first = true;
  bool drun = true;
  // loop and find all components we need to rename
  int runs = 0;
  while (drun) {
    if (m_i != forge_map.end()) {
      vsx_string<>tt = (*m_i).first;
      if (tt.find(old_identifier) == 0 || first) {
        if (first || tt[old_identifier.size()] == vsx_string<>(".")) {
          first = false;
          macro_comps.push_back(tt);
          macro_comp_p.push_back((*m_i).second);
        }
      } else drun = false;
      ++m_i;
    } else drun = false;
    ++runs;

    if (max_loop != 0)
      if (runs >= max_loop)
        drun = false;
  }

  vsx_string<>new_name_ = "";
  // do the actual renaming
  std::list<vsx_comp*>::iterator it_c = macro_comp_p.begin();

  for (std::list< vsx_string<> >::iterator it2 = macro_comps.begin(); it2 != macro_comps.end(); ++it2)
  {
    forge_map.erase(*it2);
    if (new_base.size()) {
      if (old_base.size()) {
        // moving from macro to macro
        new_name_ = new_base+"."+vsx_string_helper::str_replace<char>(old_name,new_name,vsx_string_helper::str_replace<char>(old_base+".","",*it2,1,0),1,0);
      }
      else {
        // moving from root to macro
        new_name_ = new_base+"."+vsx_string_helper::str_replace<char>(old_name,new_name,*it2,1,0);
      }
    }
    else {
      // moving the component to the root (server)
      // from macro to root
      if (old_base.size()) {
        // "old_base.component_name" -> "new_component_name"
        new_name_ = vsx_string_helper::str_replace<char>(old_name,new_name,vsx_string_helper::str_replace<char>(old_base+".","",*it2,1,0),1,0);
        new_name = new_name;
      } else {
        // plain renaming
        new_name_ = vsx_string_helper::str_replace(old_name,new_name,*it2,1,0);
      }
    }
    forge_map[new_name_] = *it_c;
    (*it_c)->name = new_name_;
    ++it_c;
  }

  // actually move the component
  if (assign_first)
  {
    if (old_identifier_component->parent)
    {
      ((vsx_comp*)old_identifier_component->parent)->children.remove(old_identifier_component);
    }
    if (dest)
    old_identifier_component->parent = dest;
    else
    old_identifier_component->parent = 0;
  }
  return 1;
}
