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
#include <tools/vsx_req.h>

typedef uint32_t vsx_bitmap_32bt;

class vsx_bitmap
{

  // data holder
  // mip map level by cube map side
  // if not a cube map, only the first is used.

  // If all 15 levels used, original image is 8192x8192 which is the max
  // size currently. If needed, more levels can be added.

  // So for instance - a 2D non-mipmapped image will put data in
  //    data[0][0]

  // A mipmapped 2D image will upt data in
  //    data[0][0]
  //    data[1][0]
  //    data[2][0]
  //    etc...

  void *data [15] [6] =
    {
      {0,0,0,0,0,0},
      {0,0,0,0,0,0},
      {0,0,0,0,0,0},
      {0,0,0,0,0,0},
      {0,0,0,0,0,0},
      {0,0,0,0,0,0},
      {0,0,0,0,0,0},
      {0,0,0,0,0,0},
      {0,0,0,0,0,0},
      {0,0,0,0,0,0},
      {0,0,0,0,0,0},
      {0,0,0,0,0,0},
      {0,0,0,0,0,0},
      {0,0,0,0,0,0},
      {0,0,0,0,0,0}
    }
  ;

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

  // geometrical / color
  unsigned int width = 0;
  unsigned int height = 0;
  unsigned int depth = false;
  bool alpha = false;
  unsigned int channels = 4;
  bool channels_bgra = false;

  // storage
  enum channel_storage_format
  {
    byte_storage= 0,
    float_storage = 1
  };

  channel_storage_format storage_format = byte_storage;

  static const size_t mip_map_level_max = 15;


  // has thread finished producing data when loading?
  volatile size_t data_ready = 0;

  enum compression_type {
    compression_none = 0,
    compression_dxt1 = 1,
    compression_dxt3 = 3,
    compression_dxt5 = 5
  };

  compression_type compression = compression_none;

  // cache information
  bool attached_to_cache;
  int references = 0;

  inline void* data_get(size_t mip_map_level = 0, size_t cube_map_side = 0)
  {
    return data[mip_map_level][cube_map_side];
  }

  inline void data_set(void* new_value, size_t mip_map_level = 0, size_t cube_map_side = 0)
  {
    data[mip_map_level][cube_map_side] = new_value;
  }

  inline void data_free(size_t mip_map_level = 0, size_t cube_map_side = 0)
  {
    req(data[mip_map_level][cube_map_side]);
    free (data[mip_map_level][cube_map_side]);
    data[mip_map_level][cube_map_side] = 0x0;
  }

  inline void data_free_all() {
    for (size_t mipmap_level = 0; mipmap_level < 15; mipmap_level++)
      for (size_t i = 0; i < 6; i++)
      {
        if (!data[mipmap_level][i])
          continue;
        free(data[mipmap_level][i]);
        data[mipmap_level][i] = 0;
      }
  }

  vsx_bitmap(bool is_attached_to_cache = false)
    :
    attached_to_cache(is_attached_to_cache)
  {
  }

  ~vsx_bitmap()
  {
    data_free_all();
  }


  bool get_mipmap_level_count()
  {
    size_t mip_map_level_count = 0;
    for (size_t mip_map_level = 0; mip_map_level < mip_map_level_max; mip_map_level++)
      if (data[mip_map_level][0])
        mip_map_level_count++;
    return mip_map_level_count;
  }

  bool is_valid()
  {
    if (!width)
      return false;
    if (!height)
      return false;
    if (!data[0][0])
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
