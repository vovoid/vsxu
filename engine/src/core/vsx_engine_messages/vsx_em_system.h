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


if (cmd == "get_module_list")
{
  std::vector< vsx_module_info* >* my_module_list = module_list->get_module_list();

  for
  (
    size_t i = 0;
    i < my_module_list->size();
    i++
  )
  {
    if
    (
        (*my_module_list)[i]->identifier != "outputs;screen"
    )
    {
      cmd_out->add_raw(
            vsx_string("module_list ")
            +
            (*my_module_list)[i]->component_class
            +
            " "
            +
            (*my_module_list)[i]->identifier
            +
            " "
            +
            base64_encode(
              (*my_module_list)[i]->description
              +
              " "
            )
      );
    }
  }
  cmd_out->add_raw("module_list_end");
  delete my_module_list;
  goto process_message_queue_end;
}





if (cmd == "get_list")
{
  std::list<vsx_string> file_list;
  vsx_string path;
  vsx_string base_path = vsx_data_path::get_instance()->data_path_get();

  if (c->parts[1] == "resources")
    path = base_path+c->parts[1];

  if (c->parts[1] == "states" || c->parts[1] == "prods" || c->parts[1] == "visuals")
    path = base_path+c->parts[1];

  get_files_recursive(path, &file_list, "", ".hidden");

  for (std::list<vsx_string>::iterator it = file_list.begin(); it != file_list.end(); ++it)
  {
    vsx_string s2 = str_replace(str_replace("/",";",path)+";","",str_replace(" ",":20:",str_replace("/",";",str_replace(path,"",*it))));
    cmd_out->add_raw(c->parts[1]+"_list "+s2);
  }

  cmd_out->add_raw(c->parts[1]+"_list_end");
  goto process_message_queue_end;
}




if (cmd == "get_state")
{
  send_state_to_client(cmd_out);
  goto process_message_queue_end;
}





if (cmd == "undo_s")
{
  vsx_command_list savelist;
  get_state_as_commandlist(savelist);
  undo_buffer.push_back(savelist);
  goto process_message_queue_end;
}




if (cmd == "undo")
{
  vsx_string error_string;
  if (undo_buffer.size())
  {
    i_load_state(undo_buffer[undo_buffer.size()-1],&error_string);
    undo_buffer.reset_used(undo_buffer.size()-1);
  }
  goto process_message_queue_end;
}





if (cmd == "system.shutdown")
{
  stop();
  exit(0);
}




if (cmd == "kwok")
{
  cmd_out->add_raw("o< KWAK");
  goto process_message_queue_end;
}

if (cmd == "hallo")
{
  cmd_out->add_raw("WAS?");
  goto process_message_queue_end;
}




