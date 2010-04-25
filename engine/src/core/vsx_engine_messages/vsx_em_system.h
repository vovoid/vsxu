/*
 * vsx_em_system.h
 *
 *  Created on: Apr 16, 2009
 *      Author: jaw
 */

#ifndef VSX_EM_SYSTEM_H_
#define VSX_EM_SYSTEM_H_

#ifndef VSX_NO_CLIENT
    if (cmd == "get_module_list") {
      for (module_iter = module_list.begin(); module_iter != module_list.end(); ++module_iter) {
        if ((*module_iter).first != "outputs;screen")
        if (!module_dll_list[(*module_iter).first]->hidden_from_gui)
        cmd_out->add_raw(vsx_string("module_list ")+(*module_iter).second->component_class+" "+(*module_iter).first+" "+base64_encode((*module_iter).second->description+" "));
      }
      cmd_out->add_raw("module_list_end");
    }
    else
    if (cmd == "get_list") {
      std::list<vsx_string> mfiles;
      vsx_string path;
      vsx_string base_path = vsx_get_data_path();
      if (c->parts[1] == "resources") path = base_path+c->parts[1];
      if (c->parts[1] == "states" || c->parts[1] == "prods" || c->parts[1] == "visuals") path = base_path+c->parts[1];
      get_files_recursive(path,&mfiles,"",".hidden");
      for (std::list<vsx_string>::iterator it = mfiles.begin(); it != mfiles.end(); ++it) {
        //printf("internal file: %s\n",(*it).c_str());
        //vsx_string s2 = str_replace("/",";",*it);
        //vsx_string s3 = str_replace("resources;","","resources;foo;bar");
        vsx_string s2 = str_replace(str_replace("/",";",path)+";","",str_replace(" ",":20:",str_replace("/",";",*it)));
        //printf("s2: %s\n",s2.c_str());
        cmd_out->add_raw(c->parts[1]+"_list "+s2);
      }
      cmd_out->add_raw(c->parts[1]+"_list_end");
    } else
    if (cmd == "get_state") {
      send_state_to_client(cmd_out);
    } else
		#ifndef VSX_NO_CLIENT
    if (cmd == "undo_s") {
        vsx_command_list savelist;
        get_state_as_commandlist(savelist);
        undo_buffer.push_back(savelist);
    } else
    if (cmd == "undo") {
      //printf("undo in engine\n");
      vsx_string error_string;
      if (undo_buffer.size()) {
        i_load_state(undo_buffer[undo_buffer.size()-1],&error_string);
        undo_buffer.reset_used(undo_buffer.size()-1);
      }
    } else
#endif
    if (cmd == "system.shutdown") {
      stop();
      exit(0);
    } else
    if (cmd == "kwok") {
      cmd_out->add_raw("o< KWAK");
    } else

    if (cmd == "hallo") {
      cmd_out->add_raw("WAS?");
    } else
#endif // NO CLIENT
    /* else
    if (cmd == "help") {
cmd_out->add(
"VSXU Command Syntax:\n","\n\
   q, quit {disconnect from server}\n\
   dn, shutdown {shut down server}\n\
   stats {get statictics dump from gfx/dsp engine}\n\
   fps_d  {gets the current framerate}\n\
   fps_d_set [frames] {sets maximum frames per interval for framerate\n\
                       calculations}\n\
   fullscreen [mode-str] {GLUT GameMode string, for instance 800x600:32@85\n\
                          To return to window mode, just go \"fullscreen\"}\n\
___________________________________________________________________________\n"
);
		} else*/
    {
#ifndef VSX_NO_CLIENT
		  cmd_out->add("invalid","command");
#endif
		}
		#

#endif /* VSX_EM_SYSTEM_H_ */
