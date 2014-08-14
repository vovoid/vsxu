#ifndef VSX_DATA_PATH_H
#define VSX_DATA_PATH_H

#include <vsx_string.h>
#include <vsx_engine_dllimport.h>
#include "vsx_engine.h"



ENGINE_DLLIMPORT class vsx_data_path
{
  vsx_string data_path;

public:

  ENGINE_DLLIMPORT vsx_data_path();

  vsx_string& data_path_get() __attribute__((always_inline))
  {
    return data_path;
  }

private:
  ENGINE_DLLIMPORT static vsx_data_path instance;
public:
  static vsx_data_path* get_instance() __attribute__((always_inline))
  {
    return &instance;
  }

};


#endif
