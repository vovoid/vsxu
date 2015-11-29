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

#ifndef VSX_STATELIST_H_
#define VSX_STATELIST_H_

#include <vsx_platform.h>
#if PLATFORM == PLATFORM_LINUX
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
#include <GL/glew.h>

#include "vsx_engine.h"
#include <texture/buffer/vsx_texture_buffer_render.h>

class state_info {
public:
  float fx_level;
  float speed;
  vsx_engine* engine;
  vsx_string<>state_name;
  vsx_string<>state_name_suffix;
  vsx_command_list cmd_in;
  vsx_command_list cmd_out;
  bool need_stop;
  bool need_reload;
  bool is_volatile;

  state_info()
  {
    speed = 1.0f;
    engine = 0;
    need_stop = false;
    need_reload = false;
    is_volatile = false;
  }
  ~state_info()
  {
    if (is_volatile) return;
    if (engine) delete engine;
  }
};

// WARNING! INIT THIS YOURSELF WITH THE NEW OPERATOR SOMEWHERE
// IN THE MAIN() METHOD OTHERWISE ARGC AND ARGV WILL BE VOID!



class vsx_statelist
{
private:
  vsx_string<>own_path;
  std::list< vsx_string<> > state_file_list;
  std::list< vsx_string<> > fader_file_list;
  std::vector<state_info> statelist;
  std::vector<state_info>::iterator state_iter;

  std::vector<vsx_engine*> faders;

  // our currently running engine
  vsx_engine* vxe;

  // module list shared among all engine instances
  vsx_module_list_abs* module_list;

  vsx_command_list *cmd_in;
  vsx_command_list *cmd_out;
  vsx_texture<> tex1;
  vsx_texture_buffer_render buf1;
  vsx_texture<> tex_to;
  vsx_texture_buffer_render buf_to;

  vsx_timer timer;

  vsx_string<>config_dir;
  vsx_string<>visual_path;
  vsx_string<>sound_type;

  int init_current(vsx_engine *vxe_local, state_info* info);
  float transition_time;
  float message_time;
  vsx_string<>message;
  bool render_first;

  vsx_command_list l_cmd_in;
  vsx_command_list l_cmd_out;
  unsigned long fade_id;
  bool randomizer;
  float randomizer_time;
  bool transitioning;
  float fx_alpha;
  float spd_alpha;
  bool show_progress_bar;
  int first;
  int start_loaded_modules;

  vsx_engine_float_array int_freq;
  vsx_engine_float_array int_wav;

  // options
  bool option_preload_all;

  void preload_engines();

public:

  void set_module_list( vsx_module_list_abs* new_module_list)
  {
    module_list = new_module_list;
  }

  vsx_engine* get_vxe() {
    return vxe;
  }
  
  void add_visual_path(vsx_string<>new_visual_path);

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
  void start();
  void stop();
  void toggle_randomizer();
  void set_randomizer(bool status);
  bool get_randomizer_status();
  
  void select_visual (int selection);
  
  void random_state();
  void next_state();
  void prev_state();

  std::list< vsx_string<> >* get_state_file_list();
  std::list< vsx_string<> >* get_fader_file_list();
  

  vsx_string<>state_loading();
  
  void inc_speed();
  void dec_speed();
  float get_speed();
  void inc_amp();
  void dec_amp();
  float get_fx_level();
  void toggle_fullscreen();
  void render();
  void init(vsx_string<>base_path,vsx_string<>init_sound_type);
  void load_fx_levels_from_user();
  void save_fx_levels_from_user();

  void set_sound_freq(float* data);
  void set_sound_wave(float* data);
  
  vsx_string<>get_meta_visual_filename();
  vsx_string<>get_meta_visual_name();
  vsx_string<>get_meta_visual_creator();
  vsx_string<>get_meta_visual_company();
  
  vsx_statelist();
  ~vsx_statelist();
};

#endif /*VSX_STATELIST_H_*/
