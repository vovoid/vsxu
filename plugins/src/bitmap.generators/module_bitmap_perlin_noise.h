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

#include <inttypes.h>
#include <bitmap/vsx_bitmap.h>
#include <bitmap/generators/vsx_bitmap_generator_perlin_noise.h>


class module_bitmap_perlin_noise : public vsx_module
{
public:
  // in - function
  vsx_module_param_float* rand_seed_in;
  vsx_module_param_float* perlin_strength_in;
  vsx_module_param_int* octave_in;
  vsx_module_param_int* frequency_in;

  // in - function_blob
  vsx_module_param_int* blob_enable_in;
  vsx_module_param_float* blob_arms_in;
  vsx_module_param_float* blob_attenuation_in;
  vsx_module_param_float* blob_star_flower_in;
  vsx_module_param_float* blob_angle_in;

  // in - options
  vsx_module_param_float4* color_in;
  vsx_module_param_int* alpha_in;
  vsx_module_param_int* bitmap_type_in;
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
        "function:complex"
        "{"
          "rand_seed:float,"
          "perlin_strength:float,"
          "octave:enum?1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16&nc=1,"
          "frequency:enum?1|2|3|4|5|6|7|8&nc=1,"
        "},"
        "function_blob:complex"
        "{"
          "enable_blob:enum?no|yes&nc=1,"
          "arms:float,"
          "attenuation:float,"
          "star_flower:float,"
          "angle:float"
        "},"
        "options:complex{"
          "color:float4?default_controller=controller_col,"
          "alpha:enum?no|yes&nc=1,"
          "size:enum?8x8|16x16|32x32|64x64|128x128|256x256|512x512|1024x1024|2048x2048&nc=1,"
          "bitmap_type:enum?integer|float&nc=1"
        "}"
      ;
    info->identifier = "bitmaps;generators;perlin_noise";
    info->out_param_spec = "bitmap:bitmap";
    info->component_class = "bitmap";
    info->description = "Perlin Noise (clouds) generator";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    // function
    rand_seed_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"rand_seed");
    rand_seed_in->set(4.0f);

    perlin_strength_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"perlin_strength");
    perlin_strength_in->set(1.0f);

    octave_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"octave");
    octave_in->set(0);

    frequency_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"frequency");
    frequency_in->set(0);

    // function blob
    blob_enable_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"enable_blob");
    blob_arms_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"arms");
    blob_attenuation_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"attenuation");
    blob_star_flower_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"star_flower");
    blob_angle_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"angle");

    // options
    color_in = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"color");
    color_in->set(1.0f, 0);
    color_in->set(1.0f, 1);
    color_in->set(1.0f, 2);
    color_in->set(1.0f, 3);

    alpha_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"alpha");

    size_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"size");
    size_in->set(4);

    bitmap_type_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"bitmap_type");

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

    vsx_string<> cache_handle = vsx_bitmap_generator_perlin_noise::generate_cache_handle(
        blob_enable_in->get() > 0,
        blob_arms_in->get(),
        blob_attenuation_in->get(),
        blob_star_flower_in->get(),
        blob_angle_in->get(),
        (int)rand_seed_in->get(),
        (int)octave_in->get(),
        (int)frequency_in->get(),
        perlin_strength_in->get(),
        alpha_in->get() > 0,
        vsx_color<>(color_in->get(0),color_in->get(1),color_in->get(2),color_in->get(3)),
        bitmap_type_in->get() != 0,
        size_in->get()
      );

    if (!bitmap)
      bitmap = vsx_bitmap_cache::get_instance()->
        aquire_create(cache_handle, 0);

    bitmap->filename = cache_handle;
    vsx_bitmap_generator_perlin_noise::generate_thread(
        bitmap,
        blob_enable_in->get() > 0,
        blob_arms_in->get(),
        blob_attenuation_in->get(),
        blob_star_flower_in->get(),
        blob_angle_in->get(),
        (int)rand_seed_in->get(),
        (int)octave_in->get(),
        (int)frequency_in->get(),
        perlin_strength_in->get(),
        alpha_in->get() > 0,
        vsx_color<>(color_in->get(0),color_in->get(1),color_in->get(2),color_in->get(3)),
        bitmap_type_in->get() != 0,
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
