#pragma once

#include <container/vsx_nw_vector.h>

// engine float arrays, data flowing from the engine to a module
// 0 is reserved for sound data, wave (512 floats)
// 1 is reserved for sound data, freq (512 floats)
typedef struct {
  vsx_nw_vector<float> array;
} vsx_module_engine_float_array;
