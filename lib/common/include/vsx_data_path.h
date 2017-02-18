#ifndef VSX_DATA_PATH_H
#define VSX_DATA_PATH_H

#include <string/vsx_string.h>
#include <vsx_common_dllimport.h>

#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
#include <windows.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string>
#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
#include <unistd.h>
#include <sys/stat.h>
#endif

#include <stdio.h>

class COMMON_DLLIMPORT vsx_data_path
{
  vsx_string<>data_path;

public:

  void init();

  vsx_string<>& data_path_get()
  {
    return data_path;
  }

  void ensure_output_directory(vsx_string<> name)
  {
      #if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
        if (access((data_path_get() + name).c_str(), 0) != 0)
        mkdir( (data_path_get() + name).c_str(), 0700 );
      #else
        DWORD dwAttr = GetFileAttributes((data_path_get() + name).c_str());
        if (dwAttr != 0xffffffff && (dwAttr & FILE_ATTRIBUTE_DIRECTORY))       
          CreateDirectory( (data_path_get() + name).c_str(), NULL );
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
