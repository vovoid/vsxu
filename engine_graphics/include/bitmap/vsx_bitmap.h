/**
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
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


#ifndef VSX_BITMAP_H
#define VSX_BITMAP_H

#include <stdlib.h>

typedef uint32_t vsx_bitmap_32bt;

class vsx_bitmap
{
public:
  vsx_string<> filename;

  // timestamp for different engine modules to keep track of when to invalidate themselves
  int timestamp = 0;

  // loader hint
  enum loader_hint {
      no_hint = 0,
      flip_vertical_hint = 1,
      cubemap_split_6_1_hint = 2,
      cubemap_load_files_hint = 4
    };

  uint64_t hint = 0;
  bool reload_hint = false;

  //vsx_bitmap_loader_hint hint;

  // geometrical / color
  unsigned int width = 0;
  unsigned int height = 0;
  unsigned int depth = false;
  bool alpha = false;
  unsigned int channels = 4;
  bool channels_bgra = false;

  // storage
  enum channel_storage_type_t
  {
    byte_storage= 0,
    float_storage = 1
  } storage_format = byte_storage;

  // data holder
  void *data[6] = {0,0,0,0,0,0};

  // has thread finished producing data when loading?
  volatile size_t data_ready = 0;

  // is data DXT5 compressed?
  bool compressed_data = false;

  // cache information
  bool attached_to_cache;
  int references = 0;


  vsx_bitmap(bool is_attached_to_cache = false)
    :
    attached_to_cache(is_attached_to_cache)
  {
  }

  ~vsx_bitmap()
  {
    free_data();
  }

  void free_data() {
    for (size_t i = 0; i < 6; i++) {
      if (!data[i])
        continue;
      free(data[i]);
      data[i] = 0;
    }
  }

  bool is_valid()
  {
    if (!width)
      return false;
    if (!height)
      return false;
    if (!data[0])
      return false;
    return true;
  }

  size_t get_channel_size()
  {
    if (storage_format == byte_storage)
      return channels;
    return sizeof(float) * channels;
  }

};


#endif
