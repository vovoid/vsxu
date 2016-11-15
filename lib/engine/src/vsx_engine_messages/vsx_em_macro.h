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



if (cmd == "macro_dump" || cmd == "component_clone")
{
  // syntax:
  //   macro_dump [name] [save_name]
  // - the most ultimate princess training ever, nyo!
  // first mission: find puchiko

  // sanity
  if (!(c->parts.size() >= 3))
    goto process_message_queue_end;

  vsx_string<>my_name = c->parts[1];
  forge_map_iter = forge_map.find(c->parts[1]);

  // sanity
  if (forge_map_iter == forge_map.end())
    goto process_message_queue_end;

  // puchiko has been found, nyo!
  vsx_command_list tmp_comp(true);
  vsx_command_list tmp_param_set(true);
  vsx_command_list tmp_connections(true);
  vsx_command_list tmp_aliases(true);

  bool drun = true;
  bool macro = ((*forge_map_iter).second->component_class == "macro");
  if (macro)
  {
    tmp_comp.add_raw(vsx_string<>("macro_create $$name ")+vsx_string_helper::f2s((*forge_map_iter).second->size), VSX_COMMAND_GARBAGE_COLLECT);
    ++forge_map_iter;
    while (drun)
    {
      if (forge_map_iter != forge_map.end())
      {
        vsx_string<>t = (*forge_map_iter).first;
        vsx_comp* comp = (*forge_map_iter).second;
        if (t.find(c->parts[1]+".") == 0)
        {
          if (comp->component_class == "macro") {
            tmp_comp.add_raw(vsx_string<>("macro_create ")+vsx_string_helper::str_replace<char>(my_name+".","$$name.",t,1)+" "+vsx_string_helper::f2s(comp->position.x)+" "+vsx_string_helper::f2s(comp->position.y)+" "+vsx_string_helper::f2s(comp->size), VSX_COMMAND_GARBAGE_COLLECT);
          } else {
            tmp_comp.add_raw(vsx_string<>("component_create ")+comp->identifier+" "+vsx_string_helper::str_replace<char>(my_name+".","$$name.",t,1)+" "+vsx_string_helper::f2s(comp->position.x)+" "+vsx_string_helper::f2s(comp->position.y), VSX_COMMAND_GARBAGE_COLLECT);
            comp->get_params_in()->dump_aliases_and_connections(c->parts[1], &tmp_connections);
            comp->get_params_out()->dump_aliases(c->parts[1], &tmp_aliases);
            comp->get_params_in()->dump_param_values(vsx_string_helper::str_replace<char>(my_name+".","$$name.",t,1),&tmp_comp);
          }
        } else drun = false;
        ++forge_map_iter;
      } else drun = false;
    }
  }
  else
  {
    vsx_comp* comp = (*forge_map_iter).second;
    tmp_comp.add_raw(vsx_string<>("component_create ")+comp->identifier+" $$name "+vsx_string_helper::f2s(comp->position.x)+" "+vsx_string_helper::f2s(comp->position.y));
    comp->get_params_in()->dump_param_values("$$name",&tmp_comp);
  }
  vsx_command_s* outc;
  tmp_comp.reset();
  while ( (outc = tmp_comp.get()) )
  {
    cmd_out->add_raw(vsx_string<>(c->parts[0] + "_add ") + c->get_parts(1,2) + " " + vsx_string_helper::base64_encode(outc->raw), VSX_COMMAND_GARBAGE_COLLECT);
  }

  if (tmp_aliases.count())
  {
    tmp_aliases.reset();
    while ( (outc = tmp_aliases.pop_back()) ) {
      cmd_out->add_raw(vsx_string<>(c->parts[0]+"_add ") + c->get_parts(1,2) + " " + vsx_string_helper::base64_encode(outc->raw), VSX_COMMAND_GARBAGE_COLLECT);
    }
  }

  if (tmp_connections.count())
  {
    tmp_connections.reset();
    while ( (outc = tmp_connections.pop_back()) ) {
      cmd_out->add_raw(vsx_string<>(c->parts[0]+"_add ") + c->get_parts(1,2) + " " + vsx_string_helper::base64_encode(outc->raw), VSX_COMMAND_GARBAGE_COLLECT);
    }
  }

  cmd_out->add_raw(vsx_string<>(c->parts[0]+"_complete ")+c->get_parts(1), VSX_COMMAND_GARBAGE_COLLECT);

  goto process_message_queue_end;
}





if (cmd == "macro_prerun")
{
  if (get_component_by_name(c->parts[3])) {
    cmd_out->add_raw(vsx_string<>("alert_fail ") + vsx_string_helper::base64_encode(c->raw) + " Error " + vsx_string_helper::base64_encode("There is already a macro '"+c->parts[3]+"'"), VSX_COMMAND_GARBAGE_COLLECT);
  } else {
    cmd_out->addc(c, VSX_COMMAND_GARBAGE_COLLECT);
  }
  goto process_message_queue_end;
}



if (cmd == "macro_create")
{
  // macro_create [macro_name] [pos_x] [pos_y] [size]

  if (c->parts.size() != 5)
  {
    cmd_out->add_raw("invalid_command wrong_number_of_arguments " + vsx_string_helper::base64_encode(c->raw), VSX_COMMAND_GARBAGE_COLLECT);
    goto process_message_queue_end;
  }

  if (!get_component_by_name(c->parts[1]))
  {
    vsx_comp* comp = add(c->parts[1]);
    // ok we force this to boo macrooo
    comp->component_class = "macro";
    comp->position.x = vsx_string_helper::s2f(c->parts[2]);
    comp->position.y = vsx_string_helper::s2f(c->parts[3]);
    comp->size = vsx_string_helper::s2f(c->parts[4]);
    // the code creating the macro seems pretty similar to that of the component eh?
    cmd_out->add_raw(vsx_string<>("component_create_ok ")+c->parts[1]+" "+get_component_by_name(c->parts[1])->component_class+" "+c->parts[2]+" "+c->parts[3]+" "+c->parts[4], VSX_COMMAND_GARBAGE_COLLECT);
    goto process_message_queue_end;
  }
  cmd_out->add_raw("alert_fail " + vsx_string_helper::base64_encode(c->raw)+" Error " + vsx_string_helper::base64_encode("There is already a macro '"+c->parts[1]), VSX_COMMAND_GARBAGE_COLLECT);

  goto process_message_queue_end;
}

