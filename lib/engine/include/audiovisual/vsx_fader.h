#pragma once

#include <vsx_engine.h>

namespace vsx
{
namespace engine
{
namespace audiovisual
{

class fader
{
  vsx_engine* engine;

  vsx_command_list cmd_in;
  vsx_command_list cmd_out;

public:

  fader(vsx_string<> filename)
    :
      cmd_in(false),
      cmd_out(false)
  {
    engine = new vsx_engine( vsx_module_list_manager::get()->module_list );
    engine->start();
    engine->load_state( filename );
  }

  void render(vsx_texture<>& tex_current, vsx_texture<>& tex_upcoming, float transition_time)
  {
    vsx_module_param_texture* param_t_a = (vsx_module_param_texture*)engine->get_in_param_by_name("visual_fader", "texture_a_in");
    vsx_module_param_texture* param_t_b = (vsx_module_param_texture*)engine->get_in_param_by_name("visual_fader", "texture_b_in");
    vsx_module_param_float* param_pos = (vsx_module_param_float*)engine->get_in_param_by_name("visual_fader", "fade_pos_in");
    vsx_module_param_float* fade_pos_from_engine = (vsx_module_param_float*)engine->get_in_param_by_name("visual_fader", "fade_pos_from_engine");
    engine->process_message_queue(&cmd_in, &cmd_out);
    cmd_out.clear_normal();

    req(param_t_a);
    req(param_t_b);
    req(param_pos);
    req(fade_pos_from_engine);

    param_t_a->set(&tex_current);
    param_t_b->set(&tex_upcoming);

    fade_pos_from_engine->set(1.0f);

    float t = CLAMP(transition_time, 0.0f, 1.0f);
    param_pos->set(1.0f - t);

    engine->render();
  }

};

}
}
}
