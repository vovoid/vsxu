class module_texture_load_png : public vsx_module 
{
  // in
  float time;
  vsx_module_param_resource* filename_in;
  vsx_module_param_int* reload;
  
  // out
  vsx_module_param_bitmap* bitmap_out;
  vsx_module_param_texture* texture_out;
  
  // internal
  
  // threading stuff
  void* pti_l;
  
  vsx_texture* texture;
  
  static void* png_worker_v(void *ptr)
  {
    //printf("thread starting\n");
    //if (((module_load_png*)ptr)->bitm.data) {
      //delete ((module_load_png*)ptr)->bitm.data;
    //}

    module_texture_load_png* module = ((module_texture_load_png*)ptr);
    
    ((module_texture_load_png*)ptr)->pp = new pngRawInfo;
    if (pngLoadRaw( module->current_filename.c_str(), ((module_texture_load_png*)ptr)->pp,((module_texture_load_png*)ptr)->engine->filesystem)) {
      ((module_texture_load_png*)ptr)->bitm.valid = true;
      ((module_texture_load_png*)ptr)->thread_state = 2;
    } else {
      //printf("thread: bitmap not valid\n");
      ((module_texture_load_png*)ptr)->bitm.valid = false;
      ((module_texture_load_png*)ptr)->thread_state = -1;
      ((module_texture_load_png*)ptr)->last_modify_time = 0;
    }
    //printf("png thread ending\n");
    return 0;
  }
  

public:
  int m_type;
  vsx_string current_filename;
  vsx_bitmap bitm;
  int bitm_timestamp; // keep track of the timestamp for the bitmap internally 
  int               thread_state;
  bool              thread_working;
  pngRawInfo*       pp;
  pthread_t         worker_t;
  pthread_attr_t    worker_t_attr;

  int texture_timestamp;

  void module_info(vsx_module_info* info)
  {
  #ifndef VSX_NO_CLIENT
    info->description = "Loads a PNG image from\ndisk and outputs a \n - VSXu bitmap \n and\n - texture.\nTexture is only loaded when used.\nThis is to preserve memory.";
    info->in_param_spec = "filename:resource,reload:enum?no|yes";
    info->out_param_spec = "texture:texture,bitmap:bitmap";
#endif
    if (m_type == 0)
    {
      info->identifier = "bitmaps;loaders;png_bitm_load";
      info->component_class = "bitmap";
    } else
    {
      info->identifier = "texture;loaders;png_tex_load";
      info->component_class = "texture";
    }
  }
  
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = false;
    
    filename_in = (vsx_module_param_resource*)in_parameters.create(VSX_MODULE_PARAM_ID_RESOURCE,"filename");
    filename_in->set("");
    current_filename = "";
    
    reload = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "reload");
    
    // out
    bitmap_out = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
  
    bitm.size_x = 0;
    bitm.size_y = 0;
    bitm.valid = false;
    bitm.data = 0;
    texture_timestamp = bitm_timestamp = bitm.timestamp;
  
    bitmap_out->set_p(bitm);
    thread_state = 0;
    texture = 0x0;
    
    texture_out = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture");
    texture_out->valid = false;
  }
  
  time_t last_modify_time;
  t_stat st;
  void run()
  {
    if (current_filename != filename_in->get() || reload->get() == 1) {
      reload->set(0);

      // time to decode a new png
      if (thread_state > 0) {
        if (thread_state == 1) {
          long* aa;
          pthread_join(worker_t, (void **)&aa);
        }
        free(pp->Data);
        delete pp; 
      }
     	if (!verify_filesuffix(filename_in->get(),"png")) {
     		filename_in->set(current_filename);
     		message = "module||ERROR! This is not a PNG image file!";
     		return;
     	} else message = "module||ok";
    
      current_filename = filename_in->get();
      stat(current_filename.c_str(), &st);
      last_modify_time = st.st_mtime;
      pthread_attr_init(&worker_t_attr);
      thread_state = 1;
      pthread_create(&(worker_t), &(worker_t_attr), &png_worker_v, (void*)this);
    }
    //printf("foobar\n");
    if (thread_state == 2) {
      thread_state = 3;
      if (bitm.valid) {
        if (pp->Components == 1) {
          bitm.bpp = 3;
          bitm.bformat = GL_RGB;
        } else 
        if (pp->Components == 2) {
          bitm.bpp = 4;
          bitm.bformat = GL_RGBA;
        } else
        if (pp->Components == 3) {
          bitm.bpp = 3;
          bitm.bformat = GL_RGB;
        } else
        if (pp->Components == 4) {
          bitm.bpp = 4;
          bitm.bformat = GL_RGBA;
        }
        bitm.size_x = pp->Width;
        bitm.size_y = pp->Height;
        bitm.data = (vsx_bitmap_32bt*)(pp->Data);

        bitm.timestamp++;
        bitmap_out->set_p(bitm);
      }
      loading_done = true;
  }
}

void output(vsx_module_param_abs* param)
{
  if (param == (vsx_module_param_abs*)texture_out)
  {
    if (texture_timestamp != bitm.timestamp)
    {
      if (texture == 0x0)
      {
        texture = new vsx_texture;
        texture->init_opengl_texture_2d();
        texture->valid = false;
      }
      texture->upload_ram_bitmap_2d(&bitm,true);
      texture->valid = true;
      texture_out->set(texture);
      texture_timestamp = bitm.timestamp;
    }
  }
}

void stop() {
  if (texture)
  {
    texture->unload();
  }
}

void start() {
  texture->init_opengl_texture_2d();
  texture->upload_ram_bitmap_2d(&bitm,true);
  texture->valid = true;
  texture_out->set(texture);
}


void on_delete() {
  if (thread_state == 1)
  pthread_join(worker_t,0);
  if (thread_state > 0) {
    free(pp->Data);
    delete pp;
  }
  if (texture) {
    texture->unload();
    delete texture;
  }
}

};


