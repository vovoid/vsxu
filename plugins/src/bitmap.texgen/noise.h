class module_bitmap_add_noise : public vsx_module {
  // in
	vsx_module_param_bitmap* bitmap_in;
	float time;
	
	// out
	vsx_module_param_bitmap* result1;
	// internal
	vsx_bitmap* bitm;
  vsx_bitmap t_bitm;

	int buf, frame;
	unsigned long *data_a;
	unsigned long *data_b;
	int bitm_timestamp;
	vsx_bitmap result_bitm;
	bool first, worker_running, t_done;
  pthread_t					worker_t;
  pthread_attr_t		worker_t_attr;
  int my_ref;
	
public:



static void* noise_worker(void *ptr) {
  int i_frame = -1;
  //int x,y;
  bool buf = false;
  unsigned long *p;
  while (((module_bitmap_add_noise*)ptr)->worker_running) {
    if (i_frame != ((module_bitmap_add_noise*)ptr)->frame) {
    //printf("%d ",ptr);
      // time to run baby
      if (buf) p = ((module_bitmap_add_noise*)ptr)->data_a;
      else p = ((module_bitmap_add_noise*)ptr)->data_b;
      
      unsigned long b_c = ((module_bitmap_add_noise*)ptr)->result_bitm.size_x * ((module_bitmap_add_noise*)ptr)->result_bitm.size_y;
      
      //((module_bitmap_add_noise*)ptr)->result_bitm->data;
      //unsigned long cc = rand()<<8 | (char)rand();
      for (unsigned long x = 0; x < b_c; ++x) {
          p[x] = ((module_bitmap_add_noise*)ptr)->t_bitm.data[x] | rand() << 8  | (unsigned char)rand(); //bitm->data[x + y*result_bitm.size_x] 
          //((module_bitmap_add_noise*)ptr)->bitm->data[x]<<
      }
      ((module_bitmap_add_noise*)ptr)->result_bitm.valid = true;
      ((module_bitmap_add_noise*)ptr)->result_bitm.data = p;
      ++((module_bitmap_add_noise*)ptr)->result_bitm.timestamp;
      buf = !buf;
      i_frame = ((module_bitmap_add_noise*)ptr)->frame;
    }
    Sleep(100);
  }
  ((module_bitmap_add_noise*)ptr)->t_done = true;
  return 0;
}

void module_info(vsx_module_info* info)
{
  info->identifier = "bitmaps;modifiers;add_noise";
  info->in_param_spec = "bitmap_in:bitmap";
  info->out_param_spec = "bitmap:bitmap";
  info->component_class = "bitmap";
  info->description = "";
}

// ----

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
	bitmap_in = (vsx_module_param_bitmap*)in_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap_in");
	result1 = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
  result_bitm.size_x = 0;
  result_bitm.size_y = 0;
  result_bitm.bpp = 4;
  result_bitm.bformat = GL_RGBA;
  my_ref = 0;
  result_bitm.ref = &my_ref;  
  first = true;
  worker_running = false;
  buf = 0;
  frame = 0;
  t_done = false;
}

void run() {
  bitm = bitmap_in->get_addr();
  if (bitm) { 
    t_bitm = *bitm;
    if (result_bitm.size_x != bitm->size_x && result_bitm.size_y != bitm->size_y) {
      if (worker_running)
      pthread_join(worker_t,0);
      worker_running = false;
      
      // need to realloc
      if (result_bitm.valid) delete[] result_bitm.data;
      data_a = new unsigned long[bitm->size_x*bitm->size_y];
      data_b = new unsigned long[bitm->size_x*bitm->size_y];
      result_bitm.data = data_a;
      result_bitm.valid = true;
      result_bitm.size_x = bitm->size_x;
      result_bitm.size_y = bitm->size_y;

      pthread_attr_init(&worker_t_attr);
      pthread_create(&worker_t, &worker_t_attr, &noise_worker, (void*)this);   
      //pthread_detach(worker_t);
      /*pthread_getschedparam (worker_t,&policy,&s_param)*/;
      sched_param s_param;
      int policy = 0;
      s_param.sched_priority = 20;
      pthread_setschedparam (worker_t,policy,&s_param);
      worker_running = true;
    }
    ++frame;
    //if (bitm->valid)
    //if (first) {
/*      for (int x = 0; x < result_bitm.size_x; ++x) {
        for (int y = 0; y < result_bitm.size_y; ++y) {
          result_bitm.data[x + y*result_bitm.size_x] = bitm->data[x + y*result_bitm.size_x] | (unsigned char)rand() << 24  | (unsigned char)rand() << 8 | (unsigned char)rand() << 16 | (unsigned char)rand(); //bitm->data[x + y*result_bitm.size_x] 
        }
      }
      first = false;
      ++result_bitm.timestamp;*/
    //}
    
    
    result1->set_p(result_bitm);
  } else {
    worker_running = false;
    pthread_join(worker_t,0);
    result1->valid = false;
  }
}

void on_delete() {
  if (worker_running) {
    worker_running = false;
    result1->valid = false;
    pthread_join(worker_t,0);
  }
  delete[] data_a;
  delete[] data_b;
  if (result_bitm.valid)
  delete[] result_bitm.data;
}

};
