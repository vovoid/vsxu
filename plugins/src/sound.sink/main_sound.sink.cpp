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

#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
#include <unistd.h>
#endif


#if (PLATFORM == PLATFORM_LINUX)
  #include <sys/prctl.h>
#endif

#include <audio/vsx_audio_mixer_manager.h>




//******************************************************************************
//******************************************************************************

#include "vsx_module_raw_sample_trigger.h"
#include "vsx_module_raw_sample_play.h"
#include "vsx_module_ogg_sample_trigger.h"
#include "vsx_module_ogg_sample_play.h"


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

void print_help()
{
}

#ifndef MOD_CM
#define MOD_CM vsx_module_sound_sink_cm
#define MOD_DM vsx_module_sound_sink_dm
#define MOD_NM vsx_module_sound_sink_nm
#endif


vsx_module* MOD_CM(unsigned long module, void* args)
{
  VSX_UNUSED(args);

  switch(module)
  {
      case 0:
          return (vsx_module*)(new vsx_module_raw_sample_trigger);
      case 1:
          return (vsx_module*)(new vsx_module_raw_sample_play);
      case 2:
          return (vsx_module*)(new vsx_module_ogg_sample_trigger);
      case 3:
          return (vsx_module*)(new vsx_module_ogg_sample_play);
  }
  return 0;
}

void MOD_DM(vsx_module* m,unsigned long module)
{
  switch (module)
  {
    case 0:
      return delete (vsx_module_raw_sample_trigger*)m;
    case 1:
      return delete (vsx_module_raw_sample_play*)m;
    case 2:
      return delete (vsx_module_ogg_sample_trigger*)m;
    case 3:
      return delete (vsx_module_ogg_sample_play*)m;
  }

}

unsigned long MOD_NM(vsx_module_engine_environment* environment)
{
  VSX_UNUSED(environment);
  return 4;
}

void on_unload_library()
{
}

