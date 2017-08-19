/**
* Project: VSXu Engine: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu Engine.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Lesser General Public License (LGPL)
*
* VSXu Engine is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU Lesser General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef VSX_ARGVECTOR_H
#define VSX_ARGVECTOR_H


#include <vsx_platform.h>
#include <vsx_common_dllimport.h>

#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
  #include <math/vsx_math.h>
  #include <sys/types.h>
  #include <unistd.h>
  #include <stdlib.h>
#endif

#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
  #include <windows.h> // GetProcAddress/GetModuleFileName
  #include <vsx_unistd.h>
#endif

#include <container/vsx_nw_vector.h>
#include <string/vsx_string.h>
#include <string/vsx_string_helper.h>

class vsx_argvector
{
  vsx_nw_vector< vsx_string<> > data;
public:
  vsx_string<>& operator[](size_t index)
  {
    return data[index];
  }

  size_t size()
  {
    return data.size();
  }

  void push_back(const vsx_string<>& val)
  {
    data.push_back(val);
  }

  bool has_param(const vsx_string<>& param)
  {
    size_t data_num_elements = size();

    if (data_num_elements == 0)
      return false;

    for (size_t i = 0; i < data_num_elements; i++)
    {
      if (data[i] == "-"+param)
      {
        return true;
      }
    }
    return false;
  }

  bool has_param_with_value(const vsx_string<>& param)
  {
    size_t data_num_elements = size();

    if (data_num_elements == 0)
      return false;

    for (size_t i = 0; i < data_num_elements; i++)
    {
      if (data[i] == "-"+param)
      {
        if (i+1 < data_num_elements)
        {
          if (data[i+1][0] != '-')
          {
            return true;
          }
        }
      }
    }
    return false;
  }

  vsx_string<>get_param_value(const vsx_string<>& param)
  {
    size_t data_num_elements = size();
    if (data_num_elements == 0) return "";
    for (size_t i = 0; i < data_num_elements; i++)
    {
      if (data[i] == "-"+param)
      {
        if (i+1 < data_num_elements)
        {
          if (data[i+1][0] != '-')
          {
            return data[i+1];
          }
        }
      }
    }
    return "";
  }

  vsx_string<> get_param_subvalue(const vsx_string<>& param, size_t sub_value_index, vsx_string<> delimiters, vsx_string<> default_value)
  {
    if (!has_param_with_value(param))
      return default_value;

    vsx_string<> value_str = get_param_value(param);

    for (size_t del_i = 0; del_i < delimiters.size(); del_i++)
    {
      vsx_nw_vector< vsx_string<> > parts;
      vsx_string_helper::explode_single(value_str, delimiters[del_i], parts);
      if (parts.size() <= 1)
        continue;
      return parts[sub_value_index];
    }
    return default_value;
  }

  void init_from_argc_argv(int argc, char *argv[])
  {
    for (int i = 0; i < argc; i++)
    {
      push_back(argv[i]);
    }
  }

  void init_from_string(vsx_string<>new_string)
  {
    vsx_string<>res;
    new_string.trim_lf();

    for (size_t i = 0; i < new_string.size(); i++)
    {
      if (new_string[i] == ' ' && res.size())
      {
        data.push_back( res );
        res.clear();
      } else
      {
        res.push_back( new_string[i] );
      }
    }
    if (res.size())
    {
      data.push_back(res);
    }
  }

  vsx_string<> serialize()
  {
    vsx_string<>res;
    for (size_t i = 0; i < data.size(); i++)
    {
      if (i) res.push_back(' ');
      res += data[i];
    }
    return res;
  }

  static vsx_string<> get_executable_directory()
  {
    char pBuf[512];
    const size_t len = 512;

    #if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
      int bytes = GetModuleFileName(NULL, pBuf, len);
      if(bytes == 0)
        return "";
      else
        return vsx_string_helper::path_from_filename(vsx_string<>(pBuf), '\\');
    #else

      // if packed with UPX
      if (getenv("   "))
        return vsx_string_helper::path_from_filename(vsx_string<>(getenv("   ")));

      char szTmp[32];
      sprintf(szTmp, "/proc/self/exe");
      ssize_t bytes = MIN(readlink(szTmp, pBuf, len), (ssize_t)len - 1);

      if(bytes >= 0)
        pBuf[bytes] = '\0';

    #endif
    return vsx_string_helper::path_from_filename(vsx_string<>(pBuf));
  }

  static vsx_string<> get_home_directory()
  {
    #if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
    char* home_dir = getenv("HOME");
    #else
      #if COMPILER == COMPILER_VISUAL_STUDIO
        char* home_dir = std::getenv("USERPROFILE");
      #else
        char* home_dir = getenv("USERPROFILE");
      #endif
    #endif
    return vsx_string<>(home_dir);
  }

private:
  COMMON_DLLIMPORT static vsx_argvector instance;
public:
  static vsx_argvector* get_instance()
  {
    return &instance;
  }

  vsx_argvector() {}
};

#endif // VSX_ARGVECTOR_H
