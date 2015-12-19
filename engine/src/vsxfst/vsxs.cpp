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


#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <io.h>
#endif

#if defined(__linux__) || defined(__APPLE__)
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#endif

#include <debug/vsx_error.h>
#include <string/vsx_string_helper.h>
#include "vsxfst.h"

#ifdef VSX_PRINTF_TO_FILE

vsx_pf_file_holder vsx_pf_file_holder::instance;

#endif




























#ifndef _WIN32
void get_files_recursive_(vsx_string<>startpos, std::list< vsx_string<> >* filenames,vsx_string<>include_filter,vsx_string<>exclude_filter,vsx_string<>dir_ignore_token);
void get_files_recursive(vsx_string<>startpos, std::list< vsx_string<> >* filenames,vsx_string<>include_filter,vsx_string<>exclude_filter,vsx_string<>dir_ignore_token)
{
  get_files_recursive_(startpos,filenames,include_filter,exclude_filter,dir_ignore_token);
  (*filenames).sort();
}
void get_files_recursive_(vsx_string<>startpos, std::list< vsx_string<> >* filenames,vsx_string<>include_filter,vsx_string<>exclude_filter,vsx_string<>dir_ignore_token)
{
#else
void get_files_recursive(vsx_string<>startpos, std::list< vsx_string<> >* filenames,vsx_string<>include_filter,vsx_string<>exclude_filter,vsx_string<>dir_ignore_token) {
#endif

#ifdef _WIN32
  _finddata_t fdp;
#else
  DIR* dir;
  dirent* dp;
#endif

  vsxf filesystem;

  long fhandle = 0;
  bool run = true;
#ifdef _WIN32
  vsx_string<>fstring = startpos+"/*";
  fhandle=_findfirst(fstring.c_str(),&fdp);
#else
  vsx_string<>fstring = startpos;
  dir = opendir(startpos.c_str());
  if (!dir) return;
  dp = readdir(dir);
  if (!dp) return;
#endif
  bool exclude = false;
  bool include = true;
#ifdef VSXS_DEBUG
  FILE* fp = fopen("get_files_recursive.log","a");
  fprintf(fp,"Starting traversion in : %s\n",fstring.c_str());
#endif
  if (fhandle != -1)
  while (run)
  {
    vsx_string<>cur_directory_item;
#ifdef _WIN32
    cur_directory_item = fdp.name;
#else
    cur_directory_item = dp->d_name;
    // stat the file to see if it's a dir or not
    struct stat stbuf;
    vsx_string<>full_path = fstring + "/" + cur_directory_item;
    stat(full_path.c_str(), &stbuf);
#endif
#ifdef VSXS_DEBUG
    fprintf(fp,"File found: %s\n",full_path.c_str());
#endif
    if (include_filter == "")
      include = true;
    else
    {
#ifdef _WIN32
      if (!(fdp.attrib & _A_SUBDIR))
#else
      if (!S_ISDIR(stbuf.st_mode) && !S_ISLNK(stbuf.st_mode))
#endif
        include = false;
        if (cur_directory_item.find(include_filter) != -1)
          include = true;
    }

    if (exclude_filter == "")
    {
      exclude = false;
    }
    else {
      vsx_nw_vector< vsx_string<> > parts;
      vsx_string<>deli = " ";
      vsx_string_helper::explode(exclude_filter,deli, parts);
      exclude = false;
      unsigned long i = 0;
      while (i < parts.size() && !exclude)
      {
        if (cur_directory_item.find(parts[i]) != -1) exclude = true;
        ++i;
      }
    }

    if ((cur_directory_item != ".") && (cur_directory_item != "..") && !exclude)
    {
      #ifdef VSXS_DEBUG
        fprintf(fp,"ss = %s  ___ include %d exclude %d  link: %d \n",ss.c_str(),include,exclude,(int)(stbuf.st_mode));
        fprintf(fp,"S_ISREG: %d\n", S_ISREG(stbuf.st_mode) );
        fprintf(fp,"S_ISDIR: %d\n", S_ISDIR(stbuf.st_mode) );
      #endif
#ifdef _WIN32
      if (fdp.attrib & _A_SUBDIR)
#else
      if (S_ISDIR(stbuf.st_mode) || S_ISLNK(stbuf.st_mode))
#endif
      {
        if (!filesystem.is_file( startpos + "/" + cur_directory_item + "/" + dir_ignore_token) )
        {
          // recurse into the subdirectory
          get_files_recursive(startpos+"/"+cur_directory_item,filenames,include_filter,exclude_filter,dir_ignore_token);
        }
      } else {
        if (include)
        {
          filenames->push_back(startpos+"/"+cur_directory_item);
        }
      }
    }
#ifdef _WIN32
    if (_findnext(fhandle,&fdp) == -1) run = false;
#else
  dp = readdir(dir);
    if (dp == 0)
      run = false;
#endif
  }
#ifdef _WIN32
  _findclose(fhandle);
#else
  closedir(dir);
#endif
  #ifdef VSXS_DEBUG
  fclose(fp);
  #endif
}

