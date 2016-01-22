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
#include <bitmap/generators/vsx_bitmap_generator_concentric_circles.h>

class module_bitmap_generators_concentric_circles : public vsx_module
{
  // in
  vsx_module_param_float* frequency_in;
  vsx_module_param_float* attenuation_in;
  vsx_module_param_float4* color_in;
  vsx_module_param_int* alpha_in;
  vsx_module_param_int* size_in;

	// out
  vsx_module_param_bitmap* bitmap_out;
  vsx_module_param_texture* texture_out;

	// internal
  bool worker_running = false;
  vsx_bitmap* bitmap = 0x0;
  vsx_bitmap* old_bitmap = 0x0;
  float frequency_cache = 0.0f;
  float attenuation_cache = 0.1f;
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
        "bitmaps;generators;concentric_circles"
        "||!bitmaps;generators;particles;concentric_circles"
      ;

    info->in_param_spec =
        "settings:complex{"
          "frequency:float?min=0.0,"
          "attenuation:float?default_controller=controller_slider&min=0.0,"
          "color:float4?default_controller=controller_col,"
          "alpha:enum?no|yes"
        "},"
        "size:enum?8x8|16x16|32x32|64x64|128x128|256x256|512x512|1024x1024|2048x2048"
      ;

    info->out_param_spec = "bitmap:bitmap";
    info->component_class = "bitmap";
    info->description = "Generates a texture with concentric circles.";
  }


  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    frequency_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"frequency");
    frequency_in->set(1.0f);

    attenuation_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"attenuation");
    attenuation_in->set(2.0f);

    size_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"size");
    size_in->set(4);

    alpha_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"alpha");
    alpha_in->set(0);

    color_in = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"color");
    color_in->set(1.0f,0);
    color_in->set(1.0f,1);
    color_in->set(1.0f,2);
    color_in->set(1.0f,3);

    bitmap_out = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
  }



  bool has_state_changed()
  {
    cache_check_f(frequency, 0.01f)
    cache_check_f(attenuation, 0.01f)
    cache_check(size)
    cache_check(alpha)

    if (
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

    if (bitmap)
    {
      old_bitmap = bitmap;
      bitmap = 0x0;
    }

    vsx_string<> cache_handle = vsx_bitmap_generator_concentric_circles::generate_cache_handle(
          frequency_in->get(),
          attenuation_in->get(),
          vsx_color<>(color_in->get(0), color_in->get(1), color_in->get(2), color_in->get(3)),
          (bool)alpha_in->get(),
          size_in->get()
        );

    if (!bitmap)
      bitmap = vsx_bitmap_cache::get_instance()->
        aquire_create(cache_handle, 0);

    bitmap->filename = cache_handle;
    vsx_bitmap_generator_concentric_circles::load(
          bitmap,
          frequency_in->get(),
          attenuation_in->get(),
          vsx_color<>(color_in->get(0), color_in->get(1), color_in->get(2), color_in->get(3)),
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
