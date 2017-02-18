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

// Left to do on the sequence pool abstraction layer:
// * fix saving / loading
// *

if (cmd == "seq_pool")
{
  if (c->parts[1] == "seq_list")
  {
    cmd_out->add_raw(c->parts[0]+" "+c->parts[1]+"_ok "+sequence_pool.get_selected()->get_channel_names(), VSX_COMMAND_GARBAGE_COLLECT);
    goto process_message_queue_end;
  }

  if (c->parts[1] == "group_list")
  {
    cmd_out->add_raw(c->parts[0]+" "+c->parts[1]+"_ok "+sequence_pool.get_selected()->group_dump_all(), VSX_COMMAND_GARBAGE_COLLECT);
    goto process_message_queue_end;
  }

  if (c->parts[1] == "group_del")
  {
    sequence_pool.get_selected()->group_del(c->parts[2]);
    cmd_out->add_raw(c->parts[0]+" group_list_ok "+sequence_pool.get_selected()->group_dump_all(), VSX_COMMAND_GARBAGE_COLLECT);
    goto process_message_queue_end;
  }



  // ***************************************
  // Dump names request
  // ***************************************
  // 0=seq_pool 1=dump_names
  if (c->parts[1] == "dump_names")
  {
    cmd_out->add_raw("seq_pool dump_names "+sequence_pool.dump_names(), VSX_COMMAND_GARBAGE_COLLECT);
    goto process_message_queue_end;
  }



  // ***************************************
  // Set time loop point
  // ***************************************
  // 0=seq_pool 1=time_set_loop_point 2=[time:float]
  if (c->parts[1] == "time_set_loop_point")
  {
    sequence_pool.set_loop_point(vsx_string_helper::s2f(c->parts[2]));
    goto process_message_queue_end;
  }


  // ***************************************
  // Set time
  // ***************************************
  // 0=seq_pool 1=time_set 2=[time:float]
  if (c->parts[1] == "time_set")
  {
    //printf("time_set: %f\n", vsx_string_helper::s2f(c->parts[2]));
    sequence_pool.set_time(vsx_string_helper::s2f(c->parts[2]));
    goto process_message_queue_end;
  }


  // ***************************************
  // Play
  // ***************************************
  // 0=seq_pool 1=play
  if (c->parts[1] == "play")
  {
    sequence_pool.play();
    time_play();
    goto process_message_queue_end;
  }


  // ***************************************
  // Stop
  // ***************************************
  // 0=seq_pool 1=stop
  if (c->parts[1] == "stop") {
    sequence_pool.stop();
    if (engine_time_from_sequence_pool)
      current_state = VSX_ENGINE_STOPPED;
    goto process_message_queue_end;
  }


  // ***************************************
  // Stop
  // ***************************************
  // 0=seq_pool 1=stop
  if (c->parts[1] == "rewind")
  {
    sequence_pool.rewind();
    if (engine_time_from_sequence_pool)
      current_state = VSX_ENGINE_REWIND;
    goto process_message_queue_end;
  }


  // ***************************************
  // Propagate Time - let sequence pool's time control engine's time
  // ***************************************
  // 0=seq_pool 1=propagate_time
  if (c->parts[1] == "propagate_time")
  {
    engine_time_from_sequence_pool = true;
    goto process_message_queue_end;
  }


  // ***************************************
  // Add new sequencer pool
  // ***************************************
  // 0=seq_pool 1=add 2=[name]
  if (c->parts[1] == "add")
  {
    // 1. add
    if (sequence_pool.add(c->parts[2]))
    {
      cmd_out->add_raw("seq_pool dump_names "+sequence_pool.dump_names(), VSX_COMMAND_GARBAGE_COLLECT);
    }
    goto process_message_queue_end;
  }


  // ***************************************
  // Remove sequencer pool
  // ***************************************
  // 0=seq_pool 1=del 2=[name]
  if (c->parts[1] == "del")
  {
    // 1. del
    vsx_param_sequence_list* list = sequence_pool.get_sequence_list_by_name(c->parts[2]);
    //TODO: remove sequences from sequence_list also
    if (list) sequence_list.remove_master_channel_lines_referring_to_sequence_list(list);
    if (sequence_pool.del(c->parts[2]))
    {
      cmd_out->add_raw("seq_pool del "+c->parts[2], VSX_COMMAND_GARBAGE_COLLECT);
      cmd_out->add_raw("seq_pool dump_names "+sequence_pool.dump_names(), VSX_COMMAND_GARBAGE_COLLECT);
    }
    goto process_message_queue_end;
  }



  // ***************************************
  // Clone sequencer pool
  // ***************************************
  // 0=seq_pool 1=clone 2=[name] 3=[dest_name]
  if (c->parts[1] == "clone")
  {
    // 1. clone
    if (sequence_pool.clone(c->parts[2], c->parts[3]))
    {
      cmd_out->add_raw("seq_pool dump_names "+sequence_pool.dump_names(), VSX_COMMAND_GARBAGE_COLLECT);
    }
    else
      FAIL("Sequence Pool", "No sequence found or duplicate name!");
    goto process_message_queue_end;
  }



  // ***************************************
  // Export sequencer list to file
  // ***************************************
  // 0=seq_pool 1=export 2=[filename]
  if (c->parts[1] == "export")
  {
    if (!sequence_pool.export_to_file(c->parts[2]))
    {
      FAIL("Sequence Pool", "Export failed...");
    }
    goto process_message_queue_end;
  }

  // ***************************************
  // Import sequencer list from file
  // ***************************************
  // 0=seq_pool 1=import 2=[filename]
  if (c->parts[1] == "import")
  {
    //printf("import seq_pool\n");
    if (!sequence_pool.import_from_file(c->parts[2]))
    {
      FAIL("Sequence Pool", "Import failed...");
    }
    goto process_message_queue_end;
  }



  // ***************************************
  // Add parameter to current selected sequencer pool
  // ***************************************
  // 0=seq_pool 1=add_param 2=[component] 3=[parameter]
  if (c->parts[1] == "add_param")
  {
    vsx_comp* component = get_component_by_name(c->parts[2]);
    if (component) {
      vsx_engine_param* param = component->get_params_in()->get_by_name(c->parts[3]);
      if (sequence_pool.get_selected())
      {
        sequence_pool.get_selected()->add_param_sequence(param,(vsx_comp_abs*)component);
        sequence_pool.get_selected()->get_init(param,cmd_out,((vsx_comp_abs*)component)->name,"seq_pool ");
      } else
        FAIL("Sequence Pool", "No sequence pool selected!");
    }
    goto process_message_queue_end;
  }



  // ***************************************
  // Toggle EDIT on/off
  // ***************************************
  // 0=seq_pool 1=toggle_edit
  if (c->parts[1] == "toggle_edit")
  {
    bool value = sequence_pool.toggle_edit();
    cmd_out->add_raw("seq_pool toggle_edit "+vsx_string_helper::i2s((int)value), VSX_COMMAND_GARBAGE_COLLECT);
    goto process_message_queue_end;
  }


  // ***************************************
  // Toggle PLAY OVERRIDE on/off
  // ***************************************
  // 0=seq_pool 1=set_play_override 2=value
  if (c->parts[1] == "set_play_override")
  {
    bool value = vsx_string_helper::s2i(c->parts[2]) != 0;
    sequence_pool.set_play_override( value );
    goto process_message_queue_end;
  }


  // ***************************************
  // Select a pool
  // ***************************************
  // 0=seq_pool 1=select 2=[name]
  if (c->parts[1] == "select")
  {
    // 1. select
    //printf("select seq_pool\n");
    if (sequence_pool.select(c->parts[2])) {
      // dump all the sequences up to the sequencer
      cmd_out->add_raw("seq_pool clear_sequencer ", VSX_COMMAND_GARBAGE_COLLECT);
      cmd_out->add_raw("seq_pool dump_names "+sequence_pool.dump_names(), VSX_COMMAND_GARBAGE_COLLECT);
    }
    goto process_message_queue_end;
  }




  if (c->parts[1] == "pseq_l_dump")
  {
    // dump all the sequences present in the current sequence pool
    if (sequence_pool.get_selected())
    {
      vsx_string<>a = sequence_pool.get_selected()->get_sequence_list_dump();
      cmd_out->add_raw("seq_pool "+c->parts[1]+"_ok "+a, VSX_COMMAND_GARBAGE_COLLECT);
    }
    goto process_message_queue_end;
  }




  // 0=seq_pool 1=pseq_inject 2=[seq_pool_name] 3=[component] 4=[param] 5=[data]
  if (c->parts[1] == "pseq_inject")
  {
    if (sequence_pool.get_sequence_list_by_name(c->parts[2]))
    {
      vsx_comp* component = get_component_by_name(c->parts[3]);
      if (component) {
        vsx_engine_param* param = component->get_params_in()->get_by_name(c->parts[4]);
        if (param)
        {
          //printf("pseq injecting param\n");
          sequence_pool.get_sequence_list_by_name(c->parts[2])
            ->
              inject_param(param, component, c->parts[5]);
        }
      }
    }
    goto process_message_queue_end;
  }

  if (c->parts[1] == "pseq_inject_get_keyframe_at_time")
  {
    float time = vsx_string_helper::s2f( c->parts[2] );
    float tolerance = vsx_string_helper::s2f( c->parts[3] );
    vsx_nw_vector<vsx_engine_param* > result_params;

    vsx_param_sequence_list* seq_list = sequence_pool.get_selected();

    seq_list->get_params_with_keyframe_at_time(time, tolerance, result_params);
    for (size_t i = 0; i < result_params.size(); i++)
    {
      vsx_engine_param* param = result_params[i];
      vsx_string<> seq_dump = seq_list->dump_param( param );
      vsx_comp_abs* comp = result_params[i]->component;

      if (seq_dump != "")
        cmd_out->add_raw(
          "seq_pool "
          "pseq_p_ok "
          "inject_get " +
          comp->name + " " +
          result_params[i]->name + " " +
          seq_dump + " " +
          vsx_string_helper::i2s(result_params[i]->module_param->type),
          VSX_COMMAND_GARBAGE_COLLECT
        );
    }
  }

  // 0=seq_pool 1=group_inject 2=[seq_pool_name] 3=[data]
  if (c->parts[1] == "group_inject")
  {
    if (!sequence_pool.get_sequence_list_by_name(c->parts[2]))
      goto process_message_queue_end;

    sequence_pool.get_sequence_list_by_name(c->parts[2])->
        group_inject(c->parts[3]);

    goto process_message_queue_end;

  }


  // ***************************************
  // Legacy sequence pattern modifier
  // ***************************************
  // 0=seq_pool 1=pseq_p 2=command 3=[component] 4=[param]
  if (c->parts[1] == "pseq_p")
  {
    if (sequence_pool.get_selected())
    {
      vsx_comp* component = get_component_by_name(c->parts[3]);
      if (component)
      {
        vsx_engine_param* param = component->get_params_in()->get_by_name(c->parts[4]);
        if (param)
        {
          // ***************************************
          // Inject Get (dump a sequencer up to the GUI)
          // ***************************************
          // 0=seq_pool 1=pseq_p 2=inject_get 3=[component] 4=[param]
          if (c->parts[2] == "inject_get")
          {
            if (sequence_pool.get_selected())
            {
              vsx_string<>sequence_specification = sequence_pool.get_selected()->dump_param(param);
              if (sequence_specification != "")
              {
                cmd_out->add_raw("seq_pool pseq_p_ok inject_get "+c->parts[3]+" "+c->parts[4]+" "+sequence_specification+" "+vsx_string_helper::i2s(param->module_param->type), VSX_COMMAND_GARBAGE_COLLECT);
              }
            }
          } else
          // 0=seq_pool 1=pseq_p 2=remove 3=[component] 4=[param]
          if (c->parts[2] == "remove")
          {
            if (sequence_pool.get_selected())
            {
              sequence_pool.get_selected()->remove_param_sequence(param);
            }
            cmd_out->add_raw("seq_pool pseq_p_ok remove "+c->parts[3]+" "+c->parts[4], VSX_COMMAND_GARBAGE_COLLECT);
          }

          // 0=seq_pool 1=pseq_p 2=command 3=[component] 4=[param] 5=[data]
          if (c->parts[2] == "save")
          {
            if (c->parts.size() != 6)
              goto process_message_queue_end;

            vsx_string_helper::write_to_file(
              vsx_data_path::get_instance()->data_path_get() +  "sequences" + DIRECTORY_SEPARATOR + c->parts[5],
              vsx_param_helper::param_name_from_id(param->module_param->type) + "\n" +
              sequence_pool.get_selected()->dump_param(param)
            );
          }

          // 0=seq_pool 1=pseq_p 2=command 3=[component] 4=[param] 5=[data]
          if (c->parts[2] == "save_data")
          {
            if (c->parts.size() != 6)
              goto process_message_queue_end;

            vsx_string<> filename = vsx_data_path::get_instance()->data_path_get() +  "sequences" + DIRECTORY_SEPARATOR + c->parts[5];

            vsx_string_helper::write_to_file(
              filename,
              sequence_pool.get_selected()->dump_param_values(param)
            );
          }


          // 0=seq_pool 1=pseq_p 2=command 3=[component] 4=[param] 5=[group_name]
          if (c->parts[2] == "group_add")
          {
            if (c->parts.size() != 6)
              goto process_message_queue_end;

            sequence_pool.get_selected()->group_add_param(c->parts[5], c->parts[3]+":"+c->parts[4]);
            cmd_out->add_raw("seq_pool pseq_p_ok group_add "+c->parts[5]+" "+c->parts[3]+" "+c->parts[4]+" "+
                sequence_pool.get_selected()->group_dump(c->parts[5]));
          }

          if (c->parts[2] == "group_del_param")
          {
            if (c->parts.size() != 6)
              goto process_message_queue_end;

            sequence_pool.get_selected()->group_del_param(c->parts[5], c->parts[3]+":"+c->parts[4]);
          }

        }
      }
    }
    goto process_message_queue_end;
  }




  // ***************************************
  // Legacy sequence pattern modifier
  // ***************************************
  // 0=seq_pool 1=pseq_p 2=command 3=[component] 4=[param]
  if (c->parts[1] == "pseq_r")
  {
    if (sequence_pool.get_selected())
    {
      vsx_comp* component = get_component_by_name(c->parts[3]);
      if (component)
      {
        vsx_engine_param* param = component->get_params_in()->get_by_name(c->parts[4]);
        if (param)
        {
          for (int i = 0; i < (int)c->parts.size()-1; i++)
          {
            c->parts[i] = c->parts[i+1];
          }
          if (c->parts[1] == "update")
          {
            sequence_pool.get_selected()->update_line(param, cmd_out, c,"seq_pool ");
          } else
          // insert
          if (c->parts[1] == "insert")
          {
            sequence_pool.get_selected()->insert_line(param, cmd_out, c,"seq_pool ");
          } else
          // insert absolute
          if (c->parts[1] == "insert_absolute")
          {
            sequence_pool.get_selected()->insert_line_absolute(param, cmd_out, c,"seq_pool ");
          } else
          //remove
          if (c->parts[1] == "remove")
          {
            sequence_pool.get_selected()->remove_line(param, cmd_out, c,"seq_pool ");
          }
        }
      }
    }
    goto process_message_queue_end;
  }
  goto process_message_queue_end;
}

