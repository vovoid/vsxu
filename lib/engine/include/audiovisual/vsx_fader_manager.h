#pragma once

#include <audiovisual/vsx_state.h>
#include <audiovisual/vsx_fader.h>
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
  vsx_nw_vector<fader*> faders;
  unsigned long fader_current = 0;

  vsx_texture<> tex_current;
  vsx_texture<> tex_upcoming;
  vsx_texture_buffer_render buf_current;
  vsx_texture_buffer_render buf_upcoming;

  float transition_time = 2.0f;

public:

  void load(vsx_string<> base_path)
  {
    vsx_string_helper::ensure_trailing_dir_separator(base_path);
    std::list< vsx_string<> > fader_file_list;
    vsx::filesystem_helper::get_files_recursive(base_path + "visuals_faders", &fader_file_list,"","");
    for (auto it = fader_file_list.begin(); it != fader_file_list.end(); ++it)
      faders.push_back( new fader(*it) );

    buf_current.init(&tex_current,vsx_gl_state::get_instance()->viewport_get_width(), vsx_gl_state::get_instance()->viewport_get_height(), false, true, false, true, 0);
    buf_upcoming.init(&tex_upcoming, vsx_gl_state::get_instance()->viewport_get_width(), vsx_gl_state::get_instance()->viewport_get_height(), false, true, false, true, 0);
  }

  void mark_change()
  {
    transition_time = 2.0f;
  }

  bool is_transitioning()
  {
    return transition_time > 0.0f && transition_time < 2.0f;
  }

  /**
   * @brief render
   * @param current
   * @param upcoming
   * @return true when rendering throug a fader
   */
  bool render(state*& current, state* upcoming)
  {
    buf_upcoming.begin_capture_to_buffer();
      upcoming->render();
      glColorMask(false, false, false, true);
      glClearColor(0,0,0,1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glColorMask(true, true, true, true);
    buf_upcoming.end_capture_to_buffer();

    if ( upcoming->done_loading() && transition_time > 1.0f )
    {
      transition_time = 1.0f;
      fader_current = rand() % (faders.size());
    }

    if (transition_time <= 0.0)
    {
      current = upcoming;
      transition_time = 2.0f;
      return false;
    }

    // begin capture current
    buf_current.begin_capture_to_buffer();

      current->render();
      glColorMask(false, false, false, true);
      glClearColor(0,0,0,1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glColorMask(true, true, true, true);
    buf_current.end_capture_to_buffer();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    faders[fader_current]->render(tex_current, tex_upcoming, transition_time);

    if (transition_time <= 1.0f)
      transition_time -= vsx::common::time::manager::get()->dt;

    return true;
  }

};

}
}
}
