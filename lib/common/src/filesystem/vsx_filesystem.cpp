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

#if defined(_WIN32)
  #include <io.h>
  #include <fcntl.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>


#include <string/vsx_string.h>
#include <string/vsx_string_helper.h>
#include <container/vsx_nw_vector.h>


#include "vsx_platform.h"
#include "vsx_version.h"
#include "debug/vsx_error.h"

#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
  #include <stdio.h>
  #include <stdlib.h>
  #include <string>
  #include <sys/stat.h>
  #include <unistd.h>
#endif


#include <filesystem/vsx_filesystem_helper.h>
#include <filesystem/vsx_filesystem.h>

namespace vsx
{

void filesystem::set_base_path(vsx_string<>new_base_path)
{
  base_path = new_base_path;
}


vsx_string<> filesystem::get_base_path()
{
  return base_path;
}


bool filesystem::is_file(const char* filename)
{
  vsx_string<> i_filename(filename);
  i_filename = remove_prefix(i_filename);

  if (archive.is_archive())
    return archive.is_file(i_filename);

  file *fp;
  fp = this->f_open(i_filename.c_str());
  if (fp == NULL)
  {
    return false;
  }
  this->f_close(fp);
  return true;
}


bool filesystem::is_file(const vsx_string<>filename)
{
  vsx_string<> i_filename = remove_prefix(filename);

  if (archive.is_archive())
    return archive.is_file(i_filename);

  file *fp;
  fp = this->f_open(i_filename.c_str());
  if (fp == NULL)
    return false;
  this->f_close(fp);
  return true;
}

size_t  filesystem::num_open_files = 0;

file* filesystem::f_open(const char* filename)
{
  vsx_string<> i_filename(filename);
  i_filename = remove_prefix(i_filename);


  reqrv(i_filename.size(), 0x0);

  file* handle = new file;
  handle->filename = i_filename;

  if (archive.is_archive())
  {
    archive.file_open(i_filename.c_str(), handle);
    return handle;
  }

  #if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
    i_filename = vsx_string_helper::str_replace<char>("\\","/",i_filename);
  #endif
  #if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
    i_filename = vsx_string_helper::str_replace<char>("/","\\",i_filename);
  #endif

  vsx_string<> target_filename = base_path + i_filename;
  handle->handle = fopen(target_filename.c_str(),"rb");
  if (!handle->handle)
  {
    if (errno == EMFILE)
    {
      vsx_printf(L"File open failed - Too many open files! - %hs, errno: %d\n", filename, errno);
      //for(auto it = open_files.begin(); it != open_files.end(); it++)
      //  if (it->second) // io handle open
      //    vsx_printf(L"  Open File: \"%hs\"\n", it->first.c_str());
    }

    if (errno != ENOENT)
      vsx_printf(L"File open failed - %hs, errno: %d\n", filename, errno);

    delete handle;
    handle = 0x0;
  }
  if (handle)
  {
    num_open_files++;
    //open_files[handle->filename] = true;
    //vsx_printf(L"open files: %d \"%hs\"\n", num_open_files, handle->filename.c_str());
  }
  return handle;
}

void filesystem::f_close(file* &handle)
{
  req(handle);

  num_open_files--;

  //open_files.erase(handle->filename);

  if (!archive.is_archive())
  {
    if (handle->handle)
      fclose(handle->handle);
    if (handle->data.size())
      free( handle->data.get_pointer() );
  }

  delete handle;
  handle = 0x0;
}

void filesystem::f_close_io(file* &handle)
{
  req(handle);
  req(!archive.is_archive());
  req(handle->handle);
  fclose(handle->handle);
  handle->handle = 0x0;
  //open_files[handle->filename] = false;
}

size_t filesystem::f_get_size(file* handle)
{
  if (archive.is_archive())
    return handle->size;

  reqrv(handle->handle,0);

  long prev_pos = ftell(handle->handle);
  fseek( handle->handle, 0, SEEK_END);
  long size = ftell(handle->handle);
  fseek( handle->handle, prev_pos, SEEK_SET);
  return size;
}

unsigned char* filesystem::f_data_get(file* handle)
{
  if (archive.is_archive())
    if (handle->data.size())
      return handle->data.get_pointer();

  size_t size = f_get_size(handle);
  unsigned char* buf = (unsigned char*)malloc(size);
  reqrv(buf, 0x0);
  f_read((void*)buf, size, handle);
  handle->data.set_volatile();
  handle->data.set_data( buf, size );
  return buf;
}

char* filesystem::f_gets_entire(file* handle)
{
  if (handle->data.size())
  {
    char* string = (char*)malloc(handle->data.size() + 1);
    memcpy(string, (char*)handle->data.get_pointer(), handle->data.size());
    string[handle->data.size()] = 0;
    return string;
  }

  size_t size = f_get_size(handle);
  char* buf = (char*)malloc(size+1);
  reqrv(buf, 0x0);
  f_read((void*)buf, size, handle);
  buf[size] = 0;
  return buf;
}

char* filesystem::f_gets(char* buf, unsigned long max_buf_size, file* handle)
{
  if (!archive.is_archive())
  {
    reqrv(handle->handle, 0);
    return fgets(buf, max_buf_size, handle->handle);
  }

  unsigned long i = 0;
  bool run = true;

  while (i < max_buf_size && run)
  {
    if (handle->position == handle->data.size())
      break;

    if (handle->data[handle->position] == 0x0A)
      run = false;

    buf[i] = handle->data[handle->position];
    i++;
    handle->position++;
  }

  if (i < max_buf_size)
    buf[i] = 0;

  if (i != 0)
    return buf;

  return 0x0;
}

size_t filesystem::f_read(void* buf, size_t num_bytes, file* handle)
{
  reqrv(handle->handle, 0);

  if (!handle->data.size())
    return fread(buf, 1, num_bytes, handle->handle);

  if (handle->position + num_bytes > handle->size)
    num_bytes = handle->size - handle->position;

  memcpy(buf, handle->data.get_pointer() + handle->position, num_bytes);
  handle->position += num_bytes;
  return num_bytes;
}

}


