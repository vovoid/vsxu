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
#include <math/vector/vsx_vector2.h>
#include <color/vsx_color.h>
#include <bitmap/generators/vsx_bitmap_generator_plasma.h>

class module_bitmap_plasma : public vsx_module
{
public:
  // in - function
  vsx_module_param_float4* col_amp_in;
  vsx_module_param_float4* col_ofs_in;

  // in - function - period
  vsx_module_param_float3* r_period_in;
  vsx_module_param_float3* g_period_in;
  vsx_module_param_float3* b_period_in;
  vsx_module_param_float3* a_period_in;

  // in - function - ofs
  vsx_module_param_float3* r_ofs_in;
  vsx_module_param_float3* g_ofs_in;
  vsx_module_param_float3* b_ofs_in;
  vsx_module_param_float3* a_ofs_in;

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
        "col_amp:float4,"
        "col_ofs:float4,"
        "period:complex{"
          "r_period:float3,"
          "g_period:float3,"
          "b_period:float3,"
          "a_period:float3"
        "},"
        "ofs:complex{"
          "r_ofs:float3,"
          "g_ofs:float3,"
          "b_ofs:float3,"
          "a_ofs:float3"
        "}"
      "},"
      "options:complex{"
        "size:enum?8x8|16x16|32x32|64x64|128x128|256x256|512x512|1024x1024&nc=1"
      "}"
    ;

    info->identifier = "bitmaps;generators;plasma";
    info->out_param_spec = "bitmap:bitmap";
    info->component_class = "bitmap";
    info->description = "Generates a Sin-plasma bitmap";
  }
  
  
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    // function
    col_amp_in = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"col_amp");
    col_amp_in->set(1.0f, 0);
    col_amp_in->set(1.0f, 1);
    col_amp_in->set(1.0f, 2);
    col_amp_in->set(1.0f, 3);
    col_ofs_in = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"col_ofs");

    r_period_in = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"r_period");
    r_period_in->set(1.0f, 0);
    r_period_in->set(1.0f, 1);

    g_period_in = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"g_period");
    g_period_in->set(1.0f, 0);
    g_period_in->set(1.0f, 1);

    b_period_in = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"b_period");
    a_period_in = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"a_period");

    r_ofs_in = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"r_ofs");
    g_ofs_in = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"g_ofs");
    b_ofs_in = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"b_ofs");
    a_ofs_in = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"a_ofs");

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

    vsx_string<> cache_handle = vsx_bitmap_generator_plasma::generate_cache_handle(
        vsx_vector2f(r_period_in->get(0), r_period_in->get(1)),
        vsx_vector2f(g_period_in->get(0), g_period_in->get(1)),
        vsx_vector2f(b_period_in->get(0), b_period_in->get(1)),
        vsx_vector2f(a_period_in->get(0), a_period_in->get(1)),
        vsx_vector2f(r_ofs_in->get(0), r_ofs_in->get(1)),
        vsx_vector2f(g_ofs_in->get(0), g_ofs_in->get(1)),
        vsx_vector2f(b_ofs_in->get(0), b_ofs_in->get(1)),
        vsx_vector2f(a_ofs_in->get(0), a_ofs_in->get(1)),
        vsx_colorf(col_amp_in->get(0), col_amp_in->get(1), col_amp_in->get(2), col_amp_in->get(3)),
        vsx_colorf(col_ofs_in->get(0), col_ofs_in->get(1), col_ofs_in->get(2), col_ofs_in->get(3)),
        size_in->get()
      );

    if (!bitmap)
      bitmap = vsx_bitmap_cache::get_instance()->
        aquire_create(cache_handle, 0);

    bitmap->filename = cache_handle;
    vsx_bitmap_generator_plasma::generate_thread(
          bitmap,
          vsx_vector2f(r_period_in->get(0), r_period_in->get(1)),
          vsx_vector2f(g_period_in->get(0), g_period_in->get(1)),
          vsx_vector2f(b_period_in->get(0), b_period_in->get(1)),
          vsx_vector2f(a_period_in->get(0), a_period_in->get(1)),
          vsx_vector2f(r_ofs_in->get(0), r_ofs_in->get(1)),
          vsx_vector2f(g_ofs_in->get(0), g_ofs_in->get(1)),
          vsx_vector2f(b_ofs_in->get(0), b_ofs_in->get(1)),
          vsx_vector2f(a_ofs_in->get(0), a_ofs_in->get(1)),
          vsx_colorf(col_amp_in->get(0), col_amp_in->get(1), col_amp_in->get(2), col_amp_in->get(3)),
          vsx_colorf(col_ofs_in->get(0), col_ofs_in->get(1), col_ofs_in->get(2), col_ofs_in->get(3)),
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
