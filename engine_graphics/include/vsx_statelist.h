#ifndef VSX_STATELIST_H_
#define VSX_STATELIST_H_

#include <vsx_platform.h>
#if PLATFORM == PLATFORM_LINUX
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#endif

#include "vsx_engine.h"

class state_info {
public:
  float fx_level;
  float speed;
  vsx_engine* engine;
  vsx_string state_name;
  vsx_string state_name_suffix;
  vsx_command_list cmd_in;
  vsx_command_list cmd_out;
  bool need_stop;
  bool need_reload;

  state_info() {
    speed = 1.0f;
    engine = 0;
    need_stop = false;
    need_reload = false;
  }
};

// WARNING! INIT THIS YOURSELF WITH THE NEW OPERATOR SOMEWHERE
// IN THE MAIN() METHOD OTHERWISE ARGC AND ARGV WILL BE VOID!



class vsx_statelist {
private:
  vsx_string own_path;
  std::list<vsx_string> state_file_list;
  std::list<vsx_string> fader_file_list;
  std::vector<state_info> statelist;
  std::vector<state_info>::iterator state_iter;

  std::vector<vsx_engine*> faders;

  vsx_engine* vxe;
  vsx_engine* vxe_to;

  vsx_command_list *cmd_in;
  vsx_command_list *cmd_out;
  vsx_texture tex1;
  vsx_texture tex_to;

  vsx_timer timer;

  vsx_string config_dir;
  vsx_string visual_path;

  void init_current(vsx_engine *vxe_local, state_info* info);
  float transition_time;
  float message_time;
  vsx_string message;
  bool render_first;
  vsx_engine* lvxe;
  vsx_command_list l_cmd_in;
  vsx_command_list l_cmd_out;
  unsigned long fade_id;
  bool no_fbo_ati;
  bool randomizer;
  float randomizer_time;
  bool transitioning;
  float fx_alpha;
  float spd_alpha;
  bool show_progress_bar;
  int first;
  int start_loaded_modules;

public:

  vsx_engine* get_vxe() {
    return vxe;
  }

  state_info* get_state() {
    return &(*state_iter);
  }
  void start();
  void stop();
  void toggle_randomizer();
  void random_state();
  void next_state();
  void prev_state();
  void inc_speed();
  void dec_speed();
  void inc_amp();
  void dec_amp();
  void toggle_fullscreen();
  void render();
  void init(vsx_string base_path);
  void load_fx_levels_from_user();
  void save_fx_levels_from_user();

  vsx_statelist();
};

#endif /*VSX_STATELIST_H_*/
