#ifndef VSX_DATA_PATH_H
#define VSX_DATA_PATH_H

#include <vsx_string.h>


#ifdef _WIN32
#if VSX_ENG_DLL
# define DLLIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
# define DLLIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */
#else
#define DLLIMPORT
#endif


DLLIMPORT class vsx_data_path
{
  vsx_string data_path;

public:

  DLLIMPORT vsx_data_path();

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
