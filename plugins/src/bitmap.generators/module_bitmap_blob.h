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

#include <vsx_module.h>
#include <vsx_param.h>

#include <vsx_module_cache_helper.h>
#include <bitmap/vsx_bitmap.h>
#include <texture/vsx_texture.h>
#include <bitmap/generators/vsx_bitmap_generator_blob.h>

class module_bitmap_generators_blob : public vsx_module
{
  // in
  vsx_module_param_float* arms_in;
  vsx_module_param_float* attenuation_in;
  vsx_module_param_float* star_flower_in;
  vsx_module_param_float* angle_in;
  vsx_module_param_float4* color_in;
  vsx_module_param_int* alpha_in;
  vsx_module_param_int* size_in;

	// out
  vsx_module_param_bitmap* bitmap_out;

	// internal
  bool worker_running = false;
  vsx_bitmap* bitmap = 0x0;
  vsx_bitmap* old_bitmap = 0x0;
  float arms_cache = 0.0f;
  float attenuation_cache = 0.1f;
  float star_flower_cache = 0.0f;
  float angle_cache = 0.0f;
  float color_r_cache = 1.0f;
  float color_g_cache = 1.0f;
  float color_b_cache = 1.0f;
  float color_a_cache = 1.0f;
  int size_cache = 4;
  int alpha_cache = 0;

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier =
      "bitmaps;generators;blob||bitmaps;generators;particles;blob";

    info->description =
      "Generates blobs,stars or leaf\ndepending on parameters.\nPlay with the params :)";

    info->in_param_spec = ""
      "settings:complex{"
        "arms:float,"
        "attenuation:float,"
        "star_flower:float,"
        "angle:float,"
        "color:float4?default_controller=controller_col,"
        "alpha:enum?no|yes"
      "},"
      "size:enum?8x8|16x16|32x32|64x64|128x128|256x256|512x512|1024x1024|2048x2048"
      ;

    info->out_param_spec =
      "bitmap:bitmap";

    info->component_class =
      "bitmap";
  }


  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    arms_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"arms");
    arms_in->set(0.0f);

    attenuation_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"attenuation");
    attenuation_in->set(attenuation_cache);

    size_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"size");
    size_in->set(size_cache);

    alpha_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"alpha");
    alpha_in->set(alpha_cache);

    color_in = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"color");
    color_in->set(color_r_cache,0);
    color_in->set(color_g_cache,1);
    color_in->set(color_b_cache,2);
    color_in->set(color_a_cache,3);

    star_flower_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"star_flower");

    angle_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"angle");

    bitmap_out = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
  }

  bool has_state_changed()
  {
    cache_check_f(arms, 0.01f)
    cache_check_f(attenuation, 0.01f)
    cache_check(size)
    cache_check(alpha)
    cache_check_f(star_flower, 0.01f)
    cache_check_f(angle, 0.01f)

    if
    (
      (fabs(color_in->get(0) - color_r_cache) > 0.001f)
      ||
      (fabs(color_in->get(1) - color_g_cache) > 0.001f)
      ||
      (fabs(color_in->get(2) - color_b_cache) > 0.001f)
      ||
      (fabs(color_in->get(3) - color_a_cache) > 0.001f)
    )
    {
      color_r_cache = color_in->get(0);
      color_g_cache = color_in->get(1);
      color_b_cache = color_in->get(2);
      color_a_cache = color_in->get(3);
      return true;
    }

    return false;
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

    req( has_state_changed() );

    arms_cache = arms_in->get();
    attenuation_cache = attenuation_in->get();
    star_flower_cache = star_flower_in->get();
    angle_cache = angle_in->get();
    color_r_cache = color_in->get(0);
    color_g_cache = color_in->get(1);
    color_b_cache = color_in->get(2);
    color_a_cache = color_in->get(3);

    if (bitmap)
    {
      old_bitmap = bitmap;
      bitmap = 0x0;
    }

    vsx_string<> cache_handle = vsx_bitmap_generator_blob::generate_cache_handle(
        arms_cache,
        attenuation_cache,
        star_flower_cache,
        angle_cache,
        vsx_color<>(color_r_cache, color_g_cache, color_b_cache, color_a_cache),
        (bool)alpha_in->get(),
        size_in->get()
      );

    if (!bitmap)
      bitmap = vsx_bitmap_cache::get_instance()->
        aquire_create(cache_handle, 0);

    bitmap->filename = cache_handle;
    vsx_bitmap_generator_blob::load(
      bitmap,
      arms_cache,
      attenuation_cache,
      star_flower_cache,
      angle_cache,
      vsx_color<>(color_r_cache, color_g_cache, color_b_cache, color_a_cache),
      (bool)alpha_in->get(),
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
