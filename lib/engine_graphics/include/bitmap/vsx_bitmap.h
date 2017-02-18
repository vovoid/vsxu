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


#pragma once

#include <stdlib.h>
#include <string/vsx_string.h>
#include <tools/vsx_req.h>
#include <tools/vsx_foreach.h>
#include <tools/vsx_lock.h>

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

  uint64_t data_size [15] [6] =
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

  bool data_volatile = false;

public:
  vsx_string<> filename;

  // timestamp for different engine modules to keep track of when to invalidate themselves
  uint64_t timestamp = 0;

  // make sure this is not destroyed while a thread is working on it
  vsx_lock lock;

  // loader hint
  enum loader_hint {
      no_hint = 0,
      flip_vertical_hint = 1,
      cubemap_split_6_1_hint = 2,
      cubemap_load_files_hint = 4,
      cubemap_sphere_map = 8
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
  std::atomic_uint_fast64_t data_ready;

  enum compression_type {
    compression_none = 0,
    compression_dxt1 = 1,
    compression_dxt3 = 3,
    compression_dxt5 = 5
  };

  compression_type compression = compression_none;


  void copy_information_from(const vsx_bitmap& other)
  {
    width = other.width;
    height = other.height;
    depth = other.depth;
    alpha = other.alpha;
    channels = other.channels;
    channels_bgra = other.channels_bgra;
    storage_format = other.storage_format;
    compression = other.compression;
  }

  // cache information
  bool attached_to_cache = false;
  int references = 0;

  inline void* data_get(size_t mip_map_level = 0, size_t cube_map_side = 0)
  {
    return data[mip_map_level][cube_map_side];
  }

  inline uint64_t data_size_get(size_t mip_map_level = 0, size_t cube_map_side = 0)
  {
    return data_size[mip_map_level][cube_map_side];
  }

  inline uint64_t data_size_get_all()
  {
    uint64_t total_bytes;
    for (size_t mipmap_level = 0; mipmap_level < 15; mipmap_level++)
      for (size_t cubemap_side = 0; cubemap_side < 6; cubemap_side++)
        total_bytes += data_size[mipmap_level][cubemap_side];
    return total_bytes;
  }

  inline void data_mark_volatile()
  {
    data_volatile = true;
  }

  inline void data_set(void* new_value, size_t mip_map_level = 0, size_t cube_map_side = 0, uint64_t size = 0)
  {
    data[mip_map_level][cube_map_side] = new_value;
    data_size[mip_map_level][cube_map_side] = size;
  }

  inline void data_allocate(size_t mip_map_level, size_t cube_map_side, int width, int height, channel_storage_format format, unsigned int channels)
  {
    if (format == channel_storage_format::byte_storage)
    {
      size_t size_bytes = width * height * channels;
      data_set( malloc(size_bytes), mip_map_level, cube_map_side );
    }

    if (format == channel_storage_format::float_storage)
    {
      size_t size_bytes = sizeof(float) * width * height * channels;
      data_set( malloc(size_bytes), mip_map_level, cube_map_side );
    }
  }

  inline void data_free(size_t mip_map_level = 0, size_t cube_map_side = 0)
  {
    req(data[mip_map_level][cube_map_side]);
    req(!data_volatile);
    lock.aquire();
    free(data[mip_map_level][cube_map_side]);
    data[mip_map_level][cube_map_side] = 0x0;
    data_size[mip_map_level][cube_map_side] = 0;
    lock.release();
  }

  inline void data_free_all()
  {
    req(!data_volatile);
    for (size_t mipmap_level = 0; mipmap_level < 15; mipmap_level++)
      for (size_t cubemap_side = 0; cubemap_side < 6; cubemap_side++)
        data_free(mipmap_level, cubemap_side);
  }

  inline size_t sides_count_get()
  {
    for_n(i, 0, 6)
      if (!data[0][i])
        return i;
    return 6;
  }

  vsx_bitmap()
  {
    data_ready = 0;
  }

  ~vsx_bitmap()
  {
    data_free_all();
  }


  size_t get_mipmap_level_count()
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
