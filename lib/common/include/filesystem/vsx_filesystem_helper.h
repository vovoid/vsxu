#pragma once

#include <list>
#include <string/vsx_string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <vsx_platform.h>

#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
#include <windows.h>
#endif


namespace vsx
{
namespace filesystem_helper
{
  inline size_t file_get_size(vsx_string<> filename)
  {
    FILE* fp = fopen(filename.c_str(), "rb");
    if (!fp)
      return 0;

    fseek (fp, 0, SEEK_END);
    int s = ftell(fp);
    fclose(fp);

    return (size_t)s;
  }


  inline void create_directory(const char* path)
  {
    char dir_name[4096];
    char* p = (char*)path;
    char* q = dir_name;
    while( *p )
    {
      if ( DIRECTORY_SEPARATOR_CHAR == *p )
      {
        if (p != (char*)path && ':' != *( p-1 ) )
        {
          #if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
            mkdir(dir_name,0700);
          #endif
          #if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
            CreateDirectory(dir_name, NULL);
          #endif
        }
      }
      *q++ = *p++;
      *q = '\0';
    }
    #if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
      mkdir(dir_name,0700);
    #endif
    #if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
      CreateDirectory(dir_name, NULL);
    #endif
  }


  void get_files_recursive(
      vsx_string<> startpos,
      std::list< vsx_string<> >* filenames,
      vsx_string<> include_filter = "",
      vsx_string<> exclude_filter = "",
      vsx_string<> dir_ignore_token = ".vsx_hidden"
    );
}
}
