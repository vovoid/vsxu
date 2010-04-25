// Left to do on the sequence pool abstraction layer:
// * fix saving / loading
// *

if (cmd == "seq_pool") {
	//printf("seq_pool %s\n", c->parts[1].c_str());

	if (c->parts[1] == "seq_list") {
		cmd_out->add_raw(c->parts[0]+" "+c->parts[1]+"_ok "+sequence_pool.get_selected()->get_channel_names());
	} else

	// ***************************************
	// Dump names request
	// ***************************************
	// 0=seq_pool 1=dump_names
  if (c->parts[1] == "dump_names") {
		cmd_out->add_raw("seq_pool dump_names "+sequence_pool.dump_names());
  } else


	// ***************************************
	// Set time
	// ***************************************
	// 0=seq_pool 1=time_set 2=[time:float]
  if (c->parts[1] == "time_set") {
  	//printf("time_set: %f\n", s2f(c->parts[2]));
  	sequence_pool.set_time(s2f(c->parts[2]));
  } else

	// ***************************************
	// Add new sequencer pool
	// ***************************************
	// 0=seq_pool 1=add 2=[name]
	if (c->parts[1] == "add") {
		// 1. add
		if (sequence_pool.add(c->parts[2]))
		{
			cmd_out->add_raw("seq_pool dump_names "+sequence_pool.dump_names());
		}
	} else
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
			cmd_out->add_raw("seq_pool del "+c->parts[2]);
			cmd_out->add_raw("seq_pool dump_names "+sequence_pool.dump_names());
		}
	} else
	// ***************************************
	// Clone sequencer pool
	// ***************************************
	// 0=seq_pool 1=clone 2=[name] 3=[dest_name]
	if (c->parts[1] == "clone")
	{
		// 1. clone
		//printf("clone seq_pool\n");
		if (sequence_pool.clone(c->parts[2], c->parts[3]))
		{
			cmd_out->add_raw("seq_pool dump_names "+sequence_pool.dump_names());
		}
		else
			FAIL("Sequence Pool", "No sequence found or duplicate name!");
	} else

	// ***************************************
	// Add parameter to current selected sequencer pool
	// ***************************************
	// 0=seq_pool 1=add_param 2=[component] 3=[parameter]
	if (c->parts[1] == "add_param") {
    vsx_comp* component = get_by_name(c->parts[2]);
    if (component) {
      vsx_engine_param* param = component->get_params_in()->get_by_name(c->parts[3]);
      if (sequence_pool.get_selected())
      {
      	sequence_pool.get_selected()->add_param_sequence(param,(vsx_comp_abs*)component);
	      sequence_pool.get_selected()->get_init(param,cmd_out,((vsx_comp_abs*)component)->name,"seq_pool ");
      } else
      	FAIL("Sequence Pool", "No sequence pool selected!");
    }
	} else
	// ***************************************
	// Toggle EDIT on/off
	// ***************************************
	// 0=seq_pool 1=toggle_edit
	if (c->parts[1] == "toggle_edit") {
		bool value = sequence_pool.toggle_edit();
		cmd_out->add_raw("seq_pool toggle_edit "+i2s((int)value));
	} else

	// ***************************************
	// Select a pool
	// ***************************************
	// 0=seq_pool 1=select 2=[name]
	if (c->parts[1] == "select") {
		// 1. select
		//printf("select seq_pool\n");
		if (sequence_pool.select(c->parts[2])) {
			// dump all the sequences up to the sequencer
			cmd_out->add_raw("seq_pool clear_sequencer ");
			cmd_out->add_raw("seq_pool dump_names "+sequence_pool.dump_names());
		}
	} else
	//
	if (c->parts[1] == "pseq_l_dump") {
		// dump all the sequences present in the current sequence pool
		if (sequence_pool.get_selected())
		{
			vsx_string a = sequence_pool.get_selected()->get_sequence_list_dump();
			//printf("a is: %s\n", a.c_str());
			cmd_out->add_raw("seq_pool "+c->parts[1]+"_ok "+a);
		}
	} else
	// 0=seq_pool 1=pseq_inject 2=[seq_pool_name] 3=[component] 4=[param] 5=[data]
	if (c->parts[1] == "pseq_inject") {
		// dump all the sequences present in the engine
		if (sequence_pool.get_sequence_list_by_name(c->parts[2]))
		{
			vsx_comp* component = get_by_name(c->parts[3]);
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
	} else


  // ***************************************
	// Legacy sequence pattern modifier
	// ***************************************
	// 0=seq_pool 1=pseq_p 2=command 3=[component] 4=[param]
	if (c->parts[1] == "pseq_p") {
		if (sequence_pool.get_selected())
    {
			vsx_comp* component = get_by_name(c->parts[3]);
			if (component) {
				vsx_engine_param* param = component->get_params_in()->get_by_name(c->parts[4]);
				if (param)
				{
					if (c->parts[2] == "inject") {
						//param->sequence = true;
						//sequence_list.inject_param(param, dest, c->parts[4]);
					} else
					// ***************************************
					// Inject Get (dump a sequencer up to the GUI)
					// ***************************************
					// 0=seq_pool 1=pseq_p 2=inject_get 3=[component] 4=[param]
					if (c->parts[2] == "inject_get") {
						if (sequence_pool.get_selected())
						{
							vsx_string sequence_specification = sequence_pool.get_selected()->dump_param(param);
							if (sequence_specification != "") {
								cmd_out->add_raw("seq_pool pseq_p_ok inject_get "+c->parts[3]+" "+c->parts[4]+" "+sequence_specification+" "+i2s(param->module_param->type));
							}
						}
					} else
					// 0=seq_pool 1=pseq_p 2=remove 3=[component] 4=[param]
					if (c->parts[2] == "remove") {
						if (sequence_pool.get_selected())
						{
							sequence_pool.get_selected()->remove_param_sequence(param);
						}
						cmd_out->add_raw("seq_pool pseq_p_ok remove "+c->parts[3]+" "+c->parts[4]);
					}
				}
			}
    }
	}
 	// ***************************************
	// Legacy sequence pattern modifier
	// ***************************************
	// 0=seq_pool 1=pseq_p 2=command 3=[component] 4=[param]
	if (c->parts[1] == "pseq_r") {
		if (sequence_pool.get_selected())
		{
			vsx_comp* component = get_by_name(c->parts[3]);
			if (component) {
				vsx_engine_param* param = component->get_params_in()->get_by_name(c->parts[4]);
				if (param) {
					for (int i = 0; i < (int)c->parts.size()-1; i++)
						c->parts[i] = c->parts[i+1];
					// add
					//if (c->parts[1] == "add") {
					// seq_pool.get_selected()->add_line(param, cmd_out, c,"seq_pool ");
					//} else
					// update
					if (c->parts[1] == "update") {
						sequence_pool.get_selected()->update_line(param, cmd_out, c,"seq_pool ");
					} else
					// insert
					if (c->parts[1] == "insert") {
						sequence_pool.get_selected()->insert_line(param, cmd_out, c,"seq_pool ");
					} else
					//remove
					if (c->parts[1] == "remove") {
						sequence_pool.get_selected()->remove_line(param, cmd_out, c,"seq_pool ");
					}
				}
			}
		}
	}
}
else
