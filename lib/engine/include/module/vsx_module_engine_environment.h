#pragma once

#include <container/vsx_nw_vector.h>
#include <string/vsx_string.h>

/*
Environment information
*/
typedef struct {
  // engine_parameter[0] is module shared data path, data/config files that are used to define behaviour and existence of modules.
  //                     Examples: The render.glsl module is a good example. It returns one available module per file it finds
  //                               in its data directory.
  vsx_nw_vector< vsx_string<> > engine_parameter;
} vsx_module_engine_environment;

