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

if (cmd == "component_create")
{
  if (c->parts.size() == 5)
  {
    // syntax:
    //  component_create math_logic;oscillator_dlux macro1.my_oscillator 0.013 0.204
    if (!get_component_by_name(c->parts[2])) {
      if (module_list->find(c->parts[1])) {
        vsx_comp* comp = add(c->parts[2]);
        comp->load_module(c->parts[1],&engine_info);

        comp->identifier = c->parts[1];
        if ( comp->module_info->identifier_save != "")
          comp->identifier = comp->module_info->identifier_save;

        if (comp->module_info->output) {
          outputs.push_back(comp);
        }

        comp->position.x = vsx_string_helper::s2f(c->parts[3]);
        comp->position.y = vsx_string_helper::s2f(c->parts[4]);
        cmd_out->add_raw("component_create_ok "+c->parts[2]+" "+get_component_by_name(c->parts[2])->component_class+" "+c->parts[3]+" "+c->parts[4]+" "+c->parts[1], VSX_COMMAND_GARBAGE_COLLECT);
        cmd_out->add_raw("in_param_spec "+comp->name+" "+comp->in_param_spec, VSX_COMMAND_GARBAGE_COLLECT);
        cmd_out->add_raw("out_param_spec "+comp->name+" "+comp->out_param_spec, VSX_COMMAND_GARBAGE_COLLECT);

        if (comp->has_module_operations())
          cmd_out->add_raw("module_operation_spec " + comp->name + " " + comp->module_operations_as_string(), VSX_COMMAND_GARBAGE_COLLECT);
      } else
      {
        cmd_out->add_raw("alert_fail [component_create] Error "+vsx_string_helper::base64_encode("Engine does not know the module '"+c->parts[1]+"'"), VSX_COMMAND_GARBAGE_COLLECT);
      }
    } else
    {
      cmd_out->add_raw("alert_fail [component_create] Error "+vsx_string_helper::base64_encode("There is already a component '"+c->parts[2]+"'"), VSX_COMMAND_GARBAGE_COLLECT);
    }
  }
  goto process_message_queue_end;
}



if (cmd == "component_delete")
{
  if (c->parts.size() == 2)
  {
    std::map<vsx_string<>,vsx_comp*> temp_forge_map = forge_map;
    std::list<vsx_comp*> to_delete;
    forge_map_iter = temp_forge_map.find(c->parts[1]);
    if (forge_map_iter != temp_forge_map.end()) {
      // some INTENSE princess training needed here, gema!
      bool drun = true;
      bool macro = ((*forge_map_iter).second->component_class == "macro");
      while (drun) {
        if (!macro) drun = false;
        if (forge_map_iter != temp_forge_map.end())
        {
          vsx_string<>t = (*forge_map_iter).first;
          vsx_comp* comp = (*forge_map_iter).second;
          if ((t == c->parts[1]) || (macro && (t.find(c->parts[1]+".") == 0))) {
            std::map<vsx_module_param_abs*, std::list<vsx_channel*> >::iterator out_map_channels_iter;
            std::map<vsx_module_param_abs*, std::list<vsx_channel*> > temp_map = comp->out_map_channels;
            for (out_map_channels_iter = temp_map.begin(); out_map_channels_iter != temp_map.end(); ++out_map_channels_iter) {
              std::list<vsx_channel*>::iterator it;
              for (it = (*out_map_channels_iter).second.begin(); it != (*out_map_channels_iter).second.end(); ++it) {
                (*it)->component->disconnect((*it)->my_param->name,comp,(*out_map_channels_iter).first->name);
              }
            }
            // ! 3:: remove aliases of other components that have aliased our params and connections (this does this)
            for
            (
              std::vector<vsx_engine_param*>::iterator it = comp->get_params_in()->param_id_list.begin();
              it != comp->get_params_in()->param_id_list.end();
              ++it
            )
            {
              if ((*it)->sequence)
              {
                sequence_list.remove_param_sequence((*it));
                cmd_out->add_raw("pseq_p_ok remove "+comp->name+" "+(*it)->name, VSX_COMMAND_GARBAGE_COLLECT);
              }
              sequence_pool.remove_param_sequence((*it));
              interpolation_list.remove(*it);
            }
            // remove aliases AND connections
            comp->get_params_in()->unalias_aliased();
            comp->get_params_out()->unalias_aliased();

            ++forge_map_iter;
            forge_map.erase(t);
            bool fdrun = true;
            std::vector<vsx_comp*>::iterator fit = forge.begin();

            while (fdrun) {
              if (fit != forge.end()) {
                if ((*fit) == comp)
                {
                  forge.erase(fit);
                  fdrun = false;
                }
                if (fdrun) {
                  ++fit;
                }
              } else fdrun = false; //-buu
            }

            if (comp->module_info->output) {
              fdrun = true;
              outputs.remove_value(comp);
            }
            // unload the module
            if (!macro)
            {
              comp->unload_module();
            }

            to_delete.push_back(comp);
          } else drun = false;
        } else drun = false;
      }
      // delete the components listed in to_delete
      for (std::list<vsx_comp*>::iterator it_td = to_delete.begin(); it_td != to_delete.end(); ++it_td) {
        delete (*it_td);
      }
      cmd_out->add_raw("component_delete_ok "+c->parts[1], VSX_COMMAND_GARBAGE_COLLECT);
    } else cmd_out->add_raw("alert_fail "+vsx_string_helper::base64_encode(c->raw)+" Error "+vsx_string_helper::base64_encode("Error, component '"+c->parts[1]+"' does not exist!"), VSX_COMMAND_GARBAGE_COLLECT);
  }
  goto process_message_queue_end;
}







if (cmd == "component_assign")
{
  // syntax:
  //  0=component_assign [1=macro name] [2=master_component],[component],[component],... [3=pos_x] [4=pos_y]
  // Moving components to a macro/outside and keeping existing connections, aliases are positively WASTED! :3
  // NOTE! It's up to the GUI to put the master component first (used for distance measuring for correct placement)
  //
  // 1. Get a descriptive list of connections on the engine level (endpoints)
  // 2. Unalias/Disconnect all those in the engine
  // 3. Move/rename the components
  // 4. Go through all connection infos and reconnect as best we can with the VSXu CONNECT_FAR algorithm, create aliases when needed

  if (c->parts.size() != 5)
  {
    cmd_out->add_raw("invalid_command wrong_number_of_arguments "+vsx_string_helper::base64_encode(c->raw), VSX_COMMAND_GARBAGE_COLLECT);
    goto process_message_queue_end;
  }




  vsx_comp* dest = get_component_by_name(c->parts[1]);
  if (!dest)
    c->parts[1] = "";

  vsx_string<>deli = ",";
  vsx_nw_vector <vsx_string<> > comp_source;
  vsx_string_helper::explode(c->parts[2],deli,comp_source);
  std::vector<vsx_comp*> components;
  // 0. Go through the list of components, all here  needs to be done for each and one component exclusively
  foreach(comp_source, i)
  {
    vsx_comp* comp = get_component_by_name( comp_source[i] );
    req_continue(comp);
    components.push_back(comp);
  }

  // check if we can do the operation
  bool namecheck = true;
  vsx_string<>first_part;
  vsx_string<>comp_name;
  for (std::vector<vsx_comp*>::iterator it = components.begin(); it != components.end(); ++it)
  {
    first_part = c->parts[1]+".";
    comp_name = (*it)->name;
    if (c->parts[1] == "") {
      // not moving it to a macro
      if ((*it)->parent) {
        comp_name = vsx_string_helper::str_replace<char>((*it)->parent->name+".","",(*it)->name);
      }
    } else {
      if ((*it)->parent)
      comp_name = vsx_string_helper::str_replace<char>((*it)->parent->name+".","",(*it)->name);
    }

    if (forge_map.find(first_part+comp_name) != forge_map.end()) {
      it = components.end();
    }
  }
  if (!namecheck)
  {
    cmd_out->add_raw("alert_fail "+vsx_string_helper::base64_encode(c->raw)+" Error "+vsx_string_helper::base64_encode("Error, there is already a component named "+first_part+comp_name), VSX_COMMAND_GARBAGE_COLLECT);
    goto process_message_queue_end;
  }

  std::list<vsx_engine_param_connection_info*> abs_connections_in;
  std::list<vsx_engine_param_connection_info*> abs_connections_out;
  for (std::vector<vsx_comp*>::iterator it = components.begin(); it != components.end(); ++it) {
    // 0. Check that we actually CAN move it - that there isn't already a component on the same level sharing the same name
    //bool already_there = false;
    // 1. Get a list of connections on the engine level.
    //    this needs to contain info on to what engine_param_list the connection is headed, and wich is the real owner in our component
    // 1a. Start with the in-bound paramlist
    vsx_engine_param_list* in = (*it)->get_params_in();
    vsx_engine_param_list* out = (*it)->get_params_out();
    in->get_abs_connections(&abs_connections_in);
    out->get_abs_connections(&abs_connections_out);
  }

  // the first component is the 'MASTER' - the one the user moves.
  // Thus we can calculate the distance between all selected components and this one and then apply this
  // delta value to the new position
  vsx_vector3<> new_position(vsx_string_helper::s2f(c->parts[3]),vsx_string_helper::s2f(c->parts[4]));
  vsx_vector3<> master_position = ((*(components.begin()))->position);

  for (std::vector<vsx_comp*>::iterator it = components.begin(); it != components.end(); ++it) {
    // 2. Unalias/Disconnect all in the engine level
    vsx_engine_param_list* in = (*it)->get_params_in();
    vsx_engine_param_list* out = (*it)->get_params_out();
    in->disconnect_abs_connections();
    out->disconnect_abs_connections();

    // 3. Move/rename the component
    rename_component((*it)->name,c->parts[1],"$");

    // a true vector operation \o/
    (*it)->position = new_position + (*it)->position - master_position;

    // old positioning code, TODO: remove this in 0.1.19
    //(*it)->position.x = vsx_string_helper::s2f(c->parts[3]);
    //(*it)->position.y = vsx_string_helper::s2f(c->parts[4]);

  }

  for (std::list<vsx_engine_param_connection_info*>::iterator it = abs_connections_in.begin(); it != abs_connections_in.end(); ++it) {
    (*it)->dest->connect_far_abs(*it,(*it)->localorder);
    delete *it;
  }

  for (std::list<vsx_engine_param_connection_info*>::iterator it = abs_connections_out.begin(); it != abs_connections_out.end(); ++it) {
    (*it)->dest->connect_far_abs(*it,(*it)->localorder);
    delete *it;
  }

  cmd_out->add_raw(c->parts[0]+"_ok "+c->parts[1]+" "+c->parts[2]+" "+c->parts[3]+" "+c->parts[4], VSX_COMMAND_GARBAGE_COLLECT);
  goto process_message_queue_end;
}






if (cmd == "component_rename")
{
  if (c->parts.size() == 3) {
    // component_rename macro1.macro2.component new_name
    // will be:
    // macro1.macro2.new_name
    if ( rename_component(c->parts[1],"$",c->parts[2]) )
      cmd_out->add_raw("component_rename_ok "+c->parts[1]+" "+c->parts[2], VSX_COMMAND_GARBAGE_COLLECT);
    else
      cmd_out->add_raw(vsx_string<>("alert_fail ")+vsx_string_helper::base64_encode(c->raw)+" Error "+vsx_string_helper::base64_encode("Rename failed."), VSX_COMMAND_GARBAGE_COLLECT);
  }
  goto process_message_queue_end;
}








if (cmd == "cpp" || cmd == "component_pos")
{
  if (c->parts.size() == 4) {
    vsx_comp* dest = get_component_by_name(c->parts[1]);
    if (dest) {
      dest->position.x = vsx_string_helper::s2f(c->parts[2]);
      dest->position.y = vsx_string_helper::s2f(c->parts[3]);
    }
  }
  goto process_message_queue_end;
}






if (cmd == "component_size")
{
  if (c->parts.size() == 3) {
    vsx_comp* dest = get_component_by_name(c->parts[1]);
    if (dest) {
      dest->size = vsx_string_helper::s2f(c->parts[2]);
    }
  }
  goto process_message_queue_end;
}





if (cmd == "get_module_status")
{
  for (std::vector<vsx_comp*>::iterator it = forge.begin(); it < forge.end(); ++it) {
    if ((*it)->module) {
      if (!(*it)->module->loading_done) {
        printf("not loaded: %s\n",(*it)->name.c_str());
        cmd_out->add_raw("c_msg "+(*it)->name+" "+vsx_string_helper::base64_encode("module||NOT LOADED"), VSX_COMMAND_GARBAGE_COLLECT);
      } else
      cmd_out->add_raw("c_msg "+(*it)->name+" "+vsx_string_helper::base64_encode("module||ok"), VSX_COMMAND_GARBAGE_COLLECT);
    }
  }
  goto process_message_queue_end;
}






#ifdef VSXU_MODULE_TIMING
if (cmd == "component_timing")
{
  if (c->parts.size() == 3) {
    vsx_comp* src = get_component_by_name(c->parts[1]);
    if (src) {
      cmd_out->add_raw(vsx_string<>("component_timing_ok ")+c->parts[2]+" "+vsx_string_helper::f2s(src->time_run,12)+" "+vsx_string_helper::f2s(src->time_output,12)+" "+vsx_string_helper::f2s(last_frame_time,12), VSX_COMMAND_GARBAGE_COLLECT);
    }
  }
  goto process_message_queue_end;
}

#endif
