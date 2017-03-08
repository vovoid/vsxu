#pragma once

#include <audiovisual/vsx_state.h>
#include <vsx_gl_state.h>
#include <time/vsx_time_manager.h>
#include <texture/buffer/vsx_texture_buffer_render.h>

namespace vsx
{
namespace engine
{
namespace audiovisual
{

class fader_manager
{
  vsx_nw_vector<vsx_engine*> faders;
  std::list< vsx_string<> > fader_file_list;
  std::list< vsx_string<> > fader_file_list_iterator;

  vsx_texture<> tex_from;
  vsx_texture_buffer_render buf_from;
  vsx_texture<> tex_to;
  vsx_texture_buffer_render buf_to;

  float transition_time = 2.0f;
  unsigned long fade_id = 0;
  vsx_command_list cmd_in;
  vsx_command_list cmd_out;

public:

  fader_manager()
    :
      cmd_in(false),
      cmd_out(false)
  {}

  void load(vsx_string<> base_path)
  {
    vsx_string_helper::ensure_trailing_dir_separator(base_path);
    vsx::filesystem_helper::get_files_recursive(base_path + "visuals_faders", &fader_file_list,"","");
    for (auto it = fader_file_list.begin(); it != fader_file_list.end(); ++it)
    {
      printf("initializing fader %s\n", (*it).c_str());
      vsx_engine* lvxe = new vsx_engine( vsx_module_list_manager::get()->module_list );
      lvxe->start();
      lvxe->load_state(*it);
      faders.push_back(lvxe);
      fade_id = 0;
    }

    buf_from.init(&tex_from,vsx_gl_state::get_instance()->viewport_get_width(), vsx_gl_state::get_instance()->viewport_get_height(), false, true, false, true, 0);
    buf_to.init(&tex_to, vsx_gl_state::get_instance()->viewport_get_width(), vsx_gl_state::get_instance()->viewport_get_height(), false, true, false, true, 0);
  }

  void mark_change()
  {
    transition_time = 2.0f;
  }

  void update()
  {
  }

  /**
   * @brief render
   * @param current
   * @param upcoming
   * @return true when rendering throug a fader
   */
  bool render(state*& current, state* upcoming)
  {
    buf_to.begin_capture_to_buffer();
      upcoming->render();
      glColorMask(false, false, false, true);
      glClearColor(0,0,0,1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glColorMask(true, true, true, true);
    buf_to.end_capture_to_buffer();

    if ( upcoming->done_loading() && transition_time > 1.0f )
    {
      transition_time = 1.0f;
      fade_id = rand() % (faders.size());
    }

    if (transition_time <= 0.0)
    {
      current = upcoming;
      transition_time = 2.0f;
      return false;
    }

    // begin capture
    buf_from.begin_capture_to_buffer();

    // render
    current->render();
      glColorMask(false, false, false, true);
      glClearColor(0,0,0,1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glColorMask(true, true, true, true);
    buf_from.end_capture_to_buffer();

    vsx_module_param_texture* param_t_a = (vsx_module_param_texture*)faders[fade_id]->get_in_param_by_name("visual_fader", "texture_a_in");
    vsx_module_param_texture* param_t_b = (vsx_module_param_texture*)faders[fade_id]->get_in_param_by_name("visual_fader", "texture_b_in");
    vsx_module_param_float* param_pos = (vsx_module_param_float*)faders[fade_id]->get_in_param_by_name("visual_fader", "fade_pos_in");
    vsx_module_param_float* fade_pos_from_engine = (vsx_module_param_float*)faders[fade_id]->get_in_param_by_name("visual_fader", "fade_pos_from_engine");
    faders[fade_id]->process_message_queue(&cmd_in, &cmd_out);
    cmd_out.clear_normal();
    if (param_t_a && param_t_b && param_pos && fade_pos_from_engine)
    {
      param_t_a->set(&tex_from);
      param_t_b->set(&tex_to);
      fade_pos_from_engine->set(1.0f);
      float t = CLAMP(transition_time, 0.0f, 1.0f);
      param_pos->set(1.0f - t);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      faders[fade_id]->render();
    }

    if (transition_time <= 1.0f)
      transition_time -= vsx::common::time::manager::get()->dt;

    return true;
  }

  bool is_transitioning()
  {
    return transition_time > 0.0f && transition_time < 2.0f;
  }

};

}
}
}
