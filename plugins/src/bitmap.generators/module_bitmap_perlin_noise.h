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



#include "perlin/perlin.h"
#include <bitmap/vsx_bitmap.h>


class module_bitmap_perlin_noise : public vsx_module
{
public:
  // in

  // blob settings
  vsx_module_param_int* enable_blob_in;
  vsx_module_param_float* arms_in;
  vsx_module_param_float* attenuation_in;
  vsx_module_param_float* star_flower_in;
  vsx_module_param_float* angle_in;
  // general settings
  vsx_module_param_int* size_in;
  vsx_module_param_int* octave_in;
  vsx_module_param_int* frequency_in;
  vsx_module_param_int* bitmap_type_in;
  vsx_module_param_int* alpha_in;
  vsx_module_param_float* perlin_strength_in;
  vsx_module_param_float4* color_in;
  vsx_module_param_float* rand_seed;

  // out
  vsx_module_param_bitmap* bitmap_out;

  // internal
  vsx_bitmap bitmap;

  int bitm_timestamp = -1;
  pthread_t worker_t;
  int p_updates = -1;
  bool worker_running = false;
  int thread_state = 0;
  int i_size = 0;
  void* to_delete_data = 0x0;

  static void* worker(void *ptr)
  {
    module_bitmap_perlin_noise* module = ((module_bitmap_perlin_noise*)ptr);

    Perlin* perlin = new Perlin(
                           module->octave_in->get()+1,
                           module->frequency_in->get()+1,
                           1.0f,
                           (int)module->rand_seed->get()
                         );
    float divisor = 1.0f / (float)module->i_size;

    float attenuation = module->attenuation_in->get();
    float arms = module->arms_in->get()*0.5f;
    float star_flower = module->star_flower_in->get();
    float angle = module->angle_in->get();
    int size = module->i_size;
    int hsize = size / 2;
    
    if (module->bitmap.storage_format == vsx_bitmap::byte_storage)
    {
      // integer data type 
      vsx_bitmap_32bt *p = (vsx_bitmap_32bt*)module->bitmap.data[0];
      float yp = 0.0f;
      float xp;
      for (int y = -hsize; y < hsize; ++y)
      {
        xp = 0.0f;
        for (int x = -hsize; x < hsize; ++x)
        {
          float dist = 1.0f;
          if (module->enable_blob_in->get())
          {
            float xx = (size/(size-2.0f))*((float)x)+0.5f;
            float yy = (size/(size-2.0f))*((float)y)+0.5f;
            float dd = sqrt(xx*xx + yy*yy);
            float dstf = dd/((float)hsize+1);
            float phase = (float)pow(1.0f - (float)fabs((float)cos(angle+arms*(float)atan2(xx,yy)))*(star_flower+(1-star_flower)*(((dstf)))),attenuation);
            if (phase > 2.0f) phase = 1.0f;
            dist = (cos(((dstf * PI/2.0f)))*phase);
            if (dist > 1.0f) dist = 1.0f;
            if (dist < 0.0f) dist = 0.0f;
          }
          float pf = pow( (perlin->Get(xp,yp)+1.0f) * 0.5f, module->perlin_strength_in->get()) * 255.0f * dist;
          if (module->alpha_in->get())
          {
            long pr = MAX(0,MIN(255,(long)(255.0f * module->color_in->get(0))));
            long pg = MAX(0,MIN(255,(long)(255.0f * module->color_in->get(1))));
            long pb = MAX(0,MIN(255,(long)(255.0f * module->color_in->get(2))));
            long pa = MAX(0,MIN(255,(long)(pf * module->color_in->get(3))));
            *p = 0x01000000 * pa | pb * 0x00010000 | pg * 0x00000100 | pr;
          } else
          {
            long pr = MAX(0,MIN(255,(long)(pf * module->color_in->get(0))));
            long pg = MAX(0,MIN(255,(long)(pf * module->color_in->get(1))));
            long pb = MAX(0,MIN(255,(long)(pf * module->color_in->get(2))));
            long pa = (long)(255.0f * module->color_in->get(3));
            *p = 0x01000000 * pa | pb * 0x00010000 | pg * 0x00000100 | pr;
          }
          ++p;
          xp += divisor;
        }
        yp += divisor;
      }
    } else
    if (module->bitmap.storage_format == vsx_bitmap::float_storage)
    {
      // integer data type
      GLfloat *p = (GLfloat*)module->bitmap.data[0];
      float yp = 0.0f;
      float xp;
      float ddiv = 1.0f / (((float)hsize)+1.0f);
      for (int y = -hsize; y < hsize; ++y)
      {
        xp = 0.0f;
        for (int x = -hsize; x < hsize; ++x)
        {
          float dist = 1.0f;
          if (module->enable_blob_in->get())
          {
            float xx = (size/(size-2.0f))*((float)x)+0.5f;
            float yy = (size/(size-2.0f))*((float)y)+0.5f;
            float dd = sqrt(xx*xx + yy*yy);
            if (dd > (float)hsize)
            {
              dist = 0.0f;
            }
            else
            {
            float xx = (size/(size-2.0f))*((float)x)+0.5f;
            float yy = (size/(size-2.0f))*((float)y)+0.5f;
            float dd = sqrt(xx*xx + yy*yy);
            float dstf = dd * ddiv;
            float phase = (float)pow(1.0f - (float)fabs((float)cos(angle+arms*(float)atan2(xx,yy)))*(star_flower+(1-star_flower)*(((dstf)))),attenuation);
            if (phase > 2.0f) phase = 1.0f;
            dist = (cos(((dstf * PI/2.0f)))*phase);
            if (dist > 1.0f) dist = 1.0f;
            if (dist < 0.0f) dist = 0.0f;
            }
          }
          
          GLfloat pf = (GLfloat)(pow( (perlin->Get(xp,yp)+1.0f) * 0.5f, module->perlin_strength_in->get())* dist);
          if (module->alpha_in->get())
          {
            p[0] = module->color_in->get(0);
            p[1] = module->color_in->get(1);
            p[2] = module->color_in->get(2);
            p[3] = MAX(0.0f,MIN(1.0f,pf * module->color_in->get(3)));
          } else {
            p[0] = pf*module->color_in->get(0);
            p[1] = pf*module->color_in->get(1);
            p[2] = pf*module->color_in->get(2);
            p[3] = module->color_in->get(3);
          }
          ++p;
          ++p;
          ++p;
          ++p;
          xp += divisor;
        }
        yp += divisor;
      }
    }
    delete perlin;
    module->bitmap.timestamp++;
    __sync_fetch_and_add( &(module->bitmap.data_ready), 1 );
    return 0;
  }

  void module_info(vsx_module_info* info)
  {
    info->in_param_spec = "perlin_options:complex{"
                          "rand_seed:float,"
                          "perlin_strength:float,"
                          "size:enum?8x8|16x16|32x32|64x64|128x128|256x256|512x512|1024x1024|2048x2048,"
                          "octave:enum?1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16,"
                          "frequency:enum?1|2|3|4|5|6|7|8,"
                          "bitmap_type:enum?integer|float},"
                          "blob_settings:complex{"
                            "enable_blob:enum?no|yes,"
                            "arms:float,"
                            "attenuation:float,"
                            "star_flower:float,"
                            "angle:float,"
                          "},"
                          "color:float4?default_controller=controller_col,"
                          "alpha:enum?no|yes"
                          ;
    info->identifier = "bitmaps;generators;perlin_noise";
    info->out_param_spec = "bitmap:bitmap";
    info->component_class = "bitmap";
    info->description = "Perlin Noise (clouds) generator";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    rand_seed = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"rand_seed");
    rand_seed->set(4.0f);

    perlin_strength_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"perlin_strength");
    perlin_strength_in->set(1.0f);

    enable_blob_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"enable_blob");
    
    arms_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"arms");
    attenuation_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"attenuation");
    star_flower_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"star_flower");
    angle_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"angle");

    size_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"size");
    size_in->set(4);

    frequency_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"frequency");
    frequency_in->set(0);

    octave_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"octave");
    octave_in->set(0);
    
    bitmap_out = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");

    bitmap_type_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"bitmap_type");
    
    alpha_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"alpha");

    color_in = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"color");
    color_in->set(1.0f, 0);
    color_in->set(1.0f, 1);
    color_in->set(1.0f, 2);
    color_in->set(1.0f, 3);
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

      bitm_timestamp = bitmap.timestamp;
      bitmap_out->set(&bitmap);
      loading_done = true;
    }

    req(!worker_running);
    req(p_updates != param_updates);
    p_updates = param_updates;

    if (i_size != 8 << size_in->get())
    {
      i_size = 8 << size_in->get();

      if (bitmap.data[0])
        to_delete_data = bitmap.data;

      switch (bitmap_type_in->get())
      {
        case 0:
          bitmap.storage_format = vsx_bitmap::byte_storage;
          bitmap.data[0] = malloc( sizeof(vsx_bitmap_32bt) * i_size * i_size );
          break;
        case 1:
          bitmap.storage_format = vsx_bitmap::float_storage;
          bitmap.data[0] = malloc( sizeof(GLfloat) * i_size * i_size );
          break;
      }
      bitmap.width = i_size;
      bitmap.height = i_size;
    }
    bitmap.data_ready = 0;
    pthread_create(&worker_t, NULL, &worker, (void*)this);
  }

  void on_delete()
  {
    if (worker_running)
      pthread_join(worker_t,NULL);

    if (bitmap.data[0])
      free(bitmap.data[0]);
  }
};
