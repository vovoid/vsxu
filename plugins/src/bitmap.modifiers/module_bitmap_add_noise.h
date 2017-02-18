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


#include <bitmap/vsx_bitmap.h>
#include <texture/vsx_texture.h>

class module_bitmap_add_noise : public vsx_module
{
  // in
  vsx_module_param_bitmap* bitmap_in;

  // out
  vsx_module_param_bitmap* bitmap_out;

  // internal
  float time;
  vsx_bitmap* source_bitmap;
  vsx_bitmap bitmap;

  int buf = 0;
  int frame = 0;
  void* data_a = 0x0;
  void* data_b = 0x0;
  int bitm_timestamp;
  std::atomic_uint_fast8_t worker_run;
  std::thread         worker_thread;

public:

  module_bitmap_add_noise()
  {
    worker_run = 0;
  }

  void noise_worker()
  {
    int i_frame = -1;
    bool buf = false;
    uint32_t *p;
    while (worker_run.load())
    {
      if (i_frame == frame)
      {
        Sleep(100);
        continue;
      }

      if (buf)
        p = (uint32_t*)data_a;
      else
        p = (uint32_t*)data_b;

      unsigned long b_c = bitmap.width * bitmap.height;

      if (bitmap.storage_format == vsx_bitmap::byte_storage && bitmap.channels == 4)
      {
        for (size_t x = 0; x < b_c; ++x)
        {
            p[x] = ((uint32_t*)bitmap.data_get() )[x] | rand() << 8  | (unsigned char)rand(); //bitm->data[x + y*result_bitm.size_x]
        }
      }
      bitmap.data_set( p );
      bitmap.timestamp++;
      buf = !buf;
      i_frame = frame;
    }
  }

  void module_info(vsx_module_specification* info)
  {
    info->identifier = "bitmaps;modifiers;add_noise";
    info->in_param_spec = "bitmap_in:bitmap";
    info->out_param_spec = "bitmap:bitmap";
    info->component_class = "bitmap";
    info->description = "Adds noise to the bitmap.";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    bitmap_in = (vsx_module_param_bitmap*)in_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap_in");
    bitmap_out = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
  }

  void run()
  {
    source_bitmap = bitmap_in->get();
    if (!source_bitmap)
    {
      if (worker_run.load())
        worker_run.fetch_sub(1);
      worker_thread.join();
      bitmap_out->valid = false;
      return;
    }

    bitmap.copy_information_from( *source_bitmap );
    ++frame;

    if (bitmap.width != source_bitmap->width && bitmap.height != source_bitmap->height)
    {
      if (worker_run.load())
      {
        worker_run.fetch_sub(1);
        worker_thread.join();
      }

      if (data_a)
      {
        free( data_a );
        free( data_b );
      }

      data_a = malloc( sizeof(uint32_t) * source_bitmap->width * source_bitmap->height);
      data_b = malloc( sizeof(uint32_t) * source_bitmap->width * source_bitmap->height);

      bitmap.data_set( data_a );
      bitmap.width = source_bitmap->width;
      bitmap.height = source_bitmap->height;

      worker_run.fetch_add(1);
      worker_thread = std::thread( [this](){noise_worker();} );
    }

    bitmap_out->set(&bitmap);
  }

  void on_delete()
  {
    if (worker_run)
    {
      worker_run = false;
      bitmap_out->valid = false;

    }
    free(data_a);
    free(data_b);
  }

};


