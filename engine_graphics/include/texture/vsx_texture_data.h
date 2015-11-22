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


#ifndef VSX_TEXTURE_DATA_H
#define VSX_TEXTURE_DATA_H

#include <stdlib.h>
#define VSX_TEXTURE_DATA_TYPE_2D 1
#define VSX_TEXTURE_DATA_TYPE_CUBE 2

class vsx_texture_data
{
public:

  size_t type;

  unsigned int width;
  unsigned int height;
  unsigned int depth;
  bool alpha;
  unsigned int channels;
  bool mipmaps;

  enum channel_storage_type_t
  {
    byte_storage= 0,
    float_storage = 1,
  } storage_format;

  bool compressed_data;


  void *data[6];
  volatile size_t data_ready;

  bool attached_to_cache; // not part of cache
  int references; // references in cache

  vsx_texture_data(size_t n_type, bool is_attached_to_cache)
    :
    type(n_type),
    width(0),
    height(0),
    depth(false),
    alpha(false),
    channels(0),
    mipmaps(0),
    storage_format(byte_storage),
    compressed_data(false),
    data{0,0,0,0,0,0},
    data_ready(0),
    attached_to_cache(is_attached_to_cache),
    references(0)
  {
  }

  ~vsx_texture_data()
  {
    if (data[0])
      free(data[0]);
    if (data[1])
      free(data[1]);
    if (data[2])
      free(data[2]);
    if (data[3])
      free(data[3]);
    if (data[4])
      free(data[4]);
    if (data[5])
      free(data[5]);
  }
};


#endif
