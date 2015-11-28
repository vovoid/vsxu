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

#include "vsx_texture_data_hint.h"

class vsx_texture_data
{
public:
  vsx_string<> filename;

  // graphics layer hints -----------------------------------------------------
  vsx_texture_data_hint hint;

  // geometrical / color ------------------------------------------------------
  unsigned int width = 0;
  unsigned int height = 0;
  unsigned int depth = false;
  bool alpha = false;
  unsigned int channels = 0;

  // storage ------------------------------------------------------------------
  enum channel_storage_type_t
  {
    byte_storage= 0,
    float_storage = 1
  } storage_format = byte_storage;

  void *data[6] = {0,0,0,0,0,0};
  volatile size_t data_ready = 0;
  bool compressed_data = false;


  // cache information --------------------------------------------------------
  bool attached_to_cache;
  int references = 0;

  vsx_texture_data(bool is_attached_to_cache)
    :
    attached_to_cache(is_attached_to_cache)
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
