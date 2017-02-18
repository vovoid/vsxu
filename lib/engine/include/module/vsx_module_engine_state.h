/*
  The information that the module get from the engine. All modules share a pointer
  to this struct.
*/

#pragma once

#include <input/vsx_input_event_queue.h>
#include <filesystem/vsx_filesystem.h>
#include "vsx_module_engine_float_array.h"

#define VSX_ENGINE_LOADING -1
#define VSX_ENGINE_STOPPED 0
#define VSX_ENGINE_PLAYING 1
#define VSX_ENGINE_REWIND 2

class vsx_module_engine_state
{
public:

  // filesystem handle to use in every module's file operations
  vsx::filesystem* filesystem;

  // event queue - use vsx_input_event_queue_reader to consume these events
  // Example:
  //   vsx_input_event_queue_reader reader(engine_state->event_queue);
  //   reader.consume(...);
  vsx_input_event_queue* event_queue;

  // module list - so that modules can construct their own vsx_engine's
  void* module_list;

  int state = VSX_ENGINE_STOPPED; // stopped or playing

  // engine effect amplification - can be used freely by the modules
  // but should primarily control
  float amp = 1.0f;

  // engine speed - dt can be multiplied with this
  // accessible through pgup / pgdn in vsxu player for instance
  float speed = 1.0f;

  // sequencer time
  // delta time, frame time, seconds
  float dtime = 0.0f;
  // accumulated time, seconds
  float vtime = 0.0f;

  // real time
  float real_dtime = 0.0f;
  float real_vtime = 0.0f;

  // time control from module
  int request_play = 0;
  int request_stop = 0;
  int request_rewind = 0;
  float request_set_time = -0.01f;

  // this is to send parameters from the implementor of the engine down to the modules, like sound data etc
  // should only be used for implementation-specific modules that are not part of a normal vsxu distribution.
  // item 0 is reserved for realtime PCM data for visualization (from host app to module)
  // item 1 is reserved for realtime FFT data for visualization (from host app to module)
  // item 2 is reserved for Full song PCM data for the sequencer, Left Channel (from module to host app)
  // item 3 is reserved for Full song PCM data for the sequencer, Right Channel (from module to host app)
  // item 4..999 are reserved for Vovoid use
  vsx_nw_vector<vsx_module_engine_float_array*> param_float_arrays;
};
