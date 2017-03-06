#pragma once

#include <vsx_engine.h>

class state_info {
public:
  float fx_level = 1.0f;
  float speed = 1.0f;
  vsx::filesystem* filesystem = 0x0;
  vsx_engine* engine = 0x0;
  vsx_string<> state_name;
  vsx_string<> state_name_suffix;
  vsx_command_list cmd_in;
  vsx_command_list cmd_out;
  bool need_stop = false;
  bool need_reload = false;
  bool is_volatile = false;

  state_info(const state_info &other)
    :
      cmd_in(false),
      cmd_out(false)
  {
    fx_level = other.fx_level;
    speed = other.speed;
    engine = other.engine;
    state_name = other.state_name;
    state_name_suffix = other.state_name_suffix;
    cmd_in = other.cmd_in;
    cmd_out = other.cmd_out;
    need_stop = other.need_stop;
    need_reload = other.need_reload;
    is_volatile = other.is_volatile;
  }

  state_info()
    :
      cmd_in(false),
      cmd_out(false)
  {
  }

  ~state_info()
  {
    req(!is_volatile);
    req(engine);
    delete engine;
  }
};
