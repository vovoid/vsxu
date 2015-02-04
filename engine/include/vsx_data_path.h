#ifndef VSX_DATA_PATH_H
#define VSX_DATA_PATH_H

#include <string/vsx_string.h>
#include <vsx_engine_dllimport.h>


ENGINE_DLLIMPORT class vsx_data_path
{
  vsx_string<>data_path;

public:

  ENGINE_DLLIMPORT vsx_data_path();

  vsx_string<>& data_path_get() __attribute__((always_inline))
  {
    return data_path;
  }

  void ensure_output_directory(vsx_string<> name);

private:
  ENGINE_DLLIMPORT static vsx_data_path instance;
public:
  static vsx_data_path* get_instance() __attribute__((always_inline))
  {
    return &instance;
  }

};


#endif
