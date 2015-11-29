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

  // out
  vsx_module_param_bitmap* bitmap_out;

  // internal
  bool need_to_rebuild;

  vsx_bitmap bitmap;
  int bitm_timestamp;

  pthread_t         worker_t;

  int p_updates;
  int my_ref;

  vsx_module_param_float* rand_seed;


  vsx_bitmap*       work_bitmap;
  bool              worker_running;
  bool              thread_created;
  int               thread_state;
  int               i_size;
  int               old_bitmap_type;

  void *to_delete_data;
  vsx_bitmap::channel_storage_type_t  to_delete_type;


  // our worker thread, to keep the tough generating work off the main loop
  // this is a fairly simple operation, but when you want to generate fractals
  // and decode film, you could run into several seconds of processing time.
  static void* worker(void *ptr) {

    module_bitmap_perlin_noise* mod = ((module_bitmap_perlin_noise*)ptr);

    Perlin* perlin = new Perlin(
                           mod->octave_in->get()+1,
                           mod->frequency_in->get()+1,
                           1.0f,
                           (int)mod->rand_seed->get()
                         );
    float divisor = 1.0f / (float)mod->i_size;

    float attenuation = mod->attenuation_in->get();
    float arms = mod->arms_in->get()*0.5f;
    float star_flower = mod->star_flower_in->get();
    float angle = mod->angle_in->get();
    int size = mod->i_size;
    int hsize = size / 2;
    
    if (mod->work_bitmap->channels == 4 && mod->work_bitmap->storage_format == vsx_bitmap::byte_storage)
    {
      // integer data type 
      vsx_bitmap_32bt *p = (vsx_bitmap_32bt*)mod->work_bitmap->data;
      float yp = 0.0f;
      float xp;
      for (int y = -hsize; y < hsize; ++y)
      {
        xp = 0.0f;
        for (int x = -hsize; x < hsize; ++x)
        {
          float dist = 1.0f;
          if (mod->enable_blob_in->get())
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
          float pf = pow( (perlin->Get(xp,yp)+1.0f) * 0.5f, mod->perlin_strength_in->get()) * 255.0f * dist;
          if (mod->alpha_in->get())
          {
            long pr = MAX(0,MIN(255,(long)(255.0f * mod->color_in->get(0))));
            long pg = MAX(0,MIN(255,(long)(255.0f * mod->color_in->get(1))));
            long pb = MAX(0,MIN(255,(long)(255.0f * mod->color_in->get(2))));
            long pa = MAX(0,MIN(255,(long)(pf * mod->color_in->get(3))));
            *p = 0x01000000 * pa | pb * 0x00010000 | pg * 0x00000100 | pr;
          } else
          {
            long pr = MAX(0,MIN(255,(long)(pf * mod->color_in->get(0))));
            long pg = MAX(0,MIN(255,(long)(pf * mod->color_in->get(1))));
            long pb = MAX(0,MIN(255,(long)(pf * mod->color_in->get(2))));
            long pa = (long)(255.0f * mod->color_in->get(3));
            *p = 0x01000000 * pa | pb * 0x00010000 | pg * 0x00000100 | pr;
          }
          
//          *p = 0xFF000000 | pf << 16 | pf << 8 | pf;// | 0 * 0x00000100 | 0;
          ++p;
          xp += divisor;
        }
        yp += divisor;
      }
    } else
    if (mod->work_bitmap->channels == 4 && mod->work_bitmap->storage_format == vsx_bitmap::float_storage)
    {
      //printf("float format\n");
      // integer data type
      GLfloat *p = (GLfloat*)mod->work_bitmap->data;
      float yp = 0.0f;
      float xp;
      float ddiv = 1.0f / (((float)hsize)+1.0f);
      for (int y = -hsize; y < hsize; ++y)
      {
        xp = 0.0f;
        for (int x = -hsize; x < hsize; ++x)
        {
          float dist = 1.0f;
          if (mod->enable_blob_in->get())
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
          
          GLfloat pf = (GLfloat)(pow( (perlin->Get(xp,yp)+1.0f) * 0.5f, mod->perlin_strength_in->get())* dist);
          if (mod->alpha_in->get())
          {
            p[0] = mod->color_in->get(0);
            p[1] = mod->color_in->get(1);
            p[2] = mod->color_in->get(2);
            p[3] = MAX(0.0f,MIN(1.0f,pf * mod->color_in->get(3)));
          } else {
            p[0] = pf*mod->color_in->get(0);
            p[1] = pf*mod->color_in->get(1);
            p[2] = pf*mod->color_in->get(2);
            p[3] = mod->color_in->get(3);
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
    mod->work_bitmap->timestamp++;
    mod->work_bitmap->valid = true;
    mod->thread_state = 2;
    // the thread will die here.
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
    thread_state = 0;
    worker_running = false;
    thread_created = false;
    p_updates = -1;

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

    i_size = 0;
    
    bitmap_out = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");

    bitmap_type_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"bitmap_type");
    
    alpha_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"alpha");

    color_in = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"color");
    color_in->set(1.0f, 0);
    color_in->set(1.0f, 1);
    color_in->set(1.0f, 2);
    color_in->set(1.0f, 3);
        
    work_bitmap = &bitmap;
    my_ref = 0;
    bitm_timestamp = bitmap.timestamp = rand();
    need_to_rebuild = true;

    to_delete_data = 0;
    old_bitmap_type = 0;
  }
  void run() {
    // initialize our worker thread, we don't want to keep the renderloop waiting do we?
    if (!worker_running)
    if (p_updates != param_updates)
    {
      //need_to_rebuild = false;
      if (i_size != 8 << size_in->get() || old_bitmap_type != bitmap_type_in->get())
      {
        i_size = 8 << size_in->get();

        if (bitmap.data)
        {
          to_delete_type = bitmap.storage_format;
          to_delete_data = bitmap.data;
        }

        switch (bitmap_type_in->get())
        {
          case 0:
            bitmap.storage_format = vsx_bitmap::byte_storage;
            bitmap.channels = 4;
            bitmap.data = new vsx_bitmap_32bt[i_size*i_size];
            break;
          case 1:
            bitmap.storage_format = vsx_bitmap::float_storage;
            bitmap.channels = 4;
            bitmap.data = new GLfloat[i_size*i_size*4];
            break;
        }
        bitmap.width = i_size;
        bitmap.height = i_size;
      }

      p_updates = param_updates;
      bitmap.valid = false;
      thread_state = 1;
      worker_running = true;
      pthread_create(&worker_t, NULL, &worker, (void*)this);
      thread_created = true;
    }


    if (thread_state == 2) {
      if (bitmap.valid && bitm_timestamp != bitmap.timestamp) {
        pthread_join(worker_t,0);
        worker_running = false;
        // ok, new version
        bitm_timestamp = bitmap.timestamp;
        bitmap_out->set(&bitmap);
        loading_done = true;
      }
      thread_state = 3;
    }
    if (to_delete_data && my_ref == 0)
    {
      if (to_delete_type == vsx_bitmap::byte_storage)
        delete[] (vsx_bitmap_32bt*)to_delete_data;

      if (to_delete_type == vsx_bitmap::float_storage)
        delete[] (GLfloat*)to_delete_data;

      to_delete_data = 0;
    }
  }

  void on_delete() {
    // wait for thread to finish
    void* ret;
    if (worker_running)
    {
      pthread_join(worker_t,&ret);
    }
    if (bitmap.data)
    {
      delete[] (vsx_bitmap_32bt*)bitmap.data;
    }
  }
};
