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
  vsx_module_param_texture* texture_out;

	// internal
	bool need_to_rebuild;

  vsx_bitmap bitmap;
	int bitm_timestamp;

  vsx_texture<>* texture;
  pthread_t	worker_t;
  pthread_attr_t attr;

  int p_updates;
  int my_ref;


  vsx_bitmap*       work_bitmap;
  bool              worker_running;
  int               thread_state;
  int               i_size;
  float             work_color[4];
  int               work_alpha;

  void *to_delete_data = 0;


public:

  int               c_type;

  void module_info(vsx_module_info* info)
  {
    info->identifier = "bitmaps;generators;blob||bitmaps;generators;particles;blob";
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
    if (c_type == 0) {
      info->out_param_spec = "bitmap:bitmap";
      info->component_class = "bitmap";
    } else
    {
      info->identifier = "texture;particles;blob";
      info->out_param_spec = "texture:texture";
      info->component_class = "texture";
    }
    info->description = "Generates blobs,stars or leaf\ndepending on parameters.\nPlay with the params :)";
  }


  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    thread_state = 0;
    worker_running = false;
    p_updates = -1;
    arms_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"arms");
    attenuation_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"attenuation");
    attenuation_in->set(0.1f);
    size_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"size");
    size_in->set(4);
    alpha_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"alpha");
    alpha_in->set(0);
    color_in = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"color");
    color_in->set(1.0f,0);
    color_in->set(1.0f,1);
    color_in->set(1.0f,2);
    color_in->set(1.0f,3);
    star_flower_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"star_flower");
    angle_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"angle");
    i_size = 0;
    bitmap_out = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
    work_bitmap = &bitmap;
    bitm_timestamp = bitmap.timestamp;
    need_to_rebuild = true;
    my_ref = 0;
    if (c_type == 1) {
      texture = new vsx_texture<>();
      texture->texture->init_opengl_texture_2d();
      texture_out = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture");
      texture_out->set(texture);
    }
    to_delete_data = 0;
  }


  static void* worker(void *ptr)
  {
    int x,y;
    float attenuation = ((module_bitmap_generators_blob*)ptr)->attenuation_in->get();
    float arms = ((module_bitmap_generators_blob*)ptr)->arms_in->get()*0.5f;
    float star_flower = ((module_bitmap_generators_blob*)ptr)->star_flower_in->get();
    float angle = ((module_bitmap_generators_blob*)ptr)->angle_in->get();
    vsx_bitmap_32bt *p = (vsx_bitmap_32bt*)((module_bitmap_generators_blob*)ptr)->work_bitmap->data_get();
    int size = ((module_bitmap_generators_blob*)ptr)->i_size;

    float dist;
    int hsize = size >> 1;
    for(y = -hsize; y < hsize; ++y)
      for(x = -hsize; x < hsize; ++x,p++)
      {
        float xx = (size/(size-2.0f))*((float)x)+0.5f;
        float yy = (size/(size-2.0f))*((float)y)+0.5f;
        float dd = sqrt(xx*xx + yy*yy);
        float dstf = dd/((float)hsize+1);
        float phase = (float)pow(1.0f - (float)fabs((float)cos(angle+arms*(float)atan2(xx,yy)))*(star_flower+(1-star_flower)*(((dstf)))),attenuation);
        if (phase > 2.0f) phase = 1.0f;
        float pf = (255.0f * (cos(((dstf * PI_FLOAT/2.0f)))*phase));
        if (pf > 255.0f) pf = 255.0f;
        if (pf < 0.0f) pf = 0.0f;
        *p = (long)pf;
        dist = cos(dstf * PI_FLOAT/2.0f)*phase;
        if (((module_bitmap_generators_blob*)ptr)->work_alpha == 1)
        {
          long pr = MAX(0,MIN(255,(long)(255.0f *  ((module_bitmap_generators_blob*)ptr)->work_color[0])));
          long pg = MAX(0,MIN(255,(long)(255.0f *  ((module_bitmap_generators_blob*)ptr)->work_color[1])));
          long pb = MAX(0,MIN(255,(long)(255.0f *  ((module_bitmap_generators_blob*)ptr)->work_color[2])));
          long pa = MAX(0,MIN(255,(long)(255.0f * dist * ((module_bitmap_generators_blob*)ptr)->work_color[3])));
          *p = 0x01000000 * pa | pb * 0x00010000 | pg * 0x00000100 | pr;
        } else
        if (((module_bitmap_generators_blob*)ptr)->work_alpha == 0) {
          long pr = MAX(0,MIN(255,(long)(255.0f * dist * ((module_bitmap_generators_blob*)ptr)->work_color[0])));
          long pg = MAX(0,MIN(255,(long)(255.0f * dist * ((module_bitmap_generators_blob*)ptr)->work_color[1])));
          long pb = MAX(0,MIN(255,(long)(255.0f * dist * ((module_bitmap_generators_blob*)ptr)->work_color[2])));
          long pa = (long)(255.0f * ((module_bitmap_generators_blob*)ptr)->work_color[3]);
          *p = 0x01000000 * pa | pb * 0x00010000 | pg * 0x00000100 | pr;
        }
      }
    ((module_bitmap_generators_blob*)ptr)->work_bitmap->timestamp++;
    ((module_bitmap_generators_blob*)ptr)->loading_done = true;
    ((module_bitmap_generators_blob*)ptr)->thread_state = 2;
    int *retval = new int;
    *retval = 0;
    pthread_exit(NULL);
    // the thread will die here.
    return 0;
  }

  void run()
  {
    // initialize our worker thread, we don't want to keep the renderloop waiting do we?
    if (thread_state == 2)
    {
      if (bitm_timestamp != bitmap.timestamp)
      {
        worker_running = false;
        pthread_join(worker_t, NULL);

        // ok, new version
        bitm_timestamp = bitmap.timestamp;
        if (c_type == 1)
        {
          vsx_texture_gl_loader::upload_bitmap_2d(texture->texture, &bitmap, true);
          texture_out->set(texture);
        }
        bitmap_out->set(&bitmap);
      }
      thread_state = 3;
    }

    if (!worker_running)
    if (p_updates != param_updates)
    {
      //need_to_rebuild = false;
      if (i_size != 8 << size_in->get())
      {
        i_size = 8 << size_in->get();
        if (bitmap.data_get())
          to_delete_data = bitmap.data_get();

        bitmap.data_set( malloc( sizeof(vsx_bitmap_32bt) * i_size * i_size ) );
        bitmap.height = bitmap.width = i_size;
      }

      p_updates = param_updates;

      work_alpha = alpha_in->get();
      work_color[0] = MIN(1.0f, color_in->get(0));
      work_color[1] = MIN(1.0f, color_in->get(1));
      work_color[2] = MIN(1.0f, color_in->get(2));
      work_color[3] = MIN(1.0f, color_in->get(3));
      thread_state = 1;

      pthread_attr_init(&attr);

      worker_running = true;
      pthread_create(&worker_t, &attr, &worker, (void*)this);
    }

    if (to_delete_data)
    {
      delete[] (vsx_bitmap_32bt*)to_delete_data;
      to_delete_data = 0;
    }
  }

  void start()
  {
    if (c_type == 1)
    {
      texture->texture->init_opengl_texture_2d();
      vsx_texture_gl_loader::upload_bitmap_2d(texture->texture, &bitmap, true);
      texture_out->set(texture);
    }
  }

  void stop()
  {
    if (c_type == 1)
    {
      texture->texture->unload();
    }
  }

  void on_delete()
  {
    // wait for thread to finish
    if (worker_running)
      pthread_join(worker_t,NULL);

    if (c_type == 1 && texture)
    {
      texture->texture->unload();
      delete texture;
    }
  }

};
