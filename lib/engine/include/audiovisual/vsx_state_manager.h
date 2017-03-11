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
#include <math/vsx_rand_singleton.h>
#include <time/vsx_time_manager.h>
#include <tools/vsx_managed_singleton.h>

#include <audiovisual/vsx_state.h>
#include <audiovisual/vsx_pack_manager.h>
#include <audiovisual/vsx_fader_manager.h>

namespace vsx
{
namespace engine
{
namespace audiovisual
{

class state_manager
  : public vsx::managed_singleton<state_manager>
{
private:
  fader_manager faders;
  pack_manager packs;

  std::vector<state*> states;
  std::vector<state*>::iterator states_iter;

  // currently active state
  state* state_current = 0x0;

  vsx_string<> config_dir;
  vsx_string<> visual_path;

  float message_time = -1.0f;
  vsx_string<> message;
  bool render_first = true;

  float auto_change_time = 0.0f;

  bool randomizer = true;
  bool sequential = false;

  vsx_module_engine_float_array int_freq;
  vsx_module_engine_float_array int_wav;

  void init_states()
  {
    // go through states and exclude broken visuals
    std::vector<state*> new_statelist;
    for (states_iter = states.begin(); states_iter != states.end(); states_iter++)
    {
      // 0 = successfully loaded
      req_continue(!(*states_iter)->init());

      // state is OK, add to state list
      new_statelist.push_back(*states_iter);

      // preload where applicable
      if (option_preload_all == true)
        while ( !(*states_iter)->done_loading() )
          (*states_iter)->render();
    }
    states = new_statelist;
  }

public:

  // options
  bool option_preload_all = false;

  // global error message
  vsx_string<> system_message;

  state_manager()
  {
    auto_change_time = vsx_rand_singleton::get()->rand.frand()*15.0f + 10.0f;
  }

  ~state_manager()
  {
    for (auto it = states.begin(); it != states.end(); ++it)
      delete *it;
  }

  std::vector<state*>& states_get()
  {
    return states;
  }

  void start()
  {
    state_current->start();
  }

  void stop()
  {
    for (auto it = states.begin(); it != states.end(); ++it)
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

  void set_sequential(bool status) {
    sequential = status;
  }

  void select_state (int selection)
  {
    req(states.size());
    req(*states_iter == state_current);

    bool change = true;
    int count = 0;
    states_iter = states.begin();
    while (change)
    {
      ++states_iter;
      count++;
      if (count >= selection)
        change = false;

      if (states_iter == states.end())
      {
          states_iter = states.begin();
          change = false;
      }
    }
    (*states_iter)->init();
    faders.mark_change();
  }


  void select_random_state(bool mark_change = true)
  {
    req(states.size());
    req(*states_iter == state_current);

    int steps = rand() % states.size();
    while (steps) {
      ++states_iter;
      if (states_iter == states.end())
        states_iter = states.begin();
      --steps;
    }

    if ((*states_iter) == state_current)
    {
      select_random_state();
      return;
    }

    (*states_iter)->init();
    if (mark_change)
      faders.mark_change();
  }

  void select_next_state()
  {
    req(states.size());
    req(*states_iter == state_current);

    if (states_iter == states.end())
      states_iter = states.begin();
    ++states_iter;

    (*states_iter)->init();
    faders.mark_change();
  }

  void select_prev_state()
  {
    req(states.size());
    req(*states_iter == state_current);
    if (states_iter == states.begin())
      states_iter = states.end();
    --states_iter;
    (*states_iter)->init();
    faders.mark_change();
  }

  vsx_string<> state_loading()
  {
    reqrv(faders.is_transitioning(), "";)
    return (*states_iter)->name;
  }

  void speed_inc()
  {
    (*states_iter)->adjust_speed(1.04f);
  }

  void speed_dec()
  {
    (*states_iter)->adjust_speed(0.96f);
  }

  float speed_get()
  {
    return (*states_iter)->speed;
  }

  void fx_level_inc()
  {
    (*states_iter)->adjust_fx_level(0.05f);
  }

  void fx_level_dec()
  {
    (*states_iter)->adjust_fx_level( -0.05f);
  }

  float fx_level_get()
  {
    return (*states_iter)->fx_level;
  }

  bool update_auto_change_time()
  {
    auto_change_time -= vsx::common::time::manager::get()->dt;
    reqrv(auto_change_time < 0.0f, false);
    auto_change_time = (float)(rand()%1000)*0.001f*15.0f+10.0f;
    return true;
  }

  void update_randomizer()
  {
    req(randomizer);
    req(update_auto_change_time());
    select_random_state();
  }

  void update_sequential()
  {
    req(sequential);
    req(update_auto_change_time());
    select_next_state();
  }

  void handle_render_first()
  {
    req(render_first);
    render_first = false;

    if ( states_iter == states.end() )
      return;

    init_states();

    // reset state_iter
    states_iter = states.begin();
    state_current = *states_iter;

    // select random initial state
    if (randomizer)
    {
      select_random_state(false);
      state_current = *states_iter;
    }
  }

  bool render_change()
  {
    reqrv( *states_iter != state_current, false); // change is on the way
    return faders.render( state_current, *states_iter );
  }

  void render()
  {
    req( states.size() );
    handle_render_first();

    // automatic progression
    update_randomizer();
    update_sequential();

    system_message = "";
    foreach (states, i)
      system_message += states[i]->system_message_get();

    req( !render_change());
    state_current->render();
  }

  void load_individual_visuals(vsx_string<>& base_path, vsx_string<> visual_path = "visuals_player")
  {
    vsx_string_helper::ensure_trailing_dir_separator(base_path);
    std::list< vsx_string<> > state_file_list;
    vsx::filesystem_helper::get_files_recursive(base_path + visual_path, &state_file_list,".vsx","");
    for (auto it = state_file_list.begin(); it != state_file_list.end(); ++it) {
      state* new_state = new state();
      new_state->filename = *it;
      states.push_back(new_state);
    }
  }

  void load(vsx_string<> base_path)
  {
    faders.load(base_path);
    load_individual_visuals(base_path);

    packs.load(base_path);
    packs.add_states(states);

    // reset state iter
    states_iter = states.begin();
  }

  void set_sound_freq(float* data)
  {
    req(state_current);
    for (unsigned long i = 0; i < 513; i++)
      int_freq.array[i] = data[i];
    state_current->set_float_array_param(1, int_freq);
  }

  void set_sound_wave(float* data)
  {
    req(state_current);
    for (unsigned long i = 0; i < 513; i++)
      int_wav.array[i] = data[i];
    state_current->set_float_array_param(0, int_wav);
  }

  vsx_string<> get_meta_visual_filename()
  {
    reqrv(state_current, "");
    return state_current->filename;
  }

  vsx_string<> get_meta_visual_name()
  {
    reqrv(state_current, "");
    return state_current->get_name();
  }

  vsx_string<> get_meta_upcoming_visual_name()
  {
    reqrv(state_current != *states_iter, "");
    return (*states_iter)->get_name();
  }

  vsx_string<> get_meta_visual_author()
  {
    reqrv(state_current, "");
    return state_current->get_author();
  }

  size_t get_meta_modules_in_engine()
  {
    reqrv(state_current->engine,0);
    return state_current->engine->get_num_modules();
  }
};

}
}
}

