#ifndef VSX_MANAGER_H
#define VSX_MANAGER_H

#include <vsx_string.h>


class vsx_manager_abs
{
public:
  // init manager with base path to where the effects (.vsx files) can be found
  // i.e. if base_path is /usr/share/vsxu/   then the engine will look in
  // /usr/share/vsxu/_visuals
  virtual void init(const char* base_path) {printf("abs_init\n");};

  // before you render first time, you need to start
  virtual void start() {};
  virtual void render() {};
  // if you are going to destroy the GL Context - as is the case
  // when destroying a window - when undocking a window or going fullscreen perhaps
  // you need top stop() the engine so all visuals can unload their OpenGL handles and
  // get ready to be started again.
  virtual void stop() {};

  // flipping through different visuals
  virtual void toggle_randomizer() {};
  virtual void pick_random_visual() {};
  virtual void next_visual() {};
  virtual void prev_visual() {};

  // provide metadata for information about current running effect
  virtual vsx_string get_meta_visual_filename() {return ""; };
  virtual vsx_string get_meta_visual_name() { return "";    };
  virtual vsx_string get_meta_visual_creator() { return ""; };
  virtual vsx_string get_meta_visual_company() { return ""; };

  // amplification/fx level (more = flashier, less = less busy)
  virtual float get_fx_level() { return 0.0f;};
  virtual void inc_fx_level() {};
  virtual void dec_fx_level() {};

  // time speed (more = faster movements, less = slow motion)
  virtual float get_speed() { return 0.0f;};
  virtual void inc_speed() {};
  virtual void dec_speed() {};

  // arbitrary engine information (statistics etc)
  // returns information about currently playing effect
  virtual int get_engine_num_modules() { return 0; };
};

extern "C" {
vsx_manager_abs* manager_factory();
}
#endif