#include <vsx_bitmap.h>
#include <texture/vsx_texture.h>

class module_texture_load_png : public vsx_module 
{
  // in
  float time;
  vsx_module_param_resource* filename_in;
  vsx_module_param_int* reload_in;
  
  // out
  vsx_module_param_bitmap* bitmap_out;
  vsx_module_param_texture* texture_out;
  
  // internal 
  void* pti_l;
  vsx_string<>current_filename;

  vsx_bitmap bitmap;
  vsx_texture* texture;

  int bitm_timestamp; // keep track of the timestamp for the bitmap internally
  volatile int               thread_state;
  bool              thread_working;
  pngRawInfo*       pp;
  pthread_t         worker_t;
  pthread_attr_t    worker_t_attr;

  int texture_timestamp;

  
  static void* png_worker_v(void *ptr)
  {
    module_texture_load_png* module = ((module_texture_load_png*)ptr);
    
    ((module_texture_load_png*)ptr)->pp = new pngRawInfo;
    if (pngLoadRaw( module->current_filename.c_str(), ((module_texture_load_png*)ptr)->pp,((module_texture_load_png*)ptr)->engine->filesystem)) {
      ((module_texture_load_png*)ptr)->bitmap.valid = true;

      // memory barrier
      asm volatile("":::"memory");

      ((module_texture_load_png*)ptr)->thread_state = 2;
      return 0;
    }

    ((module_texture_load_png*)ptr)->bitmap.valid = false;
    ((module_texture_load_png*)ptr)->last_modify_time = 0;

    // memory barrier
    asm volatile("":::"memory");

    ((module_texture_load_png*)ptr)->thread_state = -1;

    return 0;
  }
  

public:

  int m_type;

  void module_info(vsx_module_info* info)
  {
    if (m_type == 0)
    {
      info->identifier = "bitmaps;loaders;png_bitm_load";
      info->component_class = "bitmap";
    } else
    {
      info->identifier = "texture;loaders;png_tex_load";
      info->component_class = "texture";
    }

    info->description =
      "Loads a PNG image from\n"
      "disk and outputs a \n"
      " - VSXu bitmap \n"
      " and\n"
      " - texture.\n"
      "Texture is only loaded when used.\n"
      "This is to preserve memory.";

    info->in_param_spec =
      "filename:resource,"
      "reload:enum?no|yes"
    ;

    info->out_param_spec =
      "texture:texture,bitmap:bitmap";
  }
  
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = false;
    
    filename_in = (vsx_module_param_resource*)in_parameters.create(VSX_MODULE_PARAM_ID_RESOURCE,"filename");
    filename_in->set("");
    current_filename = "";
    
    reload_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "reload");
    
    // out
    bitmap_out = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
  
    texture_timestamp = bitm_timestamp = bitmap.timestamp;
  
    thread_state = 0;
    texture = 0x0;
    
    texture_out = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture");
    texture_out->valid = false;
  }
  
  time_t last_modify_time;
  t_stat st;
  void run()
  {
    if (current_filename != filename_in->get() || reload_in->get() == 1) {
      reload_in->set(0);

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

    if (thread_state == 2) {
      thread_state = 3;
      if (bitmap.valid) {
        if (pp->Components == 1) {
          bitmap.channels = 3;
        } else 
        if (pp->Components == 2) {
          bitmap.channels = 4;
        } else
        if (pp->Components == 3) {
          bitmap.channels = 3;
        } else
        if (pp->Components == 4) {
          bitmap.channels = 4;
        }
        bitmap.width = pp->Width;
        bitmap.height = pp->Height;
        bitmap.data = (vsx_bitmap_32bt*)(pp->Data);

        bitmap.timestamp++;
        bitmap_out->set(&bitmap);
      }
      loading_done = true;
  }
}

void output(vsx_module_param_abs* param)
{
  if (param == (vsx_module_param_abs*)texture_out)
  {
    if (texture_timestamp != bitmap.timestamp)
    {
      if (texture == 0x0)
      {
        texture = new vsx_texture;
        texture->texture_gl->init_opengl_texture_2d();
      }
      vsx_texture_gl_loader::upload_bitmap_2d(texture->texture_gl, &bitmap, true, true);
      pp->Data = (unsigned char*)bitmap.data;
      texture_out->set(texture);
      texture_timestamp = bitmap.timestamp;
    }
  }
}

void stop() {
  if (texture)
  {
    texture->texture_gl->unload();
  }
}

void start() {
  texture->texture_gl->init_opengl_texture_2d();
  vsx_texture_gl_loader::upload_bitmap_2d(texture->texture_gl, &bitmap, true, true);
  texture_out->set(texture);
}


void on_delete() {
  if (thread_state == 1)
    pthread_join(worker_t,0);

  if (thread_state > 0) {
    if (bitmap.data)
      delete[] (unsigned char*)(bitmap.data);

    delete pp;
  }

  if (texture) {
    texture->texture_gl->unload();
    delete texture;
  }
}

};


