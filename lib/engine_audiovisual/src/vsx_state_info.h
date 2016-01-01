#pragma once

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

  state_info(const state_info &other)
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
  {
    speed = 1.0f;
    engine = 0;
    need_stop = false;
    need_reload = false;
    is_volatile = false;
  }

  ~state_info()
  {
    if (is_volatile)
      return;
    if (engine)
      delete engine;
  }
};
