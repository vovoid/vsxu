#include <filesystem/vsx_filesystem_helper.h>
#include <sys/types.h>
#include <dirent.h>
#include <filesystem/vsx_filesystem.h>
#include <string/vsx_string_helper.h>

namespace vsx
{
namespace filesystem_helper
{

#ifndef _WIN32
void get_files_recursive_(vsx_string<>startpos, std::list< vsx_string<> >* filenames, vsx_string<>include_filter, vsx_string<>exclude_filter, vsx_string<>dir_ignore_token);

void get_files_recursive(vsx_string<>startpos, std::list< vsx_string<> >* filenames, vsx_string<>include_filter, vsx_string<>exclude_filter, vsx_string<>dir_ignore_token) {
  get_files_recursive_(startpos,filenames,include_filter,exclude_filter,dir_ignore_token);
  (*filenames).sort();
}

void get_files_recursive_(vsx_string<>startpos, std::list< vsx_string<> >* filenames,vsx_string<>include_filter,vsx_string<>exclude_filter,vsx_string<>dir_ignore_token)
{
#else
void get_files_recursive(vsx_string<>startpos, std::list< vsx_string<> >* filenames,vsx_string<>include_filter,vsx_string<>exclude_filter,vsx_string<>dir_ignore_token)
{
#endif

#ifdef _WIN32
  _finddata_t fdp;
#else
  DIR* dir;
  dirent* dp;
#endif

  vsx::filesystem filesystem;

  long fhandle = 0;
  bool run = true;

  #ifdef _WIN32
    vsx_string<> fstring = startpos+"/*";
    fhandle = _findfirst(fstring.c_str(),&fdp);
  #else
    vsx_string<>fstring = startpos;
    dir = opendir(startpos.c_str());
    if (!dir)
      return;

    dp = readdir(dir);

    if (!dp)
      return;
  #endif

  bool exclude = false;
  bool include = true;
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
    else
    {
      vsx_nw_vector< vsx_string<> > parts;
      vsx_string<>deli = " ";
      vsx_string_helper::explode(exclude_filter,deli, parts);
      exclude = false;
      unsigned long i = 0;
      while (i < parts.size() && !exclude)
      {
        if (cur_directory_item.find(parts[i]) != -1)
          exclude = true;
        ++i;
      }
    }

    if ((cur_directory_item != ".") && (cur_directory_item != "..") && !exclude)
    {
#ifdef _WIN32
      if (fdp.attrib & _A_SUBDIR)
#else
      if (S_ISDIR(stbuf.st_mode) || S_ISLNK(stbuf.st_mode))
#endif
      {
        if (!filesystem.is_file( startpos + "/" + cur_directory_item + "/" + dir_ignore_token) )
          get_files_recursive(startpos+"/"+cur_directory_item,filenames,include_filter,exclude_filter,dir_ignore_token);
      }
      else
      {
        if (include)
          filenames->push_back(startpos+"/"+cur_directory_item);
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
}

}
}
