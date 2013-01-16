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
    engine = new vsx_engine();
    engine->set_module_list( module_list );
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

  void render()
  {
    engine->process_message_queue( &cmd_in, &cmd_out, false, true );
    cmd_out.clear(true);
    engine->render();
  }
};

#endif // VSX_ENGINE_HELPER_H
