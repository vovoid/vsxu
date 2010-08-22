#ifndef VSX_MANAGER_H
#define VSX_MANAGER_H

#include <vsx_string.h>
#include <vsx_platform.h>
#include <string>

#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
#define VSX_MANAGER_DLLIMPORT
#else
  #if defined(VSX_ENG_DLL)
    #define VSX_MANAGER_DLLIMPORT __declspec (dllexport)
  #else 
    #define VSX_MANAGER_DLLIMPORT __declspec (dllimport)
  #endif
#endif
class vsx_manager_abs
{
public:
  // init manager with base path to where the effects (.vsx files) can be found
  // i.e. if base_path is /usr/share/vsxu/   then the engine will look in
  // /usr/share/vsxu/_visuals (linux)
  //
  // sound types can be "pulseaudio", "media_player", "fmod"
  virtual void init(const char* base_path, const char* sound_type) {};

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
  virtual void set_randomizer(bool status) {};
  virtual bool get_randomizer_status() {};
  virtual void pick_random_visual() {};
  virtual void next_visual() {};
  virtual void prev_visual() {};
  // if not empty string vsxu is asynchronously loading a new visualization before
  // fading to it, this method can be used to display to the user "loading visual xxxxxx..."
  virtual std::string visual_loading() {};

  // provide metadata for information about current running visualization
  virtual std::string get_meta_visual_filename() {return ""; };
  virtual std::string get_meta_visual_name() { return "";    };
  virtual std::string get_meta_visual_creator() { return ""; };
  virtual std::string get_meta_visual_company() { return ""; };

  // amplification/fx level (more = flashier, less = less busy)
  virtual float get_fx_level() { return 0.0f;};
  virtual void inc_fx_level() {};
  virtual void dec_fx_level() {};

  // time speed (more = faster movements, less = slow motion)
  virtual float get_speed() { return 0.0f;};
  virtual void inc_speed() {};
  virtual void dec_speed() {};

  // update engine sound data,
  // arrays MUST be 512 floats long
  virtual void set_sound_freq(float* data) {};
  virtual void set_sound_wave(float* data) {};

  // arbitrary engine information (statistics etc)
  // returns information about currently playing effect
  virtual int get_engine_num_modules() { return 0; };
};

extern "C" {
VSX_MANAGER_DLLIMPORT vsx_manager_abs* manager_factory();
}
#endif
