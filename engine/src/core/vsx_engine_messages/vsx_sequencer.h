//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++
//++  PARAM SEQUENCER
//++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // PATTERN/SEQUENCE MANAGEMENT
    if (cmd == "seq_list") {
    	cmd_out->add_raw(c->parts[0]+"_ok "+sequence_list.get_channel_names());
    } else
    if (cmd == "pseq_l_dump") {
      // dump all the sequences present in the engine
      cmd_out->add_raw(c->parts[0]+"_ok "+sequence_list.get_sequence_list_dump());
    } else
    if (cmd == "pseq_l_rescale_time") {
      // dump all the sequences present in the engine
      sequence_list.rescale_time(s2f(c->parts[1]),s2f(c->parts[2]));
      //cmd_out->add_raw(c->parts[0]+"_ok "+c->parts[1]+" "+sequence_list.get_sequences_dump());
    } else
    if (cmd == "pseq_p") {
      // list the params linked to sequencers
      if (c->parts[1] == "list") {
        //printf("pseq_p list\n");
        sequence_list.get_sequences(cmd_out);
      }
      else
      {
        vsx_comp* dest = get_by_name(c->parts[2]);
        if (dest) {
          vsx_engine_param* param = dest->get_params_in()->get_by_name(c->parts[3]);
          //vsx_module_param_abs* param = p->module_param;
          if (param) {
            if (c->parts[1] == "inject") {
              param->sequence = true;
              sequence_list.inject_param(param, dest, c->parts[4]);
            } else
            if (c->parts[1] == "inject_get") {
#ifndef VSX_NO_CLIENT
              vsx_string a = sequence_list.dump_param(param);
              if (a != "") {
                cmd_out->add_raw("pseq_p_ok inject_get "+c->parts[2]+" "+c->parts[3]+" "+a+" "+i2s(param->module_param->type));
              }
#endif
            } else
            if (c->parts[1] == "add") {
              if (!param->sequence) {
                param->sequence = true;
                sequence_list.add_param_sequence(param,(vsx_comp_abs*)dest);
              }
              sequence_list.get_init(param,cmd_out,((vsx_comp_abs*)dest)->name);
            } else
            if (c->parts[1] == "remove") {
              sequence_list.remove_param_sequence(param);
              param->sequence = false;
#ifndef VSX_NO_CLIENT
              cmd_out->add_raw("pseq_p_ok remove "+c->parts[2]+" "+c->parts[3]);
#endif
            }
          }
        }
      }
    } else
    // PATTERN/SEQUENCE ROW MANAGEMENT
#ifndef VSX_NO_CLIENT
    if (cmd == "pseq_r") {
      vsx_comp* dest = get_by_name(c->parts[2]);
      if (dest) {
        vsx_engine_param* param = dest->get_params_in()->get_by_name(c->parts[3]);
        if (param) {
          // 0=pseq_r 1=add 2=
          //if (c->parts[1] == "add") {
//            sequence_list.add_line(param, cmd_out, c);
  //        } else
          // update
          if (c->parts[1] == "update") {
            sequence_list.update_line(param, cmd_out, c);
          } else
          // 0=pseq_r 1=insert 2=[module] 3=[param] 4=[value] 5=[local_time_distance] 6=[interpolation_type] 7=[item_action_id]
          if (c->parts[1] == "insert") {
            sequence_list.insert_line(param, cmd_out, c);
          } else
          //remove
          if (c->parts[1] == "remove") {
            sequence_list.remove_line(param, cmd_out, c);
          }
        }
      }
    } else
#endif
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
					cmd_out->add_raw("mseq_channel_ok add "+c->parts[2]);
				} else
				{
					FAIL("Master Sequence Channel", "There seems to already be a channel with this name!");
				}
			}
			else
			if (c->parts[1] == "remove")
			{
				if (sequence_list.remove_master_channel(c->parts[2]))
				{
					cmd_out->add_raw("mseq_channel_ok remove "+c->parts[2]);
				} else
				{
					FAIL("Master Sequence Channel", "There is no channel by that name!");
				}
			}
			else
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
			}
			else
			// 0=mseq_channel 1=inject 2=[channel_name] 3=[data]
			if (c->parts[1] == "inject")
			{
				sequence_list.inject_master_channel(c->parts[2], c->parts[3]);
			}
			else
			// 0=mseq_channel 1=inject_get 2=[channel_name]
			if (c->parts[1] == "inject_get")
			{
#ifndef VSX_NO_CLIENT
        vsx_string a = sequence_list.dump_master_channel(c->parts[2]);
        if (a != "")
        {
          cmd_out->add_raw("mseq_channel_ok inject_get "+c->parts[2]+" "+a);
        }
#endif
      }
		}
		else


















