/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Vovoid Media Technologies Copyright (C) 2003-2011
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
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
#include "vsx_module.h"
#include "vsx_float_array.h"
#include "vsx_math_3d.h"

#include "pulse/simple.h"
#include "pulse/error.h"
#include "pulse/gccmacro.h"
#include <pthread.h>
#include "fftreal/fftreal.h"
#include <unistd.h>

int rtaudio_started = 0;

typedef struct
{
    float l_mul;
    vsx_float_array spectrum[2];
    vsx_float_array wave[2];    // 512 L 512 R BANZAI!
    //float spectrum[2][512];
    float vu[2];
    float octaves[2][8];
} vsx_paudio_struct;

vsx_paudio_struct pa_audio_data;

  /*
  i = 0..n	1.3 ^ i	   1.3 ^ i - 1	  1.3^i-1 / 1.3^n	  (1.3^i-1 / 1.3^n) * (n-1) + 1
  0	        1	         0	            0	                1
  1	        1,3	       0,3	          0,02346344	      1,211170956
  2	        1,69	     0,69	          0,053965911	      1,4856932
  3	        2,197	     1,197	        0,093619124	      1,842572116
  4	        2,8561	   1,8561	        0,145168301	      2,306514707
  5	        3,71293	   2,71293	      0,212182231	      2,909640075
  6	        4,826809	 3,826809	      0,299300339	      3,693703054
  7	        6,2748517	 5,2748517	    0,412553881	      4,712984927
  8	        8,15730721 7,15730721	    0,559783485	      6,038051361
  9	       10,60449937 9,604499373    0,75118197        7,760637726
  10       13,7858491812,78584918	    1	                10
  */
//void normalize_fft(float* fft, vsx_float_array& spectrum) {
//  float B1 = pow(8.0,1.0/512.0); //1.004
//  float dd = 1*(512.0/8.0);
//  float a = 0;
//  float b;
//  float diff = 0;
//  int aa;
//  for (int i = 1; i < 512; ++i) {
//    (*(spectrum.data))[i] = 0;
//    b = (float)((pow(B1,(float)(i))-1.0)*dd);
//    diff = b-a;
//    aa = (int)floor(a);

//    if((int)b == (int)a) {
//      (*(spectrum.data))[i] = fft[aa]*3 * diff;
//    }
//    else
//    {
//      ++aa;
//      (*(spectrum.data))[i] += fft[aa]*3 * (ceil(a) - a);
//      while (aa != (int)b)
//      {
//        (*(spectrum.data))[i] += fft[aa]*3;
//        ++aa;
//      }
//      (*(spectrum.data))[i] += fft[aa+1]*3 * (b - floor(b));
//    }
//    a = b;
//  }
//}

//******************************************************************************
//******************************************************************************
//******************************************************************************
//******************************************************************************

float fft[512];

int rtaudio_type = 0;

#include "vsx_listener_rtaudio.h"
#include "vsx_listener_mediaplayer.h"


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
  __declspec(dllexport) unsigned long get_num_modules();
  __declspec(dllexport) void on_unload_library();
}

size_t sound_module_type = 0;

void print_help()
{
  printf("Parameters for vsx_listener (visualization sound input):\n");
  #if (PLATFORM == PLATFORM_LINUX)
  printf("  PulseAudio is the default sound driver, but you can override with any\n"
         "  of the following:\n\n");
  printf("  -sound_type_alsa            - ALSA sound system\n");
  printf("  -sound_type_jack            - JACK sound system\n");
  printf("  -sound_type_oss             - OSS sound system\n");
  #endif
  #if (PLATFORM == PLATFORM_WINDOWS)
  printf("  DirectSound is the default sound driver, but you can override with any\n"
         "  of the following:\n\n");
  printf("  -sound_type_asio            - ASIO sound system\n");
  #endif
}


vsx_module* create_new_module(unsigned long module, void* args)
{
  vsx_argvector* internal_args = (vsx_argvector*) args;
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

      if (!rtaudio_started)
      {
        setup_rtaudio();
        rtaudio_started++;
      }
      sound_module_type = 0;
      return (vsx_module*)(new vsx_listener_pulse);
    }
  }
  return 0;
}

void destroy_module(vsx_module* m,unsigned long module)
{
  switch(sound_module_type)
  {
    case 0:
    return delete (vsx_listener_pulse*)m;
    case 1:
    return delete (vsx_listener_mediaplayer*)m;
  }
}

unsigned long get_num_modules() {
  return 1;
}

void on_unload_library()
{
  shutdown_rtaudio();
}

