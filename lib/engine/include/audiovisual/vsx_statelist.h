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

#pragma once

#include <vsx_platform.h>
#include <vsx_argvector.h>

#if PLATFORM == PLATFORM_LINUX
  #include <stdio.h>
  #include <stdlib.h>
  #include <sys/stat.h>
  #include <unistd.h>
#endif

#include <GL/glew.h>
#include "vsx_engine.h"
#include <texture/buffer/vsx_texture_buffer_render.h>
#include "vsx_state_info.h"
#include <math/vsx_rand_singleton.h>
#include <time/vsx_time_manager.h>
#include <audiovisual/vsx_pack_manager.h>
#include <audiovisual/vsx_fader_manager.h>

class vsx_statelist
{
private:
  vsx::engine::audiovisual::fader_manager faders;

  std::vector<state_info*> statelist;
  std::vector<state_info*>::iterator state_iter;

  // our currently running state
  state_info* current_state = 0x0;

  // module list shared among all engine instances
  vsx_module_list_abs* module_list = 0x0;

  vsx_string<> config_dir;
  vsx_string<> visual_path;

  float message_time = -1.0f;
  vsx_string<> message;
  bool render_first = true;

  bool randomizer = false;
  float randomizer_time = 0.0f;

  vsx_module_engine_float_array int_freq;
  vsx_module_engine_float_array int_wav;

  // options
  bool option_preload_all = false;

  void init_states()
  {
    // go through statelist and load and validate every plugin
    std::vector<state_info*> new_statelist;
    for (state_iter = statelist.begin(); state_iter != statelist.end(); state_iter++)
    {
      // 0 = successfully loaded
      req_continue(!(*state_iter)->init());

      // state is OK, add to state list
      new_statelist.push_back(*state_iter);

      if (option_preload_all == true)
        while ( !(*state_iter)->done_loading() )
          (*state_iter)->render();
    }
    statelist = new_statelist;
  }

public:

  ~vsx_statelist()
  {
    for (auto it = statelist.begin(); it != statelist.end(); ++it)
      delete *it;
  }

  void set_module_list( vsx_module_list_abs* new_module_list)
  {
    module_list = new_module_list;
  }
  
  void set_option_preload_all(bool new_value)
  {
    option_preload_all = new_value;
  }

  void start()
  {
    current_state->start();
  }

  void stop()
  {
    for (auto it = statelist.begin(); it != statelist.end(); ++it)
      (*it)->stop();
  }

  void toggle_randomizer() {
    randomizer = !randomizer;
  }

  void set_randomizer(bool status) {
    randomizer = status;
  }

  bool get_randomizer_status() {
    return randomizer;
  }

  void select_visual (int selection)
  {
    req(statelist.size());
    req(*state_iter == current_state);

    bool change = true;
    int count = 0;
    state_iter = statelist.begin();
    while (change)
    {
      ++state_iter;
      count++;
      if (count >= selection)
        change = false;

      if (state_iter == statelist.end())
      {
          state_iter = statelist.begin();
          change = false;
      }
    }
    (*state_iter)->init();
    faders.mark_change();
  }

  
  void random_state()
  {
    req(statelist.size());
    req(*state_iter == current_state);

    int steps = rand() % statelist.size();
    while (steps) {
      ++state_iter;
      if (state_iter == statelist.end())
        state_iter = statelist.begin();
      --steps;
    }

    if ((*state_iter) == current_state)
    {
      random_state();
      return;
    }

    (*state_iter)->init();
    faders.mark_change();
  }

  void next_state()
  {
    req(statelist.size());
    req(*state_iter == current_state);

    if (state_iter == statelist.end())
      state_iter = statelist.begin();
    ++state_iter;

    (*state_iter)->init();
    faders.mark_change();
  }

  void prev_state()
  {
    req(statelist.size());
    req(*state_iter == current_state);
    if (state_iter == statelist.begin())
      state_iter = statelist.end();
    --state_iter;
    (*state_iter)->init();
    faders.mark_change();
  }

  vsx_string<> state_loading()
  {
    reqrv(faders.is_transitioning(), "";)
    return (*state_iter)->name;
  }  
  
  void speed_inc()
  {
    (*state_iter)->adjust_speed(1.04f);
  }

  void speed_dec()
  {
    (*state_iter)->adjust_speed(0.96f);
  }

  float speed_get()
  {
    return (*state_iter)->speed;
  }

  void fx_level_inc()
  {
    (*state_iter)->adjust_fx_level(0.05f);
  }

  void fx_level_dec()
  {
    (*state_iter)->adjust_fx_level( -0.05f);
  }

  float fx_level_get()
  {
    return (*state_iter)->fx_level;
  }

  void handle_render_first()
  {
    req(render_first);
    render_first = false;

    if ( state_iter == statelist.end() )
      return;

    init_states();

    // reset state_iter to a random state
    state_iter = statelist.begin();
    int steps = vsx_rand_singleton::get()->rand.rand() % statelist.size();
    while (steps) {
      ++state_iter;
      if (state_iter == statelist.end())
        state_iter = statelist.begin();
      --steps;
    }

    current_state = *state_iter;
  }

  bool render_change()
  {
    reqrv( *state_iter != current_state, false); // change is on the way
    return faders.render( current_state, *state_iter );
  }

  void update_randomizer()
  {
    req(randomizer);
    randomizer_time -= vsx::common::time::manager::get()->dt;
    if (randomizer_time < 0.0f)
    {
      random_state();
      randomizer_time = (float)(rand()%1000)*0.001f*15.0f+10.0f;
    }
  }

  void render()
  {
    req( statelist.size() );
    handle_render_first();
    update_randomizer();
    req( !render_change());
    current_state->render();
  }

  void load(vsx_string<> base_path)
  {
    randomizer_time = vsx_rand_singleton::get()->rand.frand()*15.0f + 10.0f;
    vsx_string_helper::ensure_trailing_dir_separator(base_path);
    faders.load(base_path);

    visual_path = "visuals_player";

    std::list< vsx_string<> > state_file_list;
    vsx::filesystem_helper::get_files_recursive(base_path + visual_path, &state_file_list,"","");
    for (auto it = state_file_list.begin(); it != state_file_list.end(); ++it) {
      state_info* state = new state_info();
      state->filename = *it;
      statelist.push_back(state);
    }
    state_iter = statelist.begin();
  }

  void set_sound_freq(float* data)
  {
    req(current_state);
    for (unsigned long i = 0; i < 513; i++)
      int_freq.array[i] = data[i];
    current_state->set_float_array_param(1, int_freq);
  }

  void set_sound_wave(float* data)
  {
    req(current_state);
    for (unsigned long i = 0; i < 513; i++)
      int_wav.array[i] = data[i];
    current_state->set_float_array_param(0, int_wav);
  }
  
  vsx_string<> get_meta_visual_filename()
  {
    return (*state_iter)->filename;
  }

  vsx_string<> get_meta_visual_name()
  {
    // TODO: handle packs here
    reqrv((*state_iter)->engine, "");
    return (*state_iter)->engine->get_meta_information(0);
  }

  vsx_string<> get_meta_visual_creator()
  {
    reqrv((*state_iter)->engine, "");
    return (*state_iter)->engine->get_meta_information(1);
  }

  vsx_string<> get_meta_visual_company()
  {
    reqrv((*state_iter)->engine, "");
    return (*state_iter)->engine->get_meta_information(2);
  }

  size_t get_meta_modules_in_engine()
  {
    reqrv(current_state->engine,0);
    return current_state->engine->get_num_modules();
  }
};
