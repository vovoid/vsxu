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


class module_bitmap_plasma : public vsx_module {
  // in
	
	// out
	vsx_module_param_bitmap* result1;
	// internal
	bool need_to_rebuild;
	
	vsx_bitmap bitm;
	int bitm_timestamp;
	
  pthread_t					worker_t;
  pthread_attr_t		worker_t_attr;

  int p_updates;
  int my_ref;

public:
  //vsx_module_param_float* star_offset;
  vsx_module_param_float* arms;
  vsx_module_param_float* attenuation;
  vsx_module_param_float* star_flower;
  vsx_module_param_float* angle;


  vsx_module_param_float4* col_amp;
  vsx_module_param_float4* col_ofs;

  vsx_module_param_float3* r_period;
  vsx_module_param_float3* g_period;
  vsx_module_param_float3* b_period;
  vsx_module_param_float3* a_period;

  vsx_module_param_float3* r_ofs;
  vsx_module_param_float3* g_ofs;
  vsx_module_param_float3* b_ofs;
  vsx_module_param_float3* a_ofs;
  vsx_module_param_int* size;
  
  vsx_bitmap*       work_bitmap;
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
    float rpx = mod->r_period->get(0);
    float rpy = mod->r_period->get(1);
    float gpx = mod->g_period->get(0);
    float gpy = mod->g_period->get(1);
    float bpx = mod->b_period->get(0);
    float bpy = mod->b_period->get(1);
    float apx = mod->a_period->get(0);
    float apy = mod->a_period->get(1);

    float rox = mod->r_ofs->get(0);
    float roy = mod->r_ofs->get(1);
    float gox = mod->g_ofs->get(0);
    float goy = mod->g_ofs->get(1);
    float box = mod->b_ofs->get(0);
    float boy = mod->b_ofs->get(1);
    float aox = mod->a_ofs->get(0);
    float aoy = mod->a_ofs->get(1);
    
    float ramp = mod->col_amp->get(0)*127.0f;
    float gamp = mod->col_amp->get(1)*127.0f;
    float bamp = mod->col_amp->get(2)*127.0f;
    float aamp = mod->col_amp->get(3)*127.0f;

    float rofs = mod->col_ofs->get(0)*127.0f;
    float gofs = mod->col_ofs->get(1)*127.0f;
    float bofs = mod->col_ofs->get(2)*127.0f;
    float aofs = mod->col_ofs->get(3)*127.0f;

    //float attenuation = ((module_bitmap_plasma*)ptr)->attenuation->get();
    //float arms = ((module_bitmap_plasma*)ptr)->arms->get()*0.5f;
    //float star_flower = ((module_bitmap_plasma*)ptr)->star_flower->get();
    //float angle = ((module_bitmap_plasma*)ptr)->angle->get();
    vsx_bitmap_32bt *p = (vsx_bitmap_32bt*)((module_bitmap_plasma*)ptr)->work_bitmap->data;
    int ssize = ((module_bitmap_plasma*)ptr)->i_size;
    int hsize = ssize >> 1;
    //float sp1 = (float)size + 1.0f;
    float size = (float)(2.0f*PI)/(float)ssize;
    
  	for(y = -hsize; y < hsize; ++y)
  		for(x = -hsize; x < hsize; ++x,p++)
  		{
  			//float xx = (size/(size-2.0f))*((float)x)+0.5f;
  			//float yy = (size/(size-2.0f))*((float)y)+0.5f;
  			long r = (long)round(fmod(fabs((sin((x*size+rox)*rpx)*sin((y*size+roy)*rpy)+1.0f)*ramp+rofs),255.0));
  			long g = (long)round(fmod(fabs((sin((x*size+gox)*gpx)*sin((y*size+goy)*gpy)+1.0f)*gamp+gofs),255.0));
  			long b = (long)round(fmod(fabs((sin((x*size+box)*bpx)*sin((y*size+boy)*bpy)+1.0f)*bamp+bofs),255.0));
  			long a = (long)round(fmod(fabs((sin((x*size+aox)*apx)*sin((y*size+aoy)*apy)+1.0f)*aamp+aofs),255.0));
  			//long a = 255;
  			//long g = 0;
  			//long b = 0;
  			//if((long)(dd) > hsize) *p = 0;
  			//else {
/*          float phase;
          float dstf = dd/((float)hsize+1);
          phase = pow(1 - fabs(cos(angle+arms*atan2(xx,yy)))*(star_flower+(1-star_flower)*(((dstf)))),attenuation);
          if (phase > 2.0) phase = 1.0;
          *p = (long)(255.0f * (cos(((dstf * PI/2.0f)))*phase));
          if (*p > 255) *p = 255;
          if (*p < 0) *p = 0;
        }*/
  			*p = 0x01000000 * a | b * 0x00010000 | g * 0x00000100 | r;
  			
  		}
    ((module_bitmap_plasma*)ptr)->work_bitmap->timestamp++;
    ((module_bitmap_plasma*)ptr)->work_bitmap->valid = true;
    ((module_bitmap_plasma*)ptr)->thread_state = 2;
    //printf("blob thread done\n");
    // the thread will die here.
    return 0;
  }
  
  void module_info(vsx_module_info* info)
  {
    info->in_param_spec = "settings:complex{\
col_amp:float4,\
col_ofs:float4,\
period:complex{\
r_period:float3,\
g_period:float3,\
b_period:float3,\
a_period:float3\
},\
ofs:complex{\
r_ofs:float3,\
g_ofs:float3,\
b_ofs:float3,\
a_ofs:float3\
}\
},\
size:enum?8x8|16x16|32x32|64x64|128x128|256x256|512x512|1024x1024";
      info->identifier = "bitmaps;generators;plasma";
      info->out_param_spec = "bitmap:bitmap";
      info->component_class = "bitmap";
    info->description = "Generates a plasma bitmap";
  }
  
  /*void param_set_notify(const vsx_string& name) {
    need_to_rebuild = true;
  };*/
  
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    thread_state = 0;
    worker_running = false;
    p_updates = -1;

    col_amp = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"col_amp");
    col_amp->set(1.0f,0);
    col_amp->set(1.0f,1);
    col_amp->set(1.0f,2);
    col_amp->set(1.0f,3);
    col_ofs = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"col_ofs");


    r_period = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"r_period");
    r_period->set(1.0f,0);
    r_period->set(1.0f,1);
    g_period = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"g_period");
    g_period->set(1.0f,0);
    g_period->set(16.0f,1);
    b_period = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"b_period");
    a_period = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"a_period");

    r_ofs = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"r_ofs");
    g_ofs = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"g_ofs");
    b_ofs = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"b_ofs");
    a_ofs = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"a_ofs");

    arms = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"arms");

    attenuation = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"attenuation");
    attenuation->set(0.1f);

    size = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"size");
    size->set(4);
    
    star_flower = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"star_flower");

    angle = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"angle");
    i_size = 0;
  	result1 = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
    result1->set_p(bitm);
    work_bitmap = &bitm;
    bitm.data = 0;
    bitm.bpp = 4;
    bitm.bformat = GL_RGBA;
    bitm.valid = false;
    my_ref = 0;
    bitm.ref = &my_ref;    
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
      if (i_size != 8 << size->get()) {
        i_size = 8 << size->get();
        if (bitm.data) to_delete_data = bitm.data;
        //if (bitm.data) delete[] bitm.data;
        bitm.data = new vsx_bitmap_32bt[i_size*i_size];
        bitm.size_y = bitm.size_x = i_size;
      }

      p_updates = param_updates;
      bitm.valid = false;
      //printf("creating thread\n");
      pthread_attr_init(&worker_t_attr);
      thread_state = 1;
      pthread_create(&worker_t, &worker_t_attr, &worker, (void*)this);   
      pthread_detach(worker_t);

      //printf("done creating thread\n");
    }
    if (thread_state == 2) {
      if (bitm.valid && bitm_timestamp != bitm.timestamp) {
        //pthread_join(worker_t,0);
        worker_running = false;
        // ok, new version
        //printf("uploading blob to vram\n");
        bitm_timestamp = bitm.timestamp;
        result1->set_p(bitm);
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
  
  void stop() {
    //if (worker_running) {
      //printf("a");
      //pthread_join(worker_t,0);
      //printf("b");
      //worker_running = false;
    //}
      //printf("c");
      //delete texture->transform_obj;
      //printf("d");
//      texture->unload();
      //printf("e");
      //delete texture;
      //texture = 0;
  }
  
  void on_delete() {
    //printf("deleting bitmap..");
    if (thread_state == 1) {
      while (thread_state != 2) Sleep(1);
      pthread_cancel(worker_t);
    }
    //printf("a");
    //printf("b");
    //printf("c");
    delete[] (vsx_bitmap_32bt*)bitm.data;
    //printf("d");
  }
};
