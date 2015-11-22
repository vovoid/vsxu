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



class module_bitmap_plasma : public vsx_module
{
public:
  // in
  vsx_module_param_float* arms_in;
  vsx_module_param_float* attenuation_in;
  vsx_module_param_float* star_flower_in;
  vsx_module_param_float* angle_in;


  vsx_module_param_float4* col_amp_in;
  vsx_module_param_float4* col_ofs_in;

  vsx_module_param_float3* r_period_in;
  vsx_module_param_float3* g_period_in;
  vsx_module_param_float3* b_period_in;
  vsx_module_param_float3* a_period_in;

  vsx_module_param_float3* r_ofs_in;
  vsx_module_param_float3* g_ofs_in;
  vsx_module_param_float3* b_ofs_in;
  vsx_module_param_float3* a_ofs_in;
  vsx_module_param_int* size_in;

  // out
  vsx_module_param_bitmap* bitmap_out;

  // internal
  bool need_to_rebuild;

  vsx_bitmap bitm;
  int bitm_timestamp;

  pthread_t	worker_t;

  int p_updates;
  int my_ref;

  
  vsx_bitmap*       work_bitmap;
  bool              thread_created;
  bool              worker_running;
  int               thread_state;
  int               i_size;

  // our worker thread, to keep the tough generating work off the main loop
  // this is a fairly simple operation, but when you want to generate fractals
  // and decode film, you could run into several seconds of processing time. 
  static void* worker(void *ptr) {
    
    ((module_bitmap_plasma*)ptr)->worker_running = true;
    int x,y;
    module_bitmap_plasma* mod = ((module_bitmap_plasma*)ptr);
    float rpx = mod->r_period_in->get(0);
    float rpy = mod->r_period_in->get(1);
    float gpx = mod->g_period_in->get(0);
    float gpy = mod->g_period_in->get(1);
    float bpx = mod->b_period_in->get(0);
    float bpy = mod->b_period_in->get(1);
    float apx = mod->a_period_in->get(0);
    float apy = mod->a_period_in->get(1);

    float rox = mod->r_ofs_in->get(0);
    float roy = mod->r_ofs_in->get(1);
    float gox = mod->g_ofs_in->get(0);
    float goy = mod->g_ofs_in->get(1);
    float box = mod->b_ofs_in->get(0);
    float boy = mod->b_ofs_in->get(1);
    float aox = mod->a_ofs_in->get(0);
    float aoy = mod->a_ofs_in->get(1);
    
    float ramp = mod->col_amp_in->get(0)*127.0f;
    float gamp = mod->col_amp_in->get(1)*127.0f;
    float bamp = mod->col_amp_in->get(2)*127.0f;
    float aamp = mod->col_amp_in->get(3)*127.0f;

    float rofs = mod->col_ofs_in->get(0)*127.0f;
    float gofs = mod->col_ofs_in->get(1)*127.0f;
    float bofs = mod->col_ofs_in->get(2)*127.0f;
    float aofs = mod->col_ofs_in->get(3)*127.0f;

    vsx_bitmap_32bt *p = (vsx_bitmap_32bt*)((module_bitmap_plasma*)ptr)->work_bitmap->data;
    int ssize = ((module_bitmap_plasma*)ptr)->i_size;
    int hsize = ssize >> 1;
    float size = (float)(2.0f*PI)/(float)ssize;
    
    for(y = -hsize; y < hsize; ++y)
    {
      for(x = -hsize; x < hsize; ++x,p++)
      {
        long r = (long)round(fmod(fabs((sin((x*size+rox)*rpx)*sin((y*size+roy)*rpy)+1.0f)*ramp+rofs),255.0));
        long g = (long)round(fmod(fabs((sin((x*size+gox)*gpx)*sin((y*size+goy)*gpy)+1.0f)*gamp+gofs),255.0));
        long b = (long)round(fmod(fabs((sin((x*size+box)*bpx)*sin((y*size+boy)*bpy)+1.0f)*bamp+bofs),255.0));
        long a = (long)round(fmod(fabs((sin((x*size+aox)*apx)*sin((y*size+aoy)*apy)+1.0f)*aamp+aofs),255.0));
        *p = 0x01000000 * a | b * 0x00010000 | g * 0x00000100 | r;

      }
    }// for y
    ((module_bitmap_plasma*)ptr)->work_bitmap->timestamp++;
    ((module_bitmap_plasma*)ptr)->work_bitmap->valid = true;
    ((module_bitmap_plasma*)ptr)->thread_state = 2;
    return 0;
  }
  
  void module_info(vsx_module_info* info)
  {
    info->in_param_spec =
      "settings:complex{"
        "col_amp:float4?default_controller=controller_col,"
        "col_ofs:float4?default_controller=controller_col,"
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
      "size:enum?8x8|16x16|32x32|64x64|128x128|256x256|512x512|1024x1024"
    ;

    info->identifier = "bitmaps;generators;plasma";
    info->out_param_spec = "bitmap:bitmap";
    info->component_class = "bitmap";
    info->description = "Generates a Sin-plasma bitmap";
  }
  
  
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    thread_state = 0;
    worker_running = false;
    thread_created = false;
    p_updates = -1;

    col_amp_in = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"col_amp");
    col_amp_in->set(1.0f,0);
    col_amp_in->set(1.0f,1);
    col_amp_in->set(1.0f,2);
    col_amp_in->set(1.0f,3);
    col_ofs_in = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"col_ofs");


    r_period_in = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"r_period");
    r_period_in->set(1.0f,0);
    r_period_in->set(1.0f,1);
    g_period_in = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"g_period");
    g_period_in->set(1.0f,0);
    g_period_in->set(16.0f,1);
    b_period_in = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"b_period");
    a_period_in = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"a_period");

    r_ofs_in = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"r_ofs");
    g_ofs_in = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"g_ofs");
    b_ofs_in = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"b_ofs");
    a_ofs_in = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"a_ofs");

    arms_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"arms");

    attenuation_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"attenuation");
    attenuation_in->set(0.1f);

    size_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"size");
    size_in->set(4);
    
    star_flower_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"star_flower");

    angle_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"angle");
    i_size = 0;
    bitmap_out = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
    work_bitmap = &bitm;
    my_ref = 0;
    bitm_timestamp = bitm.timestamp = rand();
    need_to_rebuild = true;
    to_delete_data = 0;
  }
  void *to_delete_data;
  void run() {
    // initialize our worker thread, we don't want to keep the renderloop waiting do we?
    if (!worker_running)
    if (p_updates != param_updates) {
      //need_to_rebuild = false;
      if (i_size != 8 << size_in->get()) {
        i_size = 8 << size_in->get();
        if (bitm.data) to_delete_data = bitm.data;
        //if (bitm.data) delete[] bitm.data;
        bitm.data = new vsx_bitmap_32bt[i_size*i_size];

        bitm.width  = i_size;
        bitm.height = i_size;
      }

      p_updates = param_updates;
      bitm.valid = false;
      //printf("creating thread\n");
      thread_state = 1;
      thread_created = true;
      worker_running = true;
      pthread_create(&worker_t, NULL, &worker, (void*)this);

      //printf("done creating thread\n");
    }
    if (thread_state == 2) {
      if (bitm.valid && bitm_timestamp != bitm.timestamp) {
        pthread_join(worker_t,NULL);
        worker_running = false;
        // ok, new version
        //printf("uploading blob to vram\n");
        bitm_timestamp = bitm.timestamp;
        bitmap_out->set(&bitm);
        loading_done = true;
        if (to_delete_data)
        {
          delete[] (vsx_bitmap_32bt*)to_delete_data;
          to_delete_data = 0;
        }
      }
      thread_state = 3;
    }  
  }
  void start() {
  }  
  
  void stop() {}
  
  void on_delete() {
    if (worker_running)
    {
      // wait for thread to finish
      void* ret;
      pthread_join(worker_t,&ret);
    }
    if  (bitm.data)
    {
      delete[] (vsx_bitmap_32bt*)bitm.data;
    }
  }
};
