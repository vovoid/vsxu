#ifndef MIN_MAX_STATIC
  #define MIN_MAX_STATIC
inline float max (float x, float a)
{
   x -= a;
   x += fabs (x);
   x *= 0.5;
   x += a;
   return (x);
}

inline float min (float x, float b)
{
   x = b - x;
   x += fabs (x);
   x *= 0.5;
   x = b - x;
   return (x);
}
#endif

class module_bitmap_blob : public vsx_module {
  // in

	// out
	vsx_module_param_bitmap* result1;
	vsx_module_param_texture* result_texture;
	// internal
	bool need_to_rebuild;

	vsx_bitmap bitm;
	int bitm_timestamp;

  vsx_texture* texture;
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
  vsx_module_param_float4* color;
  vsx_module_param_int* alpha;
  vsx_module_param_int* size;

  vsx_bitmap*       work_bitmap;
  bool              worker_running;
  int               thread_state;
  int               c_type;
  int               i_size;
  float             work_color[4];
  int               work_alpha;

  // our worker thread, to keep the tough generating work off the main loop
  // this is a fairly simple operation, but when you want to generate fractals
  // and decode film, you could run into several seconds of processing time.
  static void* worker(void *ptr) {

    ((module_bitmap_blob*)ptr)->worker_running = true;
    int x,y;
    float attenuation = ((module_bitmap_blob*)ptr)->attenuation->get();
    float arms = ((module_bitmap_blob*)ptr)->arms->get()*0.5f;
    float star_flower = ((module_bitmap_blob*)ptr)->star_flower->get();
    float angle = ((module_bitmap_blob*)ptr)->angle->get();
    unsigned long *p = (unsigned long*)((module_bitmap_blob*)ptr)->work_bitmap->data;
    int size = ((module_bitmap_blob*)ptr)->i_size;
    //float sp1 = (float)size + 1.0f;
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
        *p = (long)(255.0f * (cos(((dstf * pi/2.0f)))*phase));
        if (*p > 255) *p = 255;
        if (*p < 0) *p = 0;
        dist = cos(dstf * pi/2.0f)*phase;
  			if (((module_bitmap_blob*)ptr)->work_alpha == 1)
  			{
          long pr = max(0,min(255,(long)(255.0f *  ((module_bitmap_blob*)ptr)->work_color[0])));
          long pg = max(0,min(255,(long)(255.0f *  ((module_bitmap_blob*)ptr)->work_color[1])));
          long pb = max(0,min(255,(long)(255.0f *  ((module_bitmap_blob*)ptr)->work_color[2])));
          long pa = max(0,min(255,(long)(255.0f * dist * ((module_bitmap_blob*)ptr)->work_color[3])));
          *p = 0x01000000 * pa | pb * 0x00010000 | pg * 0x00000100 | pr;
//  			  *p = 0xFF000000 | *p * 0x00010000 | *p * 0x00000100 | *p;
  			} else
  			if (((module_bitmap_blob*)ptr)->work_alpha == 0) {
          long pr = max(0,min(255,(long)(255.0f * dist * ((module_bitmap_blob*)ptr)->work_color[0])));
          long pg = max(0,min(255,(long)(255.0f * dist * ((module_bitmap_blob*)ptr)->work_color[1])));
          long pb = max(0,min(255,(long)(255.0f * dist * ((module_bitmap_blob*)ptr)->work_color[2])));
          long pa = (long)(255.0f * ((module_bitmap_blob*)ptr)->work_color[3]);
  			  *p = 0x01000000 * pa | pb * 0x00010000 | pg * 0x00000100 | pr;
  			}
  		}
    ((module_bitmap_blob*)ptr)->work_bitmap->timestamp++;
    ((module_bitmap_blob*)ptr)->work_bitmap->valid = true;
    ((module_bitmap_blob*)ptr)->thread_state = 2;
    // the thread will die here.
    return 0;
  }

  void module_info(vsx_module_info* info)
  {
    info->identifier = "bitmaps;generators;blob||bitmaps;generators;particles;blob";
    info->in_param_spec = ""
        "settings:complex{"
          "arms:float,"
          "attenuation:float,"
          "star_flower:float,"
          "angle:float,"
          "color:float4,"
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

  /*void param_set_notify(const vsx_string& name) {
    need_to_rebuild = true;
  };*/

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    thread_state = 0;
    worker_running = false;
    p_updates = -1;
    arms = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"arms");
    attenuation = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"attenuation");
    attenuation->set(0.1f);
    size = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"size");
    size->set(4);
    alpha = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"alpha");
    alpha->set(0);
    color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"color");
    color->set(1.0f,0);
    color->set(1.0f,1);
    color->set(1.0f,2);
    color->set(1.0f,3);
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
    bitm_timestamp = bitm.timestamp;
    need_to_rebuild = true;
    my_ref = 0;
    bitm.ref = &my_ref;
    if (c_type == 1) {
      texture = new vsx_texture;
      texture->init_opengl_texture();
      result_texture = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture");
      result_texture->set(texture);
    }
    to_delete_data = 0;
  }
  void *to_delete_data;
  void run() {
    //printf("param_updates: %d\n",param_updates);
    //printf("p_updates: %d\n",p_updates);
    // initialize our worker thread, we don't want to keep the renderloop waiting do we?
    if (thread_state == 2) {
      if (bitm.valid && bitm_timestamp != bitm.timestamp) {
        //pthread_join(worker_t,0);
        worker_running = false;
        // ok, new version
        //printf("uploading blob to vram\n");
        bitm_timestamp = bitm.timestamp;
        if (c_type == 1)
        texture->upload_ram_bitmap(&bitm,true);
        if (c_type == 1)
        result_texture->set(texture);
        result1->set_p(bitm);
        loading_done = true;
      }
      thread_state = 3;
    }

    if (!worker_running)
    if (p_updates != param_updates) {
      //printf("param updates changed\n");
      //need_to_rebuild = false;
      if (i_size != 8 << size->get()) {
        i_size = 8 << size->get();
        //printf("i_size: %d\n",i_size);
        if (bitm.data) to_delete_data = bitm.data;
        bitm.data = new unsigned long[i_size*i_size];
        bitm.size_y = bitm.size_x = i_size;
      }

      p_updates = param_updates;
      bitm.valid = false;
      //printf("creating thread\n");
      work_alpha = alpha->get();
      work_color[0] = min(1.0f,color->get(0));
      work_color[1] = min(1.0f,color->get(1));
      work_color[2] = min(1.0f,color->get(2));
      work_color[3] = min(1.0f,color->get(3));
      pthread_attr_init(&worker_t_attr);
      thread_state = 1;
      pthread_create(&worker_t, &worker_t_attr, &worker, (void*)this);
      pthread_detach(worker_t);

      //printf("done creating thread\n");
    }

    if (to_delete_data && my_ref == 0)
    {
      delete[] (unsigned long*)to_delete_data;
      to_delete_data = 0;
    }
  }
  void start() {
    if (c_type == 1) {
      if (bitm.valid) {
        texture->init_opengl_texture();
        texture->upload_ram_bitmap(&bitm,true);
      }
      result_texture->set(texture);
    }
  }

  void stop() {
    //if (worker_running) {
      //printf("a");
      //pthread_join(worker_t,0);
      //printf("b");
      //worker_running = false;
    //}
    if (c_type == 1) {
      //printf("c");
      //delete texture->transform_obj;
      //printf("d");
      texture->unload();
      //printf("e");
      //delete texture;
      //texture = 0;
    }
  }

  void on_delete() {
    //printf("deleting bitmap..");
    if (thread_state == 1) {
      while (thread_state != 2) Sleep(1);
      pthread_cancel(worker_t);
    }
    //printf("a");
    //printf("b");
    if (c_type == 1) {
      if (texture) {
        //printf(":t:");
        texture->unload();
        delete texture;
      }
    }
    //printf("c");
    delete[] (unsigned long*)bitm.data;
    //printf("d");
  }
};
