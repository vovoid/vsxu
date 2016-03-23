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

#include <module/vsx_module.h>

#include <bitmap/vsx_bitmap.h>
#include <bitmap/generators/vsx_bitmap_generator_subplasma.h>

class module_bitmap_subplasma : public vsx_module
{
public:
  // in - function
  vsx_module_param_float* rand_seed_in;
  vsx_module_param_int* amplitude_in;

  // in - options
  vsx_module_param_int* size_in;

  // out
  vsx_module_param_bitmap* bitmap_out;

  // internal
  bool worker_running = false;
  vsx_bitmap* bitmap = 0x0;
  vsx_bitmap* old_bitmap = 0x0;


  void module_info(vsx_module_specification* info)
  {
    info->in_param_spec =
      "function:complex{"
        "rand_seed:float,"
        "amplitude:enum?2|4|8|16|32|64|128|256|512&nc=1"
      "},"
      "options:complex{"
        "size:enum?8x8|16x16|32x32|64x64|128x128|256x256|512x512|1024x1024&nc=1"
      "}"
    ;
    info->identifier = "bitmaps;generators;subplasma";
    info->out_param_spec = "bitmap:bitmap";
    info->component_class = "bitmap";
    info->description =
      "Generates a plasma bitmap\n"
      "Thanks to BoyC of Conspiracy \n"
      "for the base code of this!";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    // function
    rand_seed_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"rand_seed");
    rand_seed_in->set(4.0f);

    amplitude_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"amplitude");

    // options
    size_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"size");
    size_in->set(4);

    // out
    bitmap_out = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
  }


  void run()
  {
    if (bitmap && bitmap->data_ready)
    {
      bitmap_out->set(bitmap);
      loading_done = true;

      if (old_bitmap)
      {
        vsx_bitmap_cache::get_instance()->destroy(old_bitmap);
        old_bitmap = 0;
      }
      worker_running = false;
    }

    req(!worker_running);

    req( param_updates );
    param_updates = 0;

    if (bitmap)
    {
      old_bitmap = bitmap;
      bitmap = 0x0;
    }

    vsx_string<> cache_handle = vsx_bitmap_generator_subplasma::generate_cache_handle(
          (int)rand_seed_in->get(),
          amplitude_in->get(),
          size_in->get()
        );

    if (!bitmap)
      bitmap = vsx_bitmap_cache::get_instance()->
        aquire_create(cache_handle, 0);

    bitmap->filename = cache_handle;
    vsx_bitmap_generator_subplasma::generate_thread(
          bitmap,
          (int)rand_seed_in->get(),
          amplitude_in->get(),
          size_in->get()
    );
    worker_running = true;
  }

  void on_delete()
  {
    if (bitmap)
      vsx_bitmap_cache::get_instance()->destroy(bitmap);
  }
};
