/*
 * vsx_em_script.h
 *
 *  Created on: Apr 16, 2009
 *      Author: jaw
 */

#ifndef VSX_EM_SCRIPT_H_
#define VSX_EM_SCRIPT_H_
#ifndef VSXE_NO_GM

  // COMPONENT VSXL
  // gui asks for the contents of a vsxl filter (and or creating a new one)
	#ifndef VSX_NO_CLIENT
    if (cmd == "vsxl_cfl") {

      vsx_comp* dest = get_by_name(c->parts[1]);
      if (dest) {
        vsx_comp_vsxl_driver_abs* driver;
        if (!dest->vsxl_modifier) {
          dest->vsxl_modifier = (vsx_comp_vsxl*)(new vsx_comp_vsxl());
          // load with default script
          driver = (vsx_comp_vsxl_driver_abs*)((vsx_comp_vsxl*)dest->vsxl_modifier)->load(dest->in_module_parameters,"");
          driver->comp = (void*)dest;
          driver->run();
          // notify the client!
          cmd_out->add_raw("vsxl_cfi_ok "+c->parts[1]);
        } else {
          //printf("already driver\n");
          driver = (vsx_comp_vsxl_driver_abs*)((vsx_comp_vsxl*)dest->vsxl_modifier)->get_driver();
        }
        if (driver)
        cmd_out->add_raw("vsxl_cfl_s "+c->parts[1]+" "+base64_encode(driver->script));

      }
    }
    else
	#endif // NO CLIENT
  // COMPONENT VSXL
  // init component vsxl filter, run from macros
  if (cmd == "vsxl_cfi") {
    vsx_comp* dest = get_by_name(c->parts[1]);
    if (dest) {
      vsx_comp_vsxl_driver_abs* driver;
      if (!dest->vsxl_modifier) {
        dest->vsxl_modifier = (vsx_comp_vsxl*)(new vsx_comp_vsxl());
        // load with default script
        driver = (vsx_comp_vsxl_driver_abs*)((vsx_comp_vsxl*)dest->vsxl_modifier)->load(dest->in_module_parameters,base64_decode(c->parts[2]));
        driver->comp = (void*)dest;
        driver->run();
        // notify the client!
					#ifndef VSX_NO_CLIENT
						cmd_out->add_raw("vsxl_cfi_ok "+c->parts[1]);
					#endif
        } else driver = (vsx_comp_vsxl_driver_abs*)((vsx_comp_vsxl*)dest->vsxl_modifier)->load((dest->in_module_parameters),base64_decode(c->parts[2]));
      }
    } else
		#ifndef VSX_NO_CLIENT
			// remove vsxl filter from the engine
			if (cmd == "vsxl_cfr") {
				vsx_comp* dest = get_by_name(c->parts[1]);
				if (dest) {
					//vsx_param_vsxl_driver_abs* driver;
					if (dest->vsxl_modifier) {
						((vsx_comp_vsxl*)dest->vsxl_modifier)->unload();
						delete (vsx_comp_vsxl*)(dest->vsxl_modifier);
						dest->vsxl_modifier = 0;
						// send status to client
						cmd_out->add_raw("vsxl_cfr_ok "+c->parts[1]);
					}
				}
			} else

			// PARAMETER VSXL
			// gui asks for the contents of a vsxl filter (and or creating a new one)
			if (cmd == "vsxl_pfl") {
				printf("pfl\n");
				vsx_comp* dest = get_by_name(c->parts[1]);
				if (dest) {
					vsx_engine_param* param = dest->get_params_in()->get_by_name(c->parts[2]);
					vsx_param_vsxl_driver_abs* driver = 0;
					if (!param->module_param->vsxl_modifier) {
					//printf("pfl_2\n");
						param->module_param->vsxl_modifier = (vsx_param_vsxl_abs*)(new vsx_param_vsxl());
						((vsx_param_vsxl*)(param->module_param->vsxl_modifier))->engine = this;
						//printf("pfl_2_2 %d\n", param->module_param->vsxl_modifier);

						// load with default script
						driver = (vsx_param_vsxl_driver_abs*)((vsx_param_vsxl*)param->module_param->vsxl_modifier)->load(param->module_param,"");
						//printf("pfl_3\n");
						driver->interpolation_list = &interpolation_list;
						driver->comp = (void*)dest;
						//printf("pfl_4\n");
						driver->run();
						//printf("pfl_5\n");
						// notify the client!
						cmd_out->add_raw("vsxl_pfi_ok "+c->parts[1]+" "+c->parts[2]);
					} else {
						//printf("already driver\n");
						driver = (vsx_param_vsxl_driver_abs*)((vsx_param_vsxl*)param->module_param->vsxl_modifier)->get_driver();
					}
					//printf("pfl_6\n");
					if (driver)
					cmd_out->add_raw("vsxl_pfl_s "+c->parts[1]+" "+c->parts[2]+" "+base64_encode(driver->script));
				}
			}
			else
		#endif // NO CLIENT
    // init vsxl filter, run from macros
    if (cmd == "vsxl_pfi") {
      vsx_comp* dest = get_by_name(c->parts[1]);
      if (dest) {
        	//printf("b %d\n",c->parts.size());
        vsx_engine_param* param = dest->get_params_in()->get_by_name(c->parts[2]);
        vsx_param_vsxl_driver_abs* driver;
        if (!param->module_param->vsxl_modifier) {
          //printf("no vsxl_modifier\n");
          param->module_param->vsxl_modifier = (vsx_param_vsxl_abs*)(new vsx_param_vsxl());
          ((vsx_param_vsxl*)(param->module_param->vsxl_modifier))->engine = this;
          driver = (vsx_param_vsxl_driver_abs*)((vsx_param_vsxl*)param->module_param->vsxl_modifier)->load(param->module_param,base64_decode(c->parts[4]),s2i(c->parts[3]));
//        	if (s2i(c->parts[3]) != -1)
        	//driver->id = s2i(c->parts[3]);

					#ifndef VSX_NO_CLIENT
          driver->interpolation_list = &interpolation_list;
					#endif
          driver->comp = (void*)dest;
          driver->run();
					// send status to client
					#ifndef VSX_NO_CLIENT
          cmd_out->add_raw("vsxl_pfi_ok "+c->parts[1]+" "+c->parts[2]);
					#endif
        } else
        	driver = (vsx_param_vsxl_driver_abs*)((vsx_param_vsxl*)param->module_param->vsxl_modifier)->load(param->module_param,base64_decode(c->parts[4]));
        driver->run();
      }
    }
else
#ifndef VSX_NO_CLIENT
    // remove vsxl filter from the engine
    if (cmd == "vsxl_pfr") {
      vsx_comp* dest = get_by_name(c->parts[1]);
      if (dest) {
        vsx_engine_param* param = dest->get_params_in()->get_by_name(c->parts[2]);
        //vsx_param_vsxl_driver_abs* driver;
        if (param->module_param->vsxl_modifier) {
          ((vsx_param_vsxl*)param->module_param->vsxl_modifier)->unload();
          delete (vsx_param_vsxl_abs*)(param->module_param->vsxl_modifier);
          param->module_param->vsxl_modifier = 0;
          // send status to client
          cmd_out->add_raw("vsxl_pfr_ok "+c->parts[1]+" "+c->parts[2]);
        }
      }
    } else
#endif   // no CLIENT

    //if (cmd == "stats") {
      /*std::stringstream ts;
      ts << frame_tcount;*/
        //Implementing new conversion functions
      //cmd_out->add("stats_frame_tcount",i2s(frame_tcount)/*ts.str()*/);
    //} else

#endif // NO GM


#endif /* VSX_EM_SCRIPT_H_ */
