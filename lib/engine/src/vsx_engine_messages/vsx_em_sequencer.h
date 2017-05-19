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

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++
//++  PARAM SEQUENCER
//++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// PATTERN/SEQUENCE MANAGEMENT
if (cmd == "seq_list")
{
  cmd_out->add_raw(c->parts[0]+"_ok "+sequence_list.get_channel_names(), VSX_COMMAND_GARBAGE_COLLECT);
  goto process_message_queue_end;
} else





if (cmd == "pseq_l_dump")
{
  // dump all the sequences present in the engine
  cmd_out->add_raw(c->parts[0]+"_ok "+sequence_list.get_sequence_list_dump(), VSX_COMMAND_GARBAGE_COLLECT);
  goto process_message_queue_end;
}





if (cmd == "pseq_l_rescale_time")
{
  // dump all the sequences present in the engine
  sequence_list.rescale_time(vsx_string_helper::s2f(c->parts[1]),vsx_string_helper::s2f(c->parts[2]));
  goto process_message_queue_end;
}



if (cmd == "pseq_inject_get_keyframe_at_time") {
  float time = vsx_string_helper::s2f( c->parts[1] );
  float tolerance = vsx_string_helper::s2f( c->parts[2] );
  vsx_nw_vector<vsx_engine_param* > result_params;

  sequence_list.get_params_with_keyframe_at_time(time, tolerance, result_params);
  for (size_t i = 0; i < result_params.size(); i++)
  {
    vsx_string<> seq_dump = sequence_list.dump_param( result_params[i] );
    cmd_out->add_raw(
      "pseq_p_ok "
      "inject_get " +
      result_params[i]->component->name + " " +
      result_params[i]->name + " " +
      seq_dump + " " +
      vsx_string_helper::i2s(result_params[i]->module_param->type),
      VSX_COMMAND_GARBAGE_COLLECT
    );
  }
}



if (cmd == "pseq_p")
{
  // list the params linked to sequencers
  if (c->parts[1] == "list")
  {
    sequence_list.get_sequences(cmd_out);
    goto process_message_queue_end;
  }

  vsx_comp* dest = get_component_by_name(c->parts[2]);
  if (!dest)
    goto process_message_queue_end;

  vsx_engine_param* param = dest->get_params_in()->get_by_name(c->parts[3]);
  if (!param)
    goto process_message_queue_end;

  if (c->parts[1] == "inject")
    sequence_list.inject_param(param, dest, c->parts[4]);

  if (c->parts[1] == "inject_get") {
    vsx_string<>a = sequence_list.dump_param(param);
    if (a != "")
      cmd_out->add_raw(
        "pseq_p_ok "
        "inject_get " +
        param->component->name + " " +
        param->name + " " +
        a + " " +
        vsx_string_helper::i2s(param->module_param->type),
        VSX_COMMAND_GARBAGE_COLLECT
      );
  }

  // 0=pseq_p 1=command 2=[component] 3=[param] 4=[data]
  if (c->parts[1] == "save_data")
  {
    vsx_string<> filename = vsx_data_path::get_instance()->data_path_get() +  "sequences" + DIRECTORY_SEPARATOR + c->parts[4];

    vsx_string_helper::write_to_file(
      filename,
      sequence_list.dump_param_values(param)
    );
  }


  if (c->parts[1] == "add") {
    if (!param->sequence) {
      sequence_list.add_param_sequence(param,(vsx_comp_abs*)dest);
    }
    sequence_list.get_init(param,cmd_out,((vsx_comp_abs*)dest)->name);
  }

  if (c->parts[1] == "remove") {
    sequence_list.remove_param_sequence(param);
    cmd_out->add_raw("pseq_p_ok remove "+c->parts[2]+" "+c->parts[3], VSX_COMMAND_GARBAGE_COLLECT);
  }

  if (c->parts[1] == "save")
  {
    vsx_comp* dest = get_component_by_name(c->parts[2]);
    if (!dest)
      goto process_message_queue_end;

    vsx_engine_param* param = dest->get_params_in()->get_by_name(c->parts[3]);
    if (!param)
      goto process_message_queue_end;

    if (c->parts.size() != 5)
      goto process_message_queue_end;

    vsx_string_helper::write_to_file(
      vsx_data_path::get_instance()->data_path_get() +  "sequences" + DIRECTORY_SEPARATOR + c->parts[4],
      vsx_param_helper::param_name_from_id(param->module_param->type) + "\n" +
      sequence_list.dump_param(param)
    );
  }
  goto process_message_queue_end;
}





// PATTERN/SEQUENCE ROW MANAGEMENT
if (cmd == "pseq_r")
{
  vsx_comp* dest = get_component_by_name(c->parts[2]);
  if (dest) {
    vsx_engine_param* param = dest->get_params_in()->get_by_name(c->parts[3]);
    if (param) {
      // update
      if (c->parts[1] == "update") {
        sequence_list.update_line(param, cmd_out, c);
      } else
      // 0=pseq_r 1=insert 2=[module] 3=[param] 4=[value] 5=[local_time_distance] 6=[interpolation_type] 7=[item_action_id]
      if (c->parts[1] == "insert") {
        sequence_list.insert_line(param, cmd_out, c);
      } else
      // insert absolute
      if (c->parts[1] == "insert_absolute")
      {
        sequence_list.insert_line_absolute(param, cmd_out, c);
      } else
      //remove
      if (c->parts[1] == "remove") {
        sequence_list.remove_line(param, cmd_out, c);
      }
    }
  }
  goto process_message_queue_end;
}



// ***************************** MASTER CHANNELS *******************************
// ***************************** MASTER CHANNELS *******************************
// ***************************** MASTER CHANNELS *******************************
// ***************************** MASTER CHANNELS *******************************
if (cmd == "mseq_channel")
{
  if (c->parts[1] == "add")
  {
    if (sequence_list.add_master_channel(c->parts[2]))
    {
      cmd_out->add_raw("mseq_channel_ok add "+c->parts[2], VSX_COMMAND_GARBAGE_COLLECT);
    } else
    {
      FAIL("Master Sequence Channel", "There seems to already be a channel with this name!");
    }
    goto process_message_queue_end;
  }






  if (c->parts[1] == "remove")
  {
    if (sequence_list.remove_master_channel(c->parts[2]))
    {
      cmd_out->add_raw("mseq_channel_ok remove "+c->parts[2], VSX_COMMAND_GARBAGE_COLLECT);
    } else
    {
      FAIL("Master Sequence Channel", "There is no channel by that name!");
    }
    goto process_message_queue_end;
  }




  // 0=mseq_channel 1=row 2=[operation] 3=[name] [...]
  if (c->parts[1] == "row")
  {
    // 0=mseq_channel 1=row 2=insert 3=[channel_name] 4=[item_action_id] 5=[local_time_distance] 6=[length]
    if (c->parts[2] == "insert")
    {
      sequence_list.insert_master_channel_line(c->parts[3],cmd_out,c);
    } else
    if (c->parts[2] == "update")
    {
      sequence_list.update_master_channel_line(c->parts[3],cmd_out,c);
    } else
    if (c->parts[2] == "remove")
    {
      sequence_list.remove_master_channel_line(c->parts[3],cmd_out,c);
    } else
    // 0=mseq_channel 1=row 2=item_time_sequence 3=[name] 4=[item_id] 5=[get]/[set] 6(optional)=[sequence_dump]
    // this command manipulates the time sequencer for an item
    if (c->parts[2] == "time_sequence")
    {
      sequence_list.time_sequence_master_channel_line(c->parts[3], cmd_out, c);
    }
    goto process_message_queue_end;
  }


  // 0=mseq_channel 1=inject 2=[channel_name] 3=[data]
  if (c->parts[1] == "inject")
  {
    sequence_list.inject_master_channel(c->parts[2], c->parts[3]);
    goto process_message_queue_end;
  }




  // 0=mseq_channel 1=inject_get 2=[channel_name]
  if (c->parts[1] == "inject_get")
  {
    vsx_string<>a = sequence_list.dump_master_channel(c->parts[2]);
    if (a != "")
    {
      cmd_out->add_raw("mseq_channel_ok inject_get "+c->parts[2]+" "+a, VSX_COMMAND_GARBAGE_COLLECT);
    }
    goto process_message_queue_end;
  }

  goto process_message_queue_end;
}



















