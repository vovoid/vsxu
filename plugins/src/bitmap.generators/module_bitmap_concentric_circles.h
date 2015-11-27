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
	bool need_to_rebuild;

	vsx_bitmap bitm;
	int bitm_timestamp;

  vsx_texture* texture;
  pthread_t	worker_t;
  pthread_attr_t attr;

  int p_updates;
  int my_ref;


  vsx_bitmap*       work_bitmap;
  bool              worker_running;
  bool              thread_created;
  int               thread_state;
  int               i_size;
  float             work_color[4];
  int               work_alpha;

  void *to_delete_data;


public:

  int               c_type;

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
    loading_done = true;
    thread_state = 0;
    worker_running = false;
    thread_created = false;
    p_updates = -1;
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
    i_size = 0;
    bitmap_out = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
    work_bitmap = &bitm;
    bitm_timestamp = bitm.timestamp;
    need_to_rebuild = true;
    my_ref = 0;
    if (c_type == 1) {
      texture = new vsx_texture();
      texture->texture_gl->init_opengl_texture_2d();
      texture_out = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture");
      texture_out->set(texture);
    }
    to_delete_data = 0;
  }


  // our worker thread, to keep the tough generating work off the main loop
  // this is a fairly simple operation, but when you want to generate fractals
  // and decode film, you could run into several seconds of processing time.
  static void* worker(void *ptr)
  {
    int x,y;
    float attenuation = ((module_bitmap_generators_concentric_circles*)ptr)->attenuation_in->get();
    float frequency = ((module_bitmap_generators_concentric_circles*)ptr)->frequency_in->get() * 2.0f;

    vsx_bitmap_32bt *p = (vsx_bitmap_32bt*)((module_bitmap_generators_concentric_circles*)ptr)->work_bitmap->data;

    int size = ((module_bitmap_generators_concentric_circles*)ptr)->i_size;

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

        if (((module_bitmap_generators_concentric_circles*)ptr)->work_alpha == 1)
        {
          long pr = MAX(0,MIN(255,(long)(255.0f *  ((module_bitmap_generators_concentric_circles*)ptr)->work_color[0])));
          long pg = MAX(0,MIN(255,(long)(255.0f *  ((module_bitmap_generators_concentric_circles*)ptr)->work_color[1])));
          long pb = MAX(0,MIN(255,(long)(255.0f *  ((module_bitmap_generators_concentric_circles*)ptr)->work_color[2])));
          long pa = MAX(0,MIN(255,(long)(255.0f * dist * ((module_bitmap_generators_concentric_circles*)ptr)->work_color[3])));
          *p = 0x01000000 * pa | pb * 0x00010000 | pg * 0x00000100 | pr;
        } else
        if (((module_bitmap_generators_concentric_circles*)ptr)->work_alpha == 0) {
          long pr = MAX(0,MIN(255,(long)(255.0f * dist * ((module_bitmap_generators_concentric_circles*)ptr)->work_color[0])));
          long pg = MAX(0,MIN(255,(long)(255.0f * dist * ((module_bitmap_generators_concentric_circles*)ptr)->work_color[1])));
          long pb = MAX(0,MIN(255,(long)(255.0f * dist * ((module_bitmap_generators_concentric_circles*)ptr)->work_color[2])));
          long pa = (long)(255.0f * ((module_bitmap_generators_concentric_circles*)ptr)->work_color[3]);
          *p = 0x01000000 * pa | pb * 0x00010000 | pg * 0x00000100 | pr;
        }
      }
    ((module_bitmap_generators_concentric_circles*)ptr)->work_bitmap->timestamp++;
    ((module_bitmap_generators_concentric_circles*)ptr)->work_bitmap->valid = true;
    ((module_bitmap_generators_concentric_circles*)ptr)->loading_done = true;
    ((module_bitmap_generators_concentric_circles*)ptr)->thread_state = 2;
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
      if (bitm.valid && bitm_timestamp != bitm.timestamp)
      {
        worker_running = false;
        pthread_join(worker_t, NULL);

        // ok, new version
        bitm_timestamp = bitm.timestamp;
        if (c_type == 1)
        {
          texture->texture_gl->init_opengl_texture_2d();
          vsx_texture_gl_loader::upload_bitmap_2d(texture->texture_gl, &bitm, false, true);
          texture_out->set(texture);
        }
        bitmap_out->set(&bitm);
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
        if (bitm.data)
        {
          to_delete_data = bitm.data;
        }
        bitm.data = new vsx_bitmap_32bt[i_size*i_size];

        bitm.width  = i_size;
        bitm.height = i_size;
      }

      p_updates = param_updates;
      bitm.valid = false;

      work_alpha = alpha_in->get();
      work_color[0] = MIN(1.0f,color_in->get(0));
      work_color[1] = MIN(1.0f,color_in->get(1));
      work_color[2] = MIN(1.0f,color_in->get(2));
      work_color[3] = MIN(1.0f,color_in->get(3));
      thread_state = 1;

      pthread_attr_init(&attr);

      worker_running = true;
      pthread_create(&worker_t, &attr, &worker, (void*)this);
      thread_created = true;
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
      if (bitm.valid)
      {
        texture->texture_gl->init_opengl_texture_2d();
        vsx_texture_gl_loader::upload_bitmap_2d(texture->texture_gl, &bitm, false, true);
      }
      texture_out->set(texture);
    }
  }

  void stop()
  {
    if (c_type == 1)
    {
      texture->texture_gl->unload();
    }
  }

  void on_delete()
  {
    // wait for thread to finish
    if (worker_running)
    {
      pthread_join(worker_t,NULL);
    }

    if (c_type == 1 && texture)
    {
      texture->texture_gl->unload();
      delete texture;
    }
    delete[] (vsx_bitmap_32bt*)bitm.data;
  }

};
