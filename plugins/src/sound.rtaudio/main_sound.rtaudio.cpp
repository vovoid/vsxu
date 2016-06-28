/**
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
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


#include "_configuration.h"
#if defined(WIN32) || defined(_WIN64) || defined(__WATCOMC__)
  #include <windows.h>
  #include <conio.h>
#else
  //#include "wincompat.h"
#endif
#include "vsx_param.h"
#include <module/vsx_module.h>
#include <math/vsx_float_array.h>

#include "fftreal/fftreal.h"

#include <RtAudio/RtAudio.h>
//#include <RtAudio/RtError.h>


#if (PLATFORM == PLATFORM_LINUX)
#include <unistd.h>
#include <sys/prctl.h>
#endif

// rt audio driver type
int rtaudio_type = 0;


typedef struct
{
    float l_mul;
    vsx_float_array spectrum[2];
    vsx_float_array wave[2];    // 512 L 512 R BANZAI!
    //float spectrum[2][512];
    float vu[2];
    float octaves[2][8];
} vsx_audio_record_buf;

vsx_audio_record_buf pa_audio_data;


#include "rtaudio_play.h"
#include "rtaudio_record.h"


//******************************************************************************
//******************************************************************************

#include "vsx_listener_rtaudio.h"
#include "vsx_listener_mediaplayer.h"
#include "vsx_module_raw_sample_trigger.h"
#include "vsx_module_raw_sample_play.h"
#include "vsx_module_ogg_sample_trigger.h"
#include "vsx_module_ogg_sample_play.h"
#include "vsx_module_midi_akai_apc.h"

int vsx_module_midi_akai_apc_40_controller::num_modules = 0;

//******************************************************************************
//*** F A C T O R Y ************************************************************
//******************************************************************************

#ifndef _WIN32
#define __declspec(a)
#endif

extern "C"
{
  __declspec(dllexport) void print_help();
  __declspec(dllexport) vsx_module* create_new_module(unsigned long module, void* args);
  __declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
  __declspec(dllexport) unsigned long get_num_modules(vsx_module_engine_environment* environment);
  __declspec(dllexport) void on_unload_library();
}

#ifndef MOD_CM
#define MOD_CM vsx_module_sound_rtaudio_cm
#define MOD_DM vsx_module_sound_rtaudio_dm
#define MOD_NM vsx_module_sound_rtaudio_nm
#endif


size_t sound_module_type = 0;

void print_help()
{
  printf("Parameters for vsx_listener (visualization sound input):\n");
  #if (PLATFORM == PLATFORM_LINUX)
  printf("  PulseAudio is the default sound driver, but you can override with any\n"
         "  of the following:\n\n");
  #if defined(__LINUX_ALSA__)
  printf("  -sound_type_alsa            - ALSA sound system\n");
  #endif

  #if defined(__LINUX_JACK__)
  printf("  -sound_type_jack            - JACK sound system\n");
  #endif

  #if defined(__LINUX_OSS__)
  printf("  -sound_type_oss             - OSS sound system\n");
  #endif
  #endif
  #if (PLATFORM == PLATFORM_WINDOWS)
  printf("  DirectSound is the default sound driver, but you can override with any\n"
         "  of the following:\n\n");
  printf("  -sound_type_asio            - ASIO sound system\n");
  #endif
}


vsx_module* MOD_CM(unsigned long module, void* args)
{
  vsx_argvector* internal_args = (vsx_argvector*) args;
  #if (PLATFORM == PLATFORM_LINUX)
  if (internal_args->has_param("sound_type_alsa"))
  {
    // ALSA
    rtaudio_type = RtAudio::LINUX_ALSA;
  } else
  if (internal_args->has_param("sound_type_jack"))
  {
    // JACK
    rtaudio_type = RtAudio::UNIX_JACK;
  } else
  if (internal_args->has_param("sound_type_oss"))
  {
    // OSS
    rtaudio_type = RtAudio::LINUX_OSS;
  } else
  {
    // default - PulseAudio
    rtaudio_type = RtAudio::LINUX_PULSE;
  }
  #endif
  #if (PLATFORM == PLATFORM_WINDOWS)
  if (internal_args->has_param("sound_type_asio"))
  {
    // asio
    rtaudio_type = RtAudio::WINDOWS_ASIO;
  } else
  {
    // directsound
    rtaudio_type = RtAudio::WINDOWS_DS;
  }
  #endif
  switch(module)
  {
    case 0:
      if (internal_args->has_param("sound_type_media_player"))
      {
        sound_module_type = 1;
        return (vsx_module*)(new vsx_listener_mediaplayer);
      }
      else
      {
        sound_module_type = 0;
        return (vsx_module*)(new vsx_listener_pulse);
      break;
      case 1:
          return (vsx_module*)(new vsx_module_raw_sample_trigger);
      case 2:
          return (vsx_module*)(new vsx_module_raw_sample_play);
      case 3:
          return (vsx_module*)(new vsx_module_ogg_sample_trigger);
      case 4:
          return (vsx_module*)(new vsx_module_ogg_sample_play);
      case 5:
          return (vsx_module*)(new vsx_module_midi_akai_apc_40_controller);
    }
  }
  return 0;
}

void MOD_DM(vsx_module* m,unsigned long module)
{
  switch (module)
  {
    case 0:
      switch(sound_module_type)
      {
        case 0:
        return delete (vsx_listener_pulse*)m;
        case 1:
        return delete (vsx_listener_mediaplayer*)m;
      }
    break;
    case 1:
      return delete (vsx_module_raw_sample_trigger*)m;
    case 2:
      return delete (vsx_module_raw_sample_play*)m;
    case 3:
      return delete (vsx_module_ogg_sample_trigger*)m;
    case 4:
      return delete (vsx_module_ogg_sample_play*)m;
    case 5:
      return delete (vsx_module_midi_akai_apc_40_controller*)m;
  }

}

unsigned long MOD_NM(vsx_module_engine_environment* environment)
{
  VSX_UNUSED(environment);
  return 6;
}

void on_unload_library()
{
  shutdown_rtaudio_record();
}

