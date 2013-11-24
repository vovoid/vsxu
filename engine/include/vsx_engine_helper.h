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


#ifndef VSX_ENGINE_HELPER_H
#define VSX_ENGINE_HELPER_H

class vsx_engine_helper
{
  vsx_command_list cmd_in;
  vsx_command_list cmd_out;
public:
  vsx_engine* engine;

  vsx_engine_helper(vsx_string state_name, vsx_module_list_abs* module_list)
  {
    // create the new engine
    engine = new vsx_engine(module_list);
    engine->set_no_send_client_time(true);
    engine->start();

    // set the screen to silent mode (no mess with openGL)
    vsx_module_param_int* engine_opengl_silent = (vsx_module_param_int*)engine->get_in_param_by_name("screen0", "opengl_silent");
    engine_opengl_silent->set(1);

    engine->load_state(state_name);
  }

  ~vsx_engine_helper()
  {
    engine->stop();
    delete engine;
  }

  void render(float max_time = 120.0f)
  {
    engine->process_message_queue( &cmd_in, &cmd_out, false, false , max_time);
    cmd_out.clear(true);
    engine->render();
  }
};

#endif // VSX_ENGINE_HELPER_H
