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

class module_bitmap_generators_concentric_circles : public vsx_module
{
  // in
  vsx_module_param_float* frequency_in;
  vsx_module_param_float* attenuation_in;
  vsx_module_param_float* angle_in;
  vsx_module_param_float4* color_in;
  vsx_module_param_int* alpha_in;
  vsx_module_param_int* size_in;

	// out
  vsx_module_param_bitmap* bitmap_out;
  vsx_module_param_texture* texture_out;

	// internal
  vsx_bitmap bitmap;
  vsx_texture<>* texture = 0x0;

  int p_updates = -1;

  pthread_t	worker_t;
  bool worker_running = false;

  int thread_state = 0;
  int i_size = 0;
  float work_color[4];
  int work_alpha;

  void *to_delete_data = 0x0;

public:

  int c_type;

  void module_info(vsx_module_info* info)
  {
    info->identifier = "bitmaps;generators;concentric_circles||bitmaps;generators;particles;concentric_circles";
    info->in_param_spec = ""
        "settings:complex{"
          "frequency:float?min=0.0,"
          "attenuation:float?default_controller=controller_slider&min=0.0,"
          "color:float4?default_controller=controller_col,"
          "alpha:enum?no|yes"
        "},"
        "size:enum?8x8|16x16|32x32|64x64|128x128|256x256|512x512|1024x1024|2048x2048"
        ;

    if (c_type == 0)
    {
      info->out_param_spec = "bitmap:bitmap";
      info->component_class = "bitmap";
    } else
    {
      info->identifier = "texture;particles;concentric_circles";
      info->out_param_spec = "texture:texture";
      info->component_class = "texture";
    }
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

    angle_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"angle");
    bitmap_out = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
  }


  static void* worker(void *ptr)
  {
    module_bitmap_generators_concentric_circles& module = *(module_bitmap_generators_concentric_circles*)ptr;

    int x,y;
    float attenuation = module.attenuation_in->get();
    float frequency = module.frequency_in->get() * 2.0f;

    vsx_bitmap_32bt *p = (vsx_bitmap_32bt*)module.bitmap.data_get();

    int size = module.i_size;

    float dist;
    int hsize = size >> 1;
    float one_div_hsize = 1.0f / ((float)hsize+1);

    vsx_printf(L"%f        %f\n", attenuation, frequency);

    for (y = -hsize; y < hsize; ++y)
      for (x = -hsize; x < hsize; ++x,p++)
      {
        float xx = (size/(size-2.0f))*((float)x)+0.5f;
        float yy = (size/(size-2.0f))*((float)y)+0.5f;
        float dd = sqrt(xx*xx + yy*yy);

        float dstf = dd * one_div_hsize;

        dist = pow(fabs(cos(dstf * PI * frequency)), attenuation) * cos(dstf * PI * 0.5);

        if ( module.work_alpha == 1)
        {
          long pr = CLAMP( (long)(255.0f *  module.work_color[0]), 0, 255);
          long pg = CLAMP( (long)(255.0f *  module.work_color[1]), 0, 255);
          long pb = CLAMP( (long)(255.0f *  module.work_color[2]), 0, 255);
          long pa = CLAMP( (long)(255.0f * dist * module.work_color[3]), 0, 255);
          *p = 0x01000000 * pa | pb * 0x00010000 | pg * 0x00000100 | pr;
        } else
        if (module.work_alpha == 0) {
          long pr = CLAMP( (long)(255.0f * dist * module.work_color[0]), 0, 255);
          long pg = CLAMP( (long)(255.0f * dist * module.work_color[1]), 0, 255);
          long pb = CLAMP( (long)(255.0f * dist * module.work_color[2]), 0, 255);
          long pa = (long)(255.0f * module.work_color[3]);
          *p = 0x01000000 * pa | pb * 0x00010000 | pg * 0x00000100 | pr;
        }
      }

    module.bitmap.timestamp++;

    __sync_fetch_and_add( &(module.bitmap.data_ready), 1 );
    return 0;
  }

  void run()
  {
    if (to_delete_data)
    {
      free(to_delete_data);
      to_delete_data = 0;
    }

    if (bitmap.data_ready && worker_running)
    {
      pthread_join(worker_t,0);
      worker_running = false;

      bitmap_out->set(&bitmap);
      loading_done = true;

      if (c_type == 1)
      {
        if (!texture)
          texture = new vsx_texture<>();
        texture->texture->init_opengl_texture_2d();
        vsx_texture_gl_loader::upload_bitmap_2d(texture->texture, &bitmap, true);
        texture_out->set(texture);
      }
    }

    req(p_updates != param_updates);
    req(!worker_running);
    p_updates = param_updates;

    if (i_size != 8 << size_in->get())
    {
      i_size = 8 << size_in->get();
      if (bitmap.data_get())
        to_delete_data = bitmap.data_get();
      bitmap.data_set( malloc( sizeof(vsx_bitmap_32bt) * i_size * i_size) );

      bitmap.width  = i_size;
      bitmap.height = i_size;
    }


    work_alpha = alpha_in->get();
    work_color[0] = CLAMP(color_in->get(0), 0.0, 1.0);
    work_color[1] = CLAMP(color_in->get(1), 0.0, 1.0);
    work_color[2] = CLAMP(color_in->get(2), 0.0, 1.0);
    work_color[3] = CLAMP(color_in->get(3), 0.0, 1.0);

    worker_running = true;
    pthread_create(&worker_t, NULL, &worker, (void*)this);
  }

  void start() {
    req(c_type);
    req(bitmap.is_valid());
    texture->texture->init_opengl_texture_2d();
    vsx_texture_gl_loader::upload_bitmap_2d(texture->texture, &bitmap, true);
    texture_out->set(texture);
  }

  void stop() {
    req(c_type);
    texture->unload_gl();
  }

  void on_delete()
  {
    // wait for thread to finish
    if (worker_running)
      pthread_join(worker_t,NULL);

    if (c_type && texture)
    {
      texture->unload_gl();
      delete texture;
    }
  }

};
