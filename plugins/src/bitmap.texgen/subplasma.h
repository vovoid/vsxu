

unsigned char catmullrom_interpolate(int v0, int v1, int v2, int v3, float xx)
{
  
  
	int a =v0 - v1;
	int P =v3 - v2 - a;
	int Q =a - P;
	int R =v2 - v0;
	int t=(int)(v1+xx*(R+xx*(Q+xx*P)));
//	int b;
	/*asm
	(	"movd %%mm0 , %1\n\t"
		"packuswb %%mm0, %%mm0\n\t"
		"movd %0, %%mm0\n\t"
		"emms\n\t"
		: "=r"(b)
		: "r"(t)
		:
	);*/
	if (t > 255) return 255; else
	if (t < 0) return 0; else
//	return (unsigned char)t;
	return (unsigned char)t;
}



class module_bitmap_subplasma : public vsx_module {
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
  vsx_module_param_float* rand_seed;

  vsx_module_param_int* size;
  vsx_module_param_int* amplitude;
  
  vsx_bitmap*       work_bitmap;
  bool              worker_running;
  int               thread_state;
  int               i_size;

  // our worker thread, to keep the tough generating work off the main loop
  // this is a fairly simple operation, but when you want to generate fractals
  // and decode film, you could run into several seconds of processing time. 
  static void* worker(void *ptr) {
    
    int x,y;
    module_bitmap_subplasma* mod = ((module_bitmap_subplasma*)ptr);
    /*float rpx = mod->r_period->get(0);
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
    
    float ramp = mod->col_amp->get(0)*127.0;
    float gamp = mod->col_amp->get(1)*127.0;
    float bamp = mod->col_amp->get(2)*127.0;
    float aamp = mod->col_amp->get(3)*127.0;

    float rofs = mod->col_ofs->get(0)*127.0;
    float gofs = mod->col_ofs->get(1)*127.0;
    float bofs = mod->col_ofs->get(2)*127.0;
    float aofs = mod->col_ofs->get(3)*127.0;

    float attenuation = ((module_bitmap_subplasma*)ptr)->attenuation->get();
    float arms = ((module_bitmap_subplasma*)ptr)->arms->get()*0.5f;
    float star_flower = ((module_bitmap_subplasma*)ptr)->star_flower->get();
    float angle = ((module_bitmap_subplasma*)ptr)->angle->get();*/
//    int gsize = ((module_bitmap_subplasma*)ptr)->i_size;
    //int hsize = ssize >> 1;
    //float sp1 = (float)size + 1.0f;
    //float size = (2.0f*pi)/(float)ssize;
  
  unsigned char* SubPlasma = new unsigned char[mod->i_size * mod->i_size];
  for (x = 0; x < mod->i_size*mod->i_size; ++x) { SubPlasma[x] = 0; }
	int np=2 << mod->amplitude->get();
	//int rx=256;
	//int ry=256;
	
	unsigned int musize = mod->i_size-1;

	//int ssize=rx/np;
	
	unsigned int mmu = (unsigned int)(((float)mod->i_size/(float)np));
	//printf("mmu: %d\n", mmu);
	unsigned int mm1 = mmu-1;
	unsigned int mm2 = mmu*2;
	float mmf = (float)mmu;



	srand((int)mod->rand_seed->get());
	for (y=0; y < np; y++)
		for (x=0; x < np; x++)
			SubPlasma[x*mmu+y*mmu*mod->i_size] = rand();

	for (y=0; y<np; y++)
		for (x=0; x<mod->i_size; x++) {
			int p=x&(~mm1);
			int zy=y*mmu*mod->i_size;
			SubPlasma[x+zy] = catmullrom_interpolate(
				SubPlasma[((p-mmu)&musize)+zy],
				SubPlasma[((p   )&musize)+zy],
				SubPlasma[((p+mmu)&musize)+zy],
				SubPlasma[((p+mm2)&musize)+zy],
				(x&mm1)/mmf);
		}
		
	int sl = mod->size->get()+3;
	for (y=0; y<mod->i_size; y++)
		for (x=0; x<mod->i_size; x++) {
			int p=y&(~(mm1));
			SubPlasma[x+y*mod->i_size] = catmullrom_interpolate(
				SubPlasma[x+(((p-mmu)&musize)<<sl)],
				SubPlasma[x+(((p   )&musize)<<sl)],
				SubPlasma[x+(((p+mmu)&musize)<<sl)],
				SubPlasma[x+(((p+mm2)&musize)<<sl)],
				(y&(mm1))/mmf);
		}

	unsigned long *p = ((module_bitmap_subplasma*)ptr)->work_bitmap->data;
    
	
    for (x = 0; x < mod->i_size * (mod->i_size); ++x, p++) 
    {
      //if (SubPlasma[x] != 0)
      //printf("SubPlasma[x] = %d,", SubPlasma[x]);
      *p = 0xFF000000 | ((unsigned long)SubPlasma[x]) << 16 | (unsigned long)SubPlasma[x] << 8 | (unsigned long)SubPlasma[x];// | 0 * 0x00000100 | 0;
      
    }
  	/*for(y = -hsize; y < hsize; ++y)
  		for(x = -hsize; x < hsize; ++x,p++)
  		{
  			float xx = (size/(size-2.0f))*((float)x)+0.5f;
  			float yy = (size/(size-2.0f))*((float)y)+0.5f;
  			long r = (long)round(fmod(fabs((sin((x*size+rox)*rpx)*sin((y*size+roy)*rpy)+1.0f)*ramp+rofs),255.0));
  			long g = (long)round(fmod(fabs((sin((x*size+gox)*gpx)*sin((y*size+goy)*gpy)+1.0f)*gamp+gofs),255.0));
  			long b = (long)round(fmod(fabs((sin((x*size+box)*bpx)*sin((y*size+boy)*bpy)+1.0f)*bamp+bofs),255.0));
  			long a = (long)round(fmod(fabs((sin((x*size+aox)*apx)*sin((y*size+aoy)*apy)+1.0f)*aamp+aofs),255.0));
  			*p = 0x01000000 * a | b * 0x00010000 | g * 0x00000100 | r;
  		}*/
    ((module_bitmap_subplasma*)ptr)->work_bitmap->timestamp++;
    ((module_bitmap_subplasma*)ptr)->work_bitmap->valid = true;
    ((module_bitmap_subplasma*)ptr)->thread_state = 2;
    //printf("blob thread done\n");
    // the thread will die here.
    return 0;
  }
  
  void module_info(vsx_module_info* info)
  {
    info->in_param_spec = "rand_seed:float,size:enum?8x8|16x16|32x32|64x64|128x128|256x256|512x512|1024x1024,\
amplitude:enum?2|4|8|16|32|64|128|256|512";
      info->identifier = "bitmaps;generators;subplasma";
      info->out_param_spec = "bitmap:bitmap";
      info->component_class = "bitmap";
    info->description = "Generates a plasma bitmap\nThanks to BoyC of Conspiracy \nfor the base code of this!";
  }
  
  /*void param_set_notify(const vsx_string& name) {
    need_to_rebuild = true;
  };*/
  
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    thread_state = 0;
    worker_running = false;
    p_updates = -1;


    rand_seed = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"rand_seed");
    rand_seed->set(4.0f);
    
    size = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"size");
    size->set(4);

    amplitude = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"amplitude");


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
    //bitm.data = new unsigned long[256*256];
    //bitm.size_y = bitm.size_x = 256;
    to_delete_data = 0;
  }
  unsigned long *to_delete_data;
  void run() {
    // initialize our worker thread, we don't want to keep the renderloop waiting do we?
    if (!worker_running)
    if (p_updates != param_updates) {
      //need_to_rebuild = false;
      if (i_size != 8 << size->get()) {
        i_size = 8 << size->get();
        if (bitm.data) to_delete_data = bitm.data;   
          //delete[] bitm.data;
        bitm.data = new unsigned long[i_size*i_size];
        bitm.size_y = bitm.size_x = i_size;
      }

      p_updates = param_updates;
      bitm.valid = false;
      //printf("creating thread\n");
      pthread_attr_init(&worker_t_attr);
      thread_state = 1;
      worker_running = true;
      pthread_create(&worker_t, &worker_t_attr, &worker, (void*)this);   
      pthread_detach(worker_t);
      
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
      delete[] to_delete_data;
      to_delete_data = 0;
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
    //printf("c");
    delete[] bitm.data;
    //printf("d");
  }
};
