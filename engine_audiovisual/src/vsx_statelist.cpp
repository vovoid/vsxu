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

#include "vsx_statelist.h"


int vsx_statelist::init_current(vsx_engine *vxe_local, state_info* info) {
  //title_timer = 5.0f;
  if (vxe_local == 0)
  {
    vxe_local = new vsx_engine();
    vxe_local->set_module_list( module_list );
    vxe_local->set_no_send_client_time(true);
    vxe_local->start();
    (*state_iter).engine = vxe_local;
#ifdef VSXU_DEBUG
    printf("loading state: %s\n", (*state_iter).state_name.c_str());
#endif
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

void vsx_statelist::add_visual_path(vsx_string new_visual_path)
{
  get_files_recursive(new_visual_path, &state_file_list,"","");

  #ifdef VSXU_DEBUG
  printf("getting files recursive: %s\n", (new_visual_path).c_str() );
  #endif

  for (std::list<vsx_string>::iterator it = state_file_list.begin(); it != state_file_list.end(); ++it) {
      state_info state;
      state.state_name = *it;
      state.state_name_suffix = state.state_name.substr(new_visual_path.size(),state.state_name.size() - new_visual_path.size() );
  #ifdef VSXU_DEBUG
      printf("adding state %s\n",(*it).c_str());
  #endif
      statelist.push_back(state);
  }
}

void vsx_statelist::toggle_randomizer() 
{
  randomizer = !randomizer;
}

bool vsx_statelist::get_randomizer_status()
{
  return randomizer;
}

void vsx_statelist::set_randomizer(bool status)
{
  randomizer = status;
}

void vsx_statelist::select_visual(int selection)
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

void vsx_statelist::next_state()
{
  if ((*state_iter).engine != vxe) return;
  ++state_iter;
  if (state_iter == statelist.end()) state_iter = statelist.begin();
  init_current((*state_iter).engine, &(*state_iter));
  transition_time = 2.0f;
}
void vsx_statelist::prev_state()
{
  if ((*state_iter).engine != vxe) return;
  if (state_iter == statelist.begin()) state_iter = statelist.end();
  --state_iter;
  init_current((*state_iter).engine, &(*state_iter));
  transition_time = 2.0f;
}


std::list<vsx_string>* vsx_statelist::get_state_file_list()
{
  return &state_file_list;
}

std::list<vsx_string>* vsx_statelist::get_fader_file_list()
{
  return &fader_file_list;
}


vsx_string vsx_statelist::state_loading()
{
  if (transition_time > 0.0f && transition_time < 2.0f) {
    return (*state_iter).state_name;
  }
  return "";
}

void vsx_statelist::inc_speed() 
{
  (*state_iter).speed *= 1.04f;
  if ((*state_iter).speed > 16.0f) (*state_iter).speed = 16.0f;
  vxe->set_speed((*state_iter).speed);
}

void vsx_statelist::dec_speed() 
{
  (*state_iter).speed *= 0.96f;
  if ((*state_iter).speed < 0.0f) (*state_iter).speed = 0.0f;
  vxe->set_speed((*state_iter).speed);
}


float vsx_statelist::get_speed()
{
  return (*state_iter).speed;
}


void vsx_statelist::inc_amp() 
{
  (*state_iter).fx_level+=0.05f;
  if ((*state_iter).fx_level > 16.0f) (*state_iter).fx_level = 16.0f;
#if defined(__linux__)
  vsx_string fxlf = config_dir+"/"+(*state_iter).state_name_suffix.substr(visual_path.size()+1 , (*state_iter).state_name_suffix.size())+"_fx_level";
#ifdef VSXU_DEBUG
  printf("fx level file: %s\n", fxlf.c_str() );
#endif
  FILE* fxfp = fopen( fxlf.c_str(), "w");
  if (fxfp)
  {
    fputs(f2s((*state_iter).fx_level).c_str(), fxfp);
    fclose(fxfp);
  }
#endif
  vxe->set_amp((*state_iter).fx_level);
  fx_alpha = 5.0f;
}

void vsx_statelist::dec_amp()
{
  (*state_iter).fx_level-=0.05f;
  if ((*state_iter).fx_level < 0.1f) (*state_iter).fx_level = 0.1f;
  vxe->set_amp((*state_iter).fx_level);
#if defined(__linux__)
  vsx_string fxlf = config_dir+"/"+(*state_iter).state_name_suffix.substr(visual_path.size()+1 , (*state_iter).state_name_suffix.size())+"_fx_level";
  FILE* fxfp = fopen( fxlf.c_str(), "w");
  if (fxfp)
  {
    fputs(f2s((*state_iter).fx_level).c_str(), fxfp);
    fclose(fxfp);
  }
#endif
  fx_alpha = 5.0f;
}

float vsx_statelist::get_fx_level()
{
  return (*state_iter).fx_level;
}

void vsx_statelist::start()
{
  vxe->start();
  vxe->load_state((*state_iter).state_name);
}

void vsx_statelist::stop()
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

vsx_string vsx_statelist::get_meta_visual_filename()
{
    return (*state_iter).state_name;
}
vsx_string vsx_statelist::get_meta_visual_name()
{
    if((*state_iter).engine)
        return (*state_iter).engine->get_meta_information(0);
    return vsx_string();
}
vsx_string vsx_statelist::get_meta_visual_creator()
{
    if((*state_iter).engine)
        return (*state_iter).engine->get_meta_information(1);
    return vsx_string();
}
vsx_string vsx_statelist::get_meta_visual_company()
{
    if((*state_iter).engine)
        return (*state_iter).engine->get_meta_information(2);
    return vsx_string();
}



/*void vsx_statelist::toggle_fullscreen() 
{
  vsx_texture aa;
  aa.unload_all_active();
  bool a = app_get_fullscreen(0);
  //vxe->stop();
  for (std::vector<state_info>::iterator it = statelist.begin(); it != statelist.end(); ++it)
  {
    if ((*it).engine)
    (*it).engine->stop();
    (*it).need_reload = true;
    //(*it).need_stop = true;
  }
  //(*state_iter).need_stop = false;
  vxe->unload_state();
  vxe->stop();
  app_set_fullscreen(0,!a);
  vxe->start();
  vxe->load_state((*state_iter).state_name);
  // todo, clear out the other engines as well
}*/

void vsx_statelist::random_state() {
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

void vsx_statelist::preload_engines()
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

void vsx_statelist::render() 
{
  if (render_first)
  {
    glewInit();
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    if (tex1.has_buffer_support())
    {
      tex1.init_render_buffer(viewport[2], viewport[3]);
      tex_to.init_render_buffer(viewport[2], viewport[3]);

      get_files_recursive(own_path+"visuals_faders", &fader_file_list,"",".svn CVS");
      for (std::list<vsx_string>::iterator it = fader_file_list.begin(); it != fader_file_list.end(); ++it)
      {
        #ifdef VSXU_DEBUG
          printf("initializing fader %s\n", (*it).c_str());
        #endif
        vsx_engine* lvxe = new vsx_engine();
        lvxe->set_module_list( module_list );
        lvxe->start();
        lvxe->load_state(*it);
        faders.push_back(lvxe);
        fade_id = 0;
      }
    }
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
  } // render first

  // prevent from rendering by mistake
  if ( !statelist.size() ) return;

  if ((*state_iter).engine != vxe) // change is on the way
  {
    if ( tex_to.has_buffer_support() )
    {
      tex_to.begin_capture_to_buffer();
        if ((*state_iter).engine)
        {
          (*state_iter).engine->process_message_queue(&(*state_iter).cmd_in,&(*state_iter).cmd_out);
          (*state_iter).engine->render();
        }
        glColorMask(false, false, false, true);
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glColorMask(true, true, true, true);
      tex_to.end_capture_to_buffer();
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
    } else
    {
      transition_time = -1.0f;
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
        {
          vxe->process_message_queue(cmd_in,cmd_out);
        }
        cmd_out->clear(true);
      }
      if (vxe)
      {
        vxe->render();
      }
    } else
    {
      if (cmd_out && cmd_in)
      {
        if (vxe)
        {
          vxe->process_message_queue(cmd_in,cmd_out);
        }
        cmd_out->clear(true);
      }

      // begin capture
      if (tex1.has_buffer_support())
      {
        tex1.begin_capture_to_buffer();
      }

      // render
      if (vxe)
      {
        vxe->render();
      }
      glColorMask(false, false, false, true);
      glClearColor(0,0,0,1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glColorMask(true, true, true, true);

      // end capture and send to fader
      if (tex1.has_buffer_support())
      {
        tex1.end_capture_to_buffer();
        vsx_module_param_texture* param_t_a = (vsx_module_param_texture*)faders[fade_id]->get_in_param_by_name("visual_fader", "texture_a_in");
        vsx_module_param_texture* param_t_b = (vsx_module_param_texture*)faders[fade_id]->get_in_param_by_name("visual_fader", "texture_b_in");
        vsx_module_param_float* param_pos = (vsx_module_param_float*)faders[fade_id]->get_in_param_by_name("visual_fader", "fade_pos_in");
        vsx_module_param_float* fade_pos_from_engine = (vsx_module_param_float*)faders[fade_id]->get_in_param_by_name("visual_fader", "fade_pos_from_engine");
        faders[fade_id]->process_message_queue(&l_cmd_in, &l_cmd_out);
        l_cmd_out.clear();
        if (param_t_a && param_t_b && param_pos && fade_pos_from_engine)
        {
          param_t_a->set(&tex1);
          param_t_b->set(&tex_to);
          fade_pos_from_engine->set(1.0f);
          float t = transition_time;
          if (t > 1.0f) t = 1.0f;
          if (t < 0.0f) t = 0.0f;
          param_pos->set(1.0-t);
          glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
          faders[fade_id]->render();
        }
      }

      if (transition_time <= 1.0f)
      {
        transition_time -= timer.dtime();
      }
    }
  } else
  {
    if (cmd_out && cmd_in)
    {
      vxe->process_message_queue(cmd_in, cmd_out);
      cmd_out->clear();
    }
    vxe->render();
    if (randomizer)
    {
      randomizer_time -= vxe->get_engine_info()->real_dtime;
      if (randomizer_time < 0.0f)
      {
        random_state();
        randomizer_time = (float)(rand()%1000)*0.001f*15.0f+10.0f;
      }
    }
  }
}



void vsx_statelist::load_fx_levels_from_user()
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
    vsx_string fxlf = config_dir+"/"+state.state_name_suffix.substr(visual_path.size()+1 , state.state_name_suffix.size())+"_fx_level";
    #ifdef VSXU_DEBUG
      printf("fx level file: %s\n", fxlf.c_str() );
    #endif
    if ( stat( fxlf.c_str(), &st) != 0 )
    {
      // no fx level file
      FILE* fpfx = fopen(fxlf.c_str(), "w");
      if (fpfx)
      {
        vsx_string ff = "1.0";
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
      vsx_string ff = dest;
      state.fx_level = s2f(ff);
    }
  }
#endif
}


void vsx_statelist::set_sound_freq(float* data)
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

void vsx_statelist::set_sound_wave(float* data)
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


void vsx_statelist::save_fx_levels_from_user()
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
    vsx_string fxlf = config_dir+"/"+state.state_name_suffix.substr(visual_path.size()+1 , state.state_name_suffix.size())+"_fx_level";
#ifdef VSXU_DEBUG
    printf("fx level file: %s\n", fxlf.c_str() );
#endif
    if ( stat( fxlf.c_str(), &st) != 0 )
    {
      // no fx level file
      FILE* fpfx = fopen(fxlf.c_str(), "w");
      if (fpfx)
      {
        vsx_string ff = "1.0";
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
      vsx_string ff = dest;
      state.fx_level = s2f(ff);
    }
  }
#endif
}


void vsx_statelist::init(vsx_string base_path,vsx_string init_sound_type)
{
  randomizer = true;
  srand ( time(NULL)+rand() );
  randomizer_time = (float)(rand()%1000)*0.001f*15.0f+10.0f;
  transition_time = 2.0f;
  message_time = -1.0f;
  fx_alpha = 0.0f;
  spd_alpha = 0.0f;
  render_first = true;
  transitioning = false;
  first = 2;
  show_progress_bar = false;
  vxe = 0;
  sound_type = init_sound_type;

  cmd_out = 0;
  own_path = base_path;
#ifdef VSXU_DEBUG
  printf("own path: %s\n", own_path.c_str() );
#endif

#if PLATFORM == PLATFORM_WINDOWS
  if (own_path.size())
  if (own_path[own_path.size()-1] != '\\') own_path.push_back('\\');
#else
  if (own_path.size())
  if (own_path[own_path.size()-1] != '/') own_path.push_back('/');
#endif
  visual_path = "visuals_player";

  get_files_recursive(own_path+visual_path, &state_file_list,"","");
#ifdef VSXU_DEBUG
  printf("getting files recursive: %s\n", (own_path+visual_path).c_str() );
#endif
  for (std::list<vsx_string>::iterator it = state_file_list.begin(); it != state_file_list.end(); ++it) {
    state_info state;
    state.state_name = *it;
    state.state_name_suffix = state.state_name.substr(own_path.size(),state.state_name.size() - own_path.size() );
#ifdef VSXU_DEBUG
    printf("adding state %s\n",(*it).c_str());
#endif

    state.fx_level = 1.0f;
    state.engine = 0;
    statelist.push_back(state);
  }
  state_iter = statelist.begin();
  load_fx_levels_from_user();
}

vsx_statelist::vsx_statelist() 
{
  option_preload_all = false;
}

vsx_statelist::~vsx_statelist()
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
