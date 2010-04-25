/*
 * vsx_em_macro.h
 *
 *  Created on: Apr 16, 2009
 *      Author: jaw
 */

#ifndef VSX_EM_MACRO_H_
#define VSX_EM_MACRO_H_

	#ifndef VSX_NO_CLIENT
    if (cmd == "macro_dump" || cmd == "component_clone") {
      // syntaX:
      //   macro_dump [name] [save_name]
      // - the most ultimate princess training ever, nyo!
      // first mission: find puchiko
      if (c->parts.size() >= 3) {
        vsx_string my_name = c->parts[1];
        forge_map_iter = forge_map.find(c->parts[1]);
        if (forge_map_iter != forge_map.end())
        {
          // puchiko has been found, nyo!
          vsx_command_list tmp_comp;
          vsx_command_list tmp_param_set;
          vsx_command_list tmp_connections;
          vsx_command_list tmp_aliases;

          bool drun = true;
          bool macro = ((*forge_map_iter).second->component_class == "macro");
          if (macro) {
            tmp_comp.add_raw(vsx_string("macro_create $$name ")+f2s((*forge_map_iter).second->size));
            ++forge_map_iter;
            while (drun) {
              if (forge_map_iter != forge_map.end()) {
                vsx_string t = (*forge_map_iter).first;
                vsx_comp* comp = (*forge_map_iter).second;
                if (t.find(c->parts[1]+".") == 0)
                {
                  if (comp->component_class == "macro") {
                    tmp_comp.add_raw(vsx_string("macro_create ")+str_replace(my_name+".","$$name.",t,1)+" "+f2s(comp->position.x)+" "+f2s(comp->position.y)+" "+f2s(comp->size));
                  } else {
                    tmp_comp.add_raw(vsx_string("component_create ")+comp->identifier+" "+str_replace(my_name+".","$$name.",t,1)+" "+f2s(comp->position.x)+" "+f2s(comp->position.y));
                    comp->get_params_in()->dump_aliases_and_connections(c->parts[1], &tmp_connections);
                    comp->get_params_out()->dump_aliases(c->parts[1], &tmp_aliases);
                    comp->get_params_in()->dump_param_values(str_replace(my_name+".","$$name.",t,1),&tmp_comp);
                  }
                } else drun = false;
                ++forge_map_iter;
              } else drun = false;
            }
          } else {
            vsx_comp* comp = (*forge_map_iter).second;
            tmp_comp.add_raw(vsx_string("component_create ")+comp->identifier+" $$name "+f2s(comp->position.x)+" "+f2s(comp->position.y));
            comp->get_params_in()->dump_param_values("$$name",&tmp_comp);
          }
          vsx_command_s* outc;
          tmp_comp.reset();
          while ( (outc = tmp_comp.get()) ) {
            cmd_out->add_raw(vsx_string(c->parts[0]+"_add ")+c->get_parts(1,2)+" "+base64_encode(outc->raw));
          }
          if (tmp_aliases.count()) {
            tmp_aliases.reset();
            while ( (outc = tmp_aliases.pop_back()) ) {
              cmd_out->add_raw(vsx_string(c->parts[0]+"_add ")+c->get_parts(1,2)+" "+base64_encode(outc->raw));
            }
          }
          if (tmp_connections.count()) {
            tmp_connections.reset();
            while ( (outc = tmp_connections.pop_back()) ) {
              cmd_out->add_raw(vsx_string(c->parts[0]+"_add ")+c->get_parts(1,2)+" "+base64_encode(outc->raw));
            }
          }
          cmd_out->add_raw(vsx_string(c->parts[0]+"_complete ")+c->get_parts(1));//+" "+c->parts[2]);
        }
      }
    } else
    if (cmd == "macro_prerun") {
      if (get_by_name(c->parts[3])) {
        cmd_out->add_raw(vsx_string("alert_fail ")+base64_encode(c->raw)+" Error "+base64_encode("There is already a macro '"+c->parts[3]+"'"));
      } else {
        cmd_out->addc(c);
      }
    } else
		#endif
    if (cmd == "macro_create") {
      if (c->parts.size() == 5) {
        // syntax:
        //  macro_create macro1 [pos_x] [pos_y] [size]
        if (!get_by_name(c->parts[1])) {
          vsx_comp* comp = add(c->parts[1]);
          // ok we force this to boo macrooo
          comp->component_class = "macro";
					#ifndef VSX_NO_CLIENT
          comp->position.x = s2f(c->parts[2]);
          comp->position.y = s2f(c->parts[3]);
          comp->size = s2f(c->parts[4]);
          // the code creating the macro seems pretty similar to that of the component eh?
          cmd_out->add_raw(vsx_string("component_create_ok ")+c->parts[1]+" "+get_by_name(c->parts[1])->component_class+" "+c->parts[2]+" "+c->parts[3]+" "+c->parts[4]);
					#endif
//          printf("macro_done\n");
        }
				#ifndef VSX_NO_CLIENT
        else
        {
          cmd_out->add_raw("alert_fail "+base64_encode(c->raw)+" Error "+base64_encode("There is already a macro '"+c->parts[1]));
        }
				#endif
      }
    } else

#endif /* VSX_EM_MACRO_H_ */
