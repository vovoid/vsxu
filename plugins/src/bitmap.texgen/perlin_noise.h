/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan "jaw" Wallmander, Vovoid Media Technologies - Copyright (C) 2003-2011
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


#include "perlin/perlin.h"
#include <vsx_bitmap.h>


class module_bitmap_texgen_perlin_noise : public vsx_module {
  // in

  // out
  vsx_module_param_bitmap* result1;
  // internal
  bool need_to_rebuild;

  vsx_bitmap bitm;
  int bitm_timestamp;

  pthread_t         worker_t;

  int p_updates;
  int my_ref;

public:
  vsx_module_param_float* rand_seed;

  // blob settings
  vsx_module_param_int* enable_blob;
  vsx_module_param_float* arms;
  vsx_module_param_float* attenuation;
  vsx_module_param_float* star_flower;
  vsx_module_param_float* angle;
  // general settings
  vsx_module_param_int* size;
  vsx_module_param_int* octave;
  vsx_module_param_int* frequency;
  vsx_module_param_int* bitmap_type;
  vsx_module_param_int* alpha;
  vsx_module_param_float* perlin_strength;
  vsx_module_param_float4* color;

  vsx_bitmap*       work_bitmap;
  bool              worker_running;
  bool              thread_created;
  int               thread_state;
  int               i_size;
  int               old_bitmap_type;

  // our worker thread, to keep the tough generating work off the main loop
  // this is a fairly simple operation, but when you want to generate fractals
  // and decode film, you could run into several seconds of processing time.
  static void* worker(void *ptr) {

    module_bitmap_texgen_perlin_noise* mod = ((module_bitmap_texgen_perlin_noise*)ptr);

    Perlin* perlin = new Perlin(
                           mod->octave->get()+1,
                           mod->frequency->get()+1,
                           1.0f,
                           (int)mod->rand_seed->get()
                         );
    float divisor = 1.0f / (float)mod->i_size;

    float attenuation = mod->attenuation->get();
    float arms = mod->arms->get()*0.5f;
    float star_flower = mod->star_flower->get();
    float angle = mod->angle->get();
    int size = mod->i_size;
    int hsize = size / 2;
    
    if (mod->work_bitmap->bpp == 4)
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
          if (mod->enable_blob->get())
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
          float pf = pow( (perlin->Get(xp,yp)+1.0f) * 0.5f, mod->perlin_strength->get()) * 255.0f * dist;
          if (mod->alpha->get())
          {
            long pr = max(0,min(255,(long)(255.0f * mod->color->get(0))));
            long pg = max(0,min(255,(long)(255.0f * mod->color->get(1))));
            long pb = max(0,min(255,(long)(255.0f * mod->color->get(2))));
            long pa = max(0,min(255,(long)(pf * mod->color->get(3))));
            *p = 0x01000000 * pa | pb * 0x00010000 | pg * 0x00000100 | pr;
          } else
          {
            long pr = max(0,min(255,(long)(pf * mod->color->get(0))));
            long pg = max(0,min(255,(long)(pf * mod->color->get(1))));
            long pb = max(0,min(255,(long)(pf * mod->color->get(2))));
            long pa = (long)(255.0f * mod->color->get(3));
            *p = 0x01000000 * pa | pb * 0x00010000 | pg * 0x00000100 | pr;
          }
          
//          *p = 0xFF000000 | pf << 16 | pf << 8 | pf;// | 0 * 0x00000100 | 0;
          ++p;
          xp += divisor;
        }
        yp += divisor;
      }
    } else
    if (mod->work_bitmap->bpp == GL_RGBA32F_ARB)
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
          if (mod->enable_blob->get())
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
          
          GLfloat pf = (GLfloat)(pow( (perlin->Get(xp,yp)+1.0f) * 0.5f, mod->perlin_strength->get())* dist);
          if (mod->alpha->get())
          {
            p[0] = mod->color->get(0);
            p[1] = mod->color->get(1);
            p[2] = mod->color->get(2);
            p[3] = max(0.0f,min(1.0f,pf * mod->color->get(3)));
          } else {
            p[0] = pf*mod->color->get(0);
            p[1] = pf*mod->color->get(1);
            p[2] = pf*mod->color->get(2);
            p[3] = mod->color->get(3);
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

    perlin_strength = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"perlin_strength");
    perlin_strength->set(1.0f);

    enable_blob = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"enable_blob");
    
    arms = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"arms");
    attenuation = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"attenuation");
    star_flower = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"star_flower");
    angle = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"angle");

    size = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"size");
    size->set(4);

    frequency = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"frequency");
    frequency->set(0);

    octave = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"octave");
    octave->set(0);

    i_size = 0;
    
    result1 = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
    result1->set_p(bitm);

    bitmap_type = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"bitmap_type");
    
    alpha = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"alpha");

    color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"color");
    color->set(1.0f, 0);
    color->set(1.0f, 1);
    color->set(1.0f, 2);
    color->set(1.0f, 3);
    
    
    work_bitmap = &bitm;
    bitm.data = 0;
    bitm.bpp = 4;
    bitm.bformat = GL_RGBA;
    bitm.valid = false;
    my_ref = 0;
    bitm.ref = &my_ref;
    bitm_timestamp = bitm.timestamp = rand();
    need_to_rebuild = true;
    //bitm.data = new vsx_bitmap_32bt[256*256];
    //bitm.size_y = bitm.size_x = 256;
    to_delete_data = 0;
    old_bitmap_type = 0;
  }
  void *to_delete_data;
  int  to_delete_type;
  void run() {
    // initialize our worker thread, we don't want to keep the renderloop waiting do we?
    if (!worker_running)
    if (p_updates != param_updates) {
      //need_to_rebuild = false;
      if (i_size != 8 << size->get() || old_bitmap_type != bitmap_type->get()) {
        i_size = 8 << size->get();
        if (bitm.data) {
          to_delete_type = bitm.bformat;
          to_delete_data = (void*)bitm.data;
        }
        old_bitmap_type = bitmap_type->get();
        switch (old_bitmap_type)
        {
          case 0:
            bitm.bpp = 4;
            bitm.data = new vsx_bitmap_32bt[i_size*i_size];
            break;
          case 1:
            bitm.bpp = GL_RGBA32F_ARB;
            bitm.data = new GLfloat[i_size*i_size*4];
            break;
        }
        bitm.size_y = bitm.size_x = i_size;
      }

      p_updates = param_updates;
      bitm.valid = false;
      //printf("creating thread\n");
      thread_state = 1;
      worker_running = true;
      pthread_create(&worker_t, NULL, &worker, (void*)this);
      thread_created = true;

      //printf("done creating thread\n");
    }
    if (thread_state == 2) {
      if (bitm.valid && bitm_timestamp != bitm.timestamp) {
        //pthread_join(worker_t,0);
        worker_running = false;
        // ok, new version
        //printf("uploading subplasma to param\n");
        bitm_timestamp = bitm.timestamp;
        result1->set_p(bitm);
        loading_done = true;
      }
      thread_state = 3;
    }
    if (to_delete_data && my_ref == 0)
    {
      delete[] (vsx_bitmap_32bt*)to_delete_data;
      to_delete_data = 0;
    }
  }

  void on_delete() {
    // wait for thread to finish
    void* ret;
    if (thread_created)
    {
      pthread_join(worker_t,&ret);
    }
    if (bitm.data)
    {
      delete[] (vsx_bitmap_32bt*)bitm.data;
    }
  }
};
