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

class vsx_statelist
{
private:
  vsx_string<> own_path;
  std::list< vsx_string<> > state_file_list;

  std::list< vsx_string<> > fader_file_list;
  std::list< vsx_string<> > fader_file_list_iterator;

  std::vector<state_info> statelist;
  std::vector<state_info>::iterator state_iter;

  vsx_nw_vector<vsx_engine*> faders;

  // our currently running engine
  vsx_engine* vxe = 0x0;

  // module list shared among all engine instances
  vsx_module_list_abs* module_list = 0x0;

  vsx_command_list *cmd_in = 0x0;
  vsx_command_list *cmd_out = 0x0;
  vsx_texture<> tex_from;
  vsx_texture_buffer_render buf_from;
  vsx_texture<> tex_to;
  vsx_texture_buffer_render buf_to;

  vsx_timer timer;

  vsx_string<> config_dir;
  vsx_string<> visual_path;
  vsx_string<> sound_type;

  float transition_time = 2.0f;
  float message_time = -1.0f;
  vsx_string<> message;
  bool render_first = true;

  vsx_command_list l_cmd_in;
  vsx_command_list l_cmd_out;
  unsigned long fade_id = 0;
  bool randomizer = true;
  float randomizer_time = 0.0f;
  bool transitioning = false;
  float fx_alpha = 0.0f;
  float spd_alpha = 0.0f;

  int first = 2;

  vsx_module_engine_float_array int_freq;
  vsx_module_engine_float_array int_wav;

  // options
  bool option_preload_all = false;

  void preload_engines()
  {
    // go through statelist and load and validate every plugin
    std::vector<state_info> new_statelist;
    for (state_iter = statelist.begin(); state_iter != statelist.end(); state_iter++)
    {
      if (init_current((*state_iter).engine, &(*state_iter)) > 0)
      {
        continue;
      }
      new_statelist.push_back(*state_iter);
      if (option_preload_all == true)
      {
        while ( (*state_iter).engine->get_modules_left_to_load() )
        {
          (*state_iter).engine->process_message_queue( &(*state_iter).cmd_in, cmd_out = &(*state_iter).cmd_out,false, true);
          (*state_iter).engine->render();
        }
      }
    }
    statelist = new_statelist;
  }


  int init_current(vsx_engine *vxe_local, state_info* info)
  {
    if (vxe_local == 0)
    {
      vxe_local = new vsx_engine( module_list );
      vxe_local->set_no_send_client_time(true);
      vxe_local->start();
      (*state_iter).engine = vxe_local;
      printf("loading state: %s\n", (*state_iter).state_name.c_str());
      return vxe_local->load_state((*state_iter).state_name);

    } else
    {
      if (info->need_reload) {
        printf("reloading state\n");
        vxe_local->unload_state();
        vxe_local->load_state(info->state_name);
        info->need_reload = false;
      }

      vxe_local->reset_time();
    }
    return 0;
  }

public:

  vsx_statelist()
      :
        l_cmd_in(false),
        l_cmd_out(false)
  {}

  ~vsx_statelist()
  {
    #ifdef VSXU_DEBUG
    printf("statelist destructor\n");
    #endif
    for (std::vector<state_info>::iterator it = statelist.begin(); it != statelist.end(); ++it)
    {
      if ((*it).engine)
      {
        (*it).engine->stop();
        delete (*it).engine;
      }
    }

    for (size_t i = 0; i < faders.size(); i++)
    {
      #ifdef VSXU_DEBUG
      printf("deleting fader %lu\n", i);
      #endif
      delete faders[i];
    }
  }

  void set_module_list( vsx_module_list_abs* new_module_list)
  {
    module_list = new_module_list;
  }

  vsx_engine* get_vxe() {
    return vxe;
  }
  
  void add_visual_path(vsx_string<>new_visual_path)
  {
    vsx::filesystem_helper::get_files_recursive(new_visual_path, &state_file_list,"","");

    #ifdef VSXU_DEBUG
    printf("getting files recursive: %s\n", (new_visual_path).c_str() );
    #endif

    statelist.clear();
    for (std::list< vsx_string<> >::iterator it = state_file_list.begin(); it != state_file_list.end(); ++it) {
        state_info state;
        state.state_name = *it;
        state.state_name_suffix = state.state_name.substr((int)new_visual_path.size(), (int)(state.state_name.size() - new_visual_path.size()) );
    #ifdef VSXU_DEBUG
        printf("adding state %s\n",(*it).c_str());
    #endif
        statelist.push_back(state);
    }
  }


  // **************************************************************************
  // OPTIONS

  // set_option_preload_all
  //   should all states be loaded on initial frame? default: false
  void set_option_preload_all(bool new_value)
  {
    option_preload_all = new_value;
  }
  // **************************************************************************

  state_info* get_state() {
    return &(*state_iter);
  }
  void start()
  {
    vxe->start();
    vxe->load_state((*state_iter).state_name);
  }

  void stop()
  {
    for (std::vector<state_info>::iterator it = statelist.begin(); it != statelist.end(); ++it)
    {
      if ((*it).engine)
      (*it).engine->stop();
      (*it).need_reload = true;
    }
    if (vxe)
    {
      vxe->unload_state();
      vxe->stop();
    }
  }

  void toggle_randomizer()
  {
    randomizer = !randomizer;
  }


  void set_randomizer(bool status)
  {
    randomizer = status;
  }

  bool get_randomizer_status()
  {
    return randomizer;
  }

  void select_visual (int selection)
  {
      if (0 == statelist.size()) return;
      if ((*state_iter).engine != vxe) return;
      bool change = true;
      int count = 0;
      state_iter = statelist.begin();
      while (change) {
        ++state_iter;
        count++;
        if (count >= selection) change = false;
        if (state_iter == statelist.end()) {
            state_iter = statelist.begin();
            change = false;
        }
      }
      init_current((*state_iter).engine, &(*state_iter));
      transition_time = 2.0f;
  }

  
  void random_state()
  {
    if (0 == statelist.size()) return;
    if ((*state_iter).engine != vxe) return;
    int steps = rand() % statelist.size();
    while (steps) {
      ++state_iter;
      if (state_iter == statelist.end()) state_iter = statelist.begin();
      --steps;
    }
    if ((*state_iter).engine == vxe) { random_state(); return; }
    init_current((*state_iter).engine, &(*state_iter));
    transition_time = 2.0f;
  }

  void next_state()
  {
    if ((*state_iter).engine != vxe) return;
    ++state_iter;
    if (state_iter == statelist.end()) state_iter = statelist.begin();
    init_current((*state_iter).engine, &(*state_iter));
    transition_time = 2.0f;
  }

  void prev_state()
  {
    if ((*state_iter).engine != vxe) return;
    if (state_iter == statelist.begin()) state_iter = statelist.end();
    --state_iter;
    init_current((*state_iter).engine, &(*state_iter));
    transition_time = 2.0f;
  }


  std::list< vsx_string<> >* get_state_file_list()
  {
    return &state_file_list;
  }

  std::list< vsx_string<> >* get_fader_file_list()
  {
    return &fader_file_list;
  }

  

  vsx_string<>state_loading()
  {
    if (transition_time > 0.0f && transition_time < 2.0f) {
      return (*state_iter).state_name;
    }
    return "";
  }  
  
  void inc_speed()
  {
    (*state_iter).speed *= 1.04f;
    if ((*state_iter).speed > 16.0f) (*state_iter).speed = 16.0f;
    vxe->set_speed((*state_iter).speed);
  }

  void dec_speed()
  {
    (*state_iter).speed *= 0.96f;
    if ((*state_iter).speed < 0.0f) (*state_iter).speed = 0.0f;
    vxe->set_speed((*state_iter).speed);
  }

  float get_speed()
  {
    return (*state_iter).speed;
  }

  void inc_fx_level()
  {
    (*state_iter).fx_level+=0.05f;
    if ((*state_iter).fx_level > 16.0f) (*state_iter).fx_level = 16.0f;
  #if defined(__linux__)
    vsx_string<>fxlf = config_dir+"/"+(*state_iter).state_name_suffix.substr(visual_path.size()+1 , (*state_iter).state_name_suffix.size())+"_fx_level";
  #ifdef VSXU_DEBUG
    printf("fx level file: %s\n", fxlf.c_str() );
  #endif
    FILE* fxfp = fopen( fxlf.c_str(), "w");
    if (fxfp)
    {
      fputs(vsx_string_helper::f2s((*state_iter).fx_level).c_str(), fxfp);
      fclose(fxfp);
    }
  #endif
    vxe->set_amp((*state_iter).fx_level);
    fx_alpha = 5.0f;
  }

  void dec_fx_level()
  {
    (*state_iter).fx_level-=0.05f;
    if ((*state_iter).fx_level < 0.1f) (*state_iter).fx_level = 0.1f;
    vxe->set_amp((*state_iter).fx_level);
  #if defined(__linux__)
    vsx_string<>fxlf = config_dir+"/"+(*state_iter).state_name_suffix.substr(visual_path.size()+1 , (*state_iter).state_name_suffix.size())+"_fx_level";
    FILE* fxfp = fopen( fxlf.c_str(), "w");
    if (fxfp)
    {
      fputs(vsx_string_helper::f2s((*state_iter).fx_level).c_str(), fxfp);
      fclose(fxfp);
    }
  #endif
    fx_alpha = 5.0f;
  }

  float get_fx_level()
  {
    return (*state_iter).fx_level;
  }

  void toggle_fullscreen();

  void load_faders()
  {
    vsx::filesystem_helper::get_files_recursive(own_path+"visuals_faders", &fader_file_list,"","");
    for (std::list< vsx_string<> >::iterator it = fader_file_list.begin(); it != fader_file_list.end(); ++it)
    {
      printf("initializing fader %s\n", (*it).c_str());
      vsx_engine* lvxe = new vsx_engine( module_list );
      lvxe->start();
      lvxe->load_state(*it);
      faders.push_back(lvxe);
      fade_id = 0;
    }
  }

  void handle_render_first()
  {
    req(render_first);
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    buf_from.init(&tex_from,viewport[2], viewport[3], false, true, false, true, 0);
    buf_to.init(&tex_to, viewport[2], viewport[3], false, true, false, true, 0);
    load_faders();
    transitioning = false;
    render_first = false;
    if ( state_iter == statelist.end() ) return;

    // mark all state_info instances volatile
    for (state_iter = statelist.begin(); state_iter != statelist.end(); state_iter++)
    {
      (*state_iter).is_volatile = true;
    }

    preload_engines();

    // mark all state_info instances non-volatile (engine will be deleted when state_iter will be deleted)
    for (state_iter = statelist.begin(); state_iter != statelist.end(); state_iter++)
    {
      (*state_iter).is_volatile = true;
    }

    // reset state_iter to a random state
    state_iter = statelist.begin();
    int steps = rand() % statelist.size();
    while (steps) {
      ++state_iter;
      if (state_iter == statelist.end()) state_iter = statelist.begin();
      --steps;
    }

    vxe = (*state_iter).engine;
    cmd_in = &(*state_iter).cmd_in;
    cmd_out = &(*state_iter).cmd_out;
  }

  bool render_change()
  {
    reqrv((*state_iter).engine != vxe, false); // change is on the way

    buf_to.begin_capture_to_buffer();
      if ((*state_iter).engine)
      {
        (*state_iter).engine->process_message_queue(&(*state_iter).cmd_in,&(*state_iter).cmd_out);
        (*state_iter).engine->render();
      }
      glColorMask(false, false, false, true);
      glClearColor(0,0,0,1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glColorMask(true, true, true, true);
    buf_to.end_capture_to_buffer();
    if (
      (*state_iter).engine->get_modules_left_to_load() == 0 &&
      (*state_iter).engine->get_commands_internal_count() &&
      transition_time > 1.0f
    )
    {
      transition_time = 1.0f;
      timer.start();
      fade_id = rand() % (faders.size());
    }

    if (transition_time <= 0.0)
    {
      vxe = (*state_iter).engine;
      cmd_in = &(*state_iter).cmd_in;
      cmd_out = &(*state_iter).cmd_out;
      transitioning = false;
      transition_time = 2.0f;
      if (cmd_out && cmd_in)
      {
        if (vxe)
          vxe->process_message_queue(cmd_in,cmd_out);
        cmd_out->clear_normal();
      }

      if (vxe)
        vxe->render();

      return true;
    }

    if (cmd_out && cmd_in)
    {
      if (vxe)
        vxe->process_message_queue(cmd_in,cmd_out);
      cmd_out->clear_normal();
    }

    // begin capture
    if (vsx_texture_buffer_base::has_buffer_support())
      buf_from.begin_capture_to_buffer();

    // render
    if (vxe)
      vxe->render();

    glColorMask(false, false, false, true);
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColorMask(true, true, true, true);

    // end capture and send to fader
    buf_from.end_capture_to_buffer();
    vsx_module_param_texture* param_t_a = (vsx_module_param_texture*)faders[fade_id]->get_in_param_by_name("visual_fader", "texture_a_in");
    vsx_module_param_texture* param_t_b = (vsx_module_param_texture*)faders[fade_id]->get_in_param_by_name("visual_fader", "texture_b_in");
    vsx_module_param_float* param_pos = (vsx_module_param_float*)faders[fade_id]->get_in_param_by_name("visual_fader", "fade_pos_in");
    vsx_module_param_float* fade_pos_from_engine = (vsx_module_param_float*)faders[fade_id]->get_in_param_by_name("visual_fader", "fade_pos_from_engine");
    faders[fade_id]->process_message_queue(&l_cmd_in, &l_cmd_out);
    l_cmd_out.clear_normal();
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
      transition_time -= (float)timer.dtime();
    return true;
  }

  void update_randomizer()
  {
    req(randomizer);
    randomizer_time -= vxe->get_engine_info()->real_dtime;
    if (randomizer_time < 0.0f)
    {
      random_state();
      randomizer_time = (float)(rand()%1000)*0.001f*15.0f+10.0f;
    }
  }

  void render()
  {
    handle_render_first();
    update_randomizer();

    req( !statelist.size() );
    req( !render_change());

    if (cmd_out && cmd_in)
    {
      vxe->process_message_queue(cmd_in, cmd_out);
      cmd_out->clear_normal();
    }

    vxe->render();
  }

  void init(vsx_string<> base_path, vsx_string<> init_sound_type)
  {
    randomizer_time = vsx_rand_singleton::get()->rand.frand()*15.0f + 10.0f;
    sound_type = init_sound_type;
    vsx_string_helper::ensure_trailing_dir_separator(base_path);

    own_path = base_path;
    visual_path = "visuals_player";

    vsx::filesystem_helper::get_files_recursive(own_path + visual_path, &state_file_list,"","");
    for (std::list< vsx_string<> >::iterator it = state_file_list.begin(); it != state_file_list.end(); ++it) {
      state_info state;
      state.state_name = *it;
      state.state_name_suffix = state.state_name.substr((int)own_path.size(), (int)(state.state_name.size() - own_path.size()) );
      state.fx_level = 1.0f;
      state.engine = 0;
      statelist.push_back(state);
    }
    state_iter = statelist.begin();
    load_fx_levels_from_user();
  }


  void load_fx_levels_from_user()
  {
  #if PLATFORM == PLATFORM_LINUX
    struct stat st;

    char* home_dir = getenv ("HOME");
    config_dir = home_dir;
    config_dir += "/.vsxu_player";
    if (access(config_dir.c_str(),0) != 0) mkdir(config_dir.c_str(),0700);

    for (std::vector<state_info>::iterator it = statelist.begin(); it != statelist.end(); it++)
    {
      state_info state = (*it);
      // read fx level files
      vsx_string<>fxlf = config_dir+"/"+state.state_name_suffix.substr(visual_path.size()+1 , state.state_name_suffix.size())+"_fx_level";
      if ( stat( fxlf.c_str(), &st) != 0 )
      {
        // no fx level file
        FILE* fpfx = fopen(fxlf.c_str(), "w");
        if (fpfx)
        {
          vsx_string<>ff = "1.0";
          fputs(ff.c_str(), fpfx);
          fclose(fpfx);
        }
        state.fx_level = 1.0f;
      } else
      {
        // got the file
        FILE* fpfx = fopen(fxlf.c_str(), "r");
        char dest[256];
        char* cc = fgets(dest, 256, fpfx);
        (void)cc;
        fclose(fpfx);
        vsx_string<>ff = dest;
        state.fx_level = vsx_string_helper::s2f(ff);
      }
    }
  #endif
  }

  void save_fx_levels_from_user()
  {
  #if PLATFORM == PLATFORM_LINUX
    struct stat st;

    char* home_dir = getenv ("HOME");
    config_dir = home_dir;
    config_dir += "/.vsxu_player";
    if (stat(config_dir.c_str(),&st) != 0)
    mkdir(config_dir.c_str(),0700);

    for (std::vector<state_info>::iterator it = statelist.begin(); it != statelist.end(); it++)
    {
      state_info state = (*it);
      // read fx level files
      vsx_string<>fxlf = config_dir+"/"+state.state_name_suffix.substr(visual_path.size()+1 , state.state_name_suffix.size())+"_fx_level";
      if ( stat( fxlf.c_str(), &st) != 0 )
      {
        // no fx level file
        FILE* fpfx = fopen(fxlf.c_str(), "w");
        if (fpfx)
        {
          vsx_string<>ff = "1.0";
          fputs(ff.c_str(), fpfx);
          fclose(fpfx);
        }
        state.fx_level = 1.0f;
      } else
      {
        // got the file
        FILE* fpfx = fopen(fxlf.c_str(), "r");
        char dest[256];
        char* cc = fgets(dest, 256, fpfx);
        (void)cc;
        fclose(fpfx);
        vsx_string<>ff = dest;
        state.fx_level = vsx_string_helper::s2f(ff);
      }
    }
  #endif
  }


  void set_sound_freq(float* data)
  {
    if (!vxe) return;
    //int_freq.array[511] = 0.0f;
    //float* dp = int_freq.array.get_pointer();
    for (unsigned long i = 0; i < 513; i++)
    {
      int_freq.array[i] = data[i];
    }
    vxe->set_float_array_param(1, &int_freq);
  }


  void set_sound_wave(float* data)
  {
    if (!vxe) return;
    //int_wav.array[511] = 0.0f;
    //float* dp = int_wav.array.get_pointer();
    for (unsigned long i = 0; i < 513; i++)
    {
      int_wav.array[i] = data[i];
    }
    vxe->set_float_array_param(0, &int_wav);
  }

  
  vsx_string<> get_meta_visual_filename()
  {
      return (*state_iter).state_name;
  }

  vsx_string<> get_meta_visual_name()
  {
      if((*state_iter).engine)
          return (*state_iter).engine->get_meta_information(0);
      return vsx_string<>();
  }

  vsx_string<> get_meta_visual_creator()
  {
      if((*state_iter).engine)
          return (*state_iter).engine->get_meta_information(1);
      return vsx_string<>();
  }

  vsx_string<> get_meta_visual_company()
  {
      if((*state_iter).engine)
          return (*state_iter).engine->get_meta_information(2);
      return vsx_string<>();
  }
};

