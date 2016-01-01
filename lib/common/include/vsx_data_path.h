#ifndef VSX_DATA_PATH_H
#define VSX_DATA_PATH_H

#include <string/vsx_string.h>
#include <vsx_common_dllimport.h>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
  #include <sys/stat.h>
#endif

#include <stdio.h>

class COMMON_DLLIMPORT vsx_data_path
{
  vsx_string<>data_path;

public:

  vsx_data_path();

  vsx_string<>& data_path_get()
  {
    return data_path;
  }

  void ensure_output_directory(vsx_string<> name)
  {
    if (access(  (data_path_get() + name).c_str(),0) != 0)
      #if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
        mkdir( (data_path_get() + name).c_str(), 0700 );
      #else
        mkdir( (data_path_get() + name).c_str() );
      #endif
  }


private:
  static vsx_data_path instance;
public:
  static vsx_data_path* get_instance()
  {
    return &instance;
  }

};


#endif
