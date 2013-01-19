/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Dinesh Manajipet, Vovoid Media Technologies - Copyright (C) 2012-2020
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

#include "input/input_audio_midi.h"
#include "input/input_audio_raw.h"
#include "input/input_audio_mediaplayer.h"

#ifndef _WIN32
#define __declspec(a)
#endif

extern "C" {
__declspec(dllexport) vsx_module* create_new_module(unsigned long module);
__declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
__declspec(dllexport) unsigned long get_num_modules();
}

vsx_module* create_new_module(unsigned long module) {
  switch(module){
    case 0:
      return (vsx_module*)(new input_audio_midi);
    case 1:
      return (vsx_module*)(new input_audio_raw);
    case 2:
      return (vsx_module*)(new input_audio_mediaplayer);
  }
}

void destroy_module(vsx_module* m,unsigned long module) {
  switch(module){
    case 0:
      return delete (input_audio_midi*)m;
    case 1:
      return delete (input_audio_raw*)m;
    case 2:
      return delete (input_audio_mediaplayer*)m;
  }
}

unsigned long get_num_modules() {
  return 3;
}