#pragma once

#include <list>
#include <string/vsx_string.h>
#include <string/vsx_string_helper.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <vsx_platform.h>
#include <debug/vsx_error.h>
#include <container/vsx_ma_vector.h>

#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
#include <windows.h>
#endif


namespace vsx
{
namespace filesystem_helper
{
  inline bool is_dir(vsx_string<> filename)
  {
    struct stat s;
    stat(filename.c_str(), &s);
    return (s.st_mode & S_IFDIR) != 0;
  }

  /**
    dev 2
    st mode 33206
    nlink 1
    rdev 2
   * @brief file_get_size
   * @param filename
   * @return
   */

  inline size_t file_get_size(vsx_string<> filename)
  {
    FILE* fp = fopen(filename.c_str(), "rb");
    if (!fp)
      return 0;

    fseek (fp, 0, SEEK_END);
    long s = ftell(fp);
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

  inline vsx_ma_vector<unsigned char> read(vsx_string<> filename)
  {
    vsx_ma_vector<unsigned char> result;
    FILE* fp = fopen(filename.c_str(),"rb");

    if (!fp)
      VSX_ERROR_RETURN_V(("error opening file: " + filename).c_str(), result);

    fseek (fp, 0, SEEK_END);
    long int file_pos = ftell(fp);
    reqrv(file_pos, result);
    result.allocate(file_pos - 1);
    fseek(fp,0,SEEK_SET);

    if ( !fread(result.get_pointer(), sizeof(char), result.size(), fp) )
      VSX_ERROR_RETURN_V("Error reading file!", result);

    fclose(fp);
    return result;
  }

  inline void write(vsx_string<> filename, vsx_string<> string)
  {
    FILE* fp = fopen(filename.c_str(),"wb");
    if (!fp)
      VSX_ERROR_RETURN("fp is not valid");

    fwrite(string.get_pointer(), 1, string.size(), fp);
    fclose(fp);
  }

  inline void write(vsx_string<> filename, vsx_ma_vector<unsigned char> data)
  {
    FILE* fp = fopen(filename.c_str(),"wb");
    if (!fp)
      VSX_ERROR_RETURN("fp is not valid");
    fwrite(data.get_pointer(), 1, data.get_sizeof(), fp);
    fclose(fp);
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
