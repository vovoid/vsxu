#ifndef VSX_DATA_PATH_H
#define VSX_DATA_PATH_H

#include <vsx_string.h>

class vsx_data_path
{
  vsx_string data_path;

public:

  vsx_data_path();

  vsx_string& data_path_get() __attribute__((always_inline))
  {
    return data_path;
  }

  static vsx_data_path* get_instance() __attribute__((always_inline))
  {
    static vsx_data_path vdp;
    return &vdp;
  }

};


#endif
