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
  if (archive.is_archive())
    return archive.is_file(vsx_string<>(filename));

  file *fp;
  fp = this->f_open(filename, "r");
  if (fp == NULL)
  {
    return false;
  }
  this->f_close(fp);
  return true;
}


bool filesystem::is_file(const vsx_string<>filename)
{

  if (archive.is_archive())
    return archive.is_file(filename);

  file *fp;
  fp = this->f_open(filename.c_str(), "r");
  if (fp == NULL)
    return false;
  this->f_close(fp);
  return true;
}



file* filesystem::f_open(const char* filename, const char* mode)
{
  vsx_string<> i_filename(filename);

  req_v(i_filename.size(), 0x0);

  file* handle = new file;

  if (archive.is_archive())
  {
    archive.file_open(filename, mode, handle);
    return handle;
  }

  #if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
    i_filename = vsx_string_helper::str_replace<char>("\\","/",i_filename);
  #endif
  #if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
    i_filename = vsx_string_helper::str_replace<char>("/","\\",i_filename);
  #endif

  handle->handle = fopen((base_path+i_filename).c_str(),mode);
  if (!handle->handle)
  {
    delete handle;
    handle = 0x0;
  }
  return handle;
}

void filesystem::f_close(file* &handle)
{
  if (!handle)
    return;

  if (archive.is_archive())
    archive.file_close(handle);
  else
    fclose(handle->handle);

  delete handle;
  handle = 0x0;
}

int filesystem::f_puts(const char* buf, file* handle)
{
  req_v(handle, 0);

  if (!archive.is_archive())
    return fputs(buf,handle->handle);

  req_v(handle->mode == file::mode_write, 0);

  size_t i = 0;
  while (buf[i])
    handle->data[handle->position = handle->size++] = buf[i++];

  return 0;
}

unsigned long filesystem::f_get_size(file* handle)
{
  if (archive.is_archive())
    return handle->size;

  long prev_pos = ftell(handle->handle);
  fseek( handle->handle, 0, SEEK_END);
  long size = ftell(handle->handle);
  fseek( handle->handle, prev_pos, SEEK_SET);
  return size;
}

char* filesystem::f_gets_entire(file* handle)
{
  if (handle->data.size())
    return (char*)handle->data.get_pointer();

  unsigned long size = f_get_size(handle);
  char* buf = (char*)malloc(size+1);
  req_v(buf, 0x0);
  f_read((void*)buf, size, handle);
  buf[size] = 0;
  return buf;
}

char* filesystem::f_gets(char* buf, unsigned long max_buf_size, file* handle)
{
  if (!archive.is_archive())
    return fgets(buf, max_buf_size, handle->handle);

  unsigned long i = 0;
  bool run = true;

  while (handle->position < handle->size && i < max_buf_size && run)
  {
    if (handle->data[handle->position] == 0x0A)
      run = false;

    buf[i] = handle->data[handle->position];
    ++i;
    ++handle->position;
  }

  if (i < max_buf_size)
    buf[i] = 0;

  if (i != 0)
    return buf;

  return 0x0;
}

wchar_t* filesystem::f_getws(wchar_t* buf, unsigned long max_buf_size, file* handle)
{
  if (!archive.is_archive())
  {
    wchar_t* ret = fgetws(buf, max_buf_size, handle->handle);
    if (!ret)
      perror("Error reading file");
    return ret;
  }

  unsigned long i = 0;
  bool run = true;

  while (handle->position < handle->size && i < max_buf_size && run)
  {
    // TODO: this is wrong
    if (handle->data[handle->position] == 0x0A)
    {
      run = false;
    }
    buf[i] = handle->data[handle->position];
    ++i;
    ++handle->position;
  }

  if (i < max_buf_size)
    buf[i] = 0;

  if (i != 0)
    return buf;

  return 0x0;
}

int filesystem::f_read(void* buf, unsigned long num_bytes, file* handle)
{
  if (!handle->data.size())
    return fread(buf, 1, num_bytes, handle->handle);

  if (handle->position + num_bytes > handle->size)
    num_bytes = handle->size - handle->position;

  memcpy(buf, handle->data.get_pointer() + handle->position, num_bytes);
  handle->position += num_bytes;
  return num_bytes;
}

}


