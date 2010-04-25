#include "_configuration.h"
#include <string>
#include "vsx_gl_global.h"
#include "vsx_math_3d.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include "pthread.h"
#include "main.h"
#include "vsxg.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef VSXU_MAC_XCODE
#include <syslog.h>
#endif

typedef struct stat t_stat;


class texture_loaders_bitmap2texture : public vsx_module {
  // in
	float time;
	vsx_module_param_bitmap* bitm_in;
	vsx_module_param_int* mipmaps;
	
	// out
	vsx_module_param_texture* result_texture;
	// internal
	
	vsx_bitmap* bitm;
	int bitm_timestamp;
	
  vsx_texture* texture;

public:

	void module_info(vsx_module_info* info)
	{
	  info->identifier = "texture;loaders;bitmap2texture";
	#ifndef VSX_NO_CLIENT
	  info->description = "";
	  info->in_param_spec = "bitmap:bitmap,mipmaps:enum?yes|no";
	  info->out_param_spec = "texture:texture";
	  info->component_class = "texture";
	#endif
	}
	
	// ----
	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
	{
	  
		bitm_in = (vsx_module_param_bitmap*)in_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
		mipmaps = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"mipmaps");
		mipmaps->set(0);
	  //bitm_in->set(bitm);
	  //bitm.size_x = 0;
	  //bitm.size_y = 0;
	  //bitm.valid = false;
	  bitm_timestamp = 0;
	  texture = new vsx_texture;
	  texture->locked = true;
	  texture->init_opengl_texture();
	
	  result_texture = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture");  
	  result_texture->set_p(*texture);
	  loading_done = true;
	}
	
	void run() {
	  bitm = bitm_in->get_addr();
	  if (!bitm) {
	    result_texture->valid = false;
	    return;
	  }
	  if (bitm->valid && bitm_timestamp != bitm->timestamp) {
	    //texture->unload();
	    // ok, new version
	    //printf("uploading bitmap as texture %d \n",bitm->timestamp);
	    bitm_timestamp = bitm->timestamp;
	    //printf("u-");
	    if (mipmaps->get() == 0)
	    texture->upload_ram_bitmap(bitm,true);
	    else
	    texture->upload_ram_bitmap(bitm,false);
	    result_texture->set_p(*texture);
	    //printf("-u");
	  } 
	}
	
	void stop() {
	  texture->unload();
	}  
	
	void start() {
	  //printf("starting textureuploader\n");
	  texture->init_opengl_texture();
	  bitm = bitm_in->get_addr();
	  if (bitm) {
	    texture->upload_ram_bitmap(bitm,mipmaps->get());
	    result_texture->set_p(*texture);
	  }
	}  
	
	void on_delete() {
	  texture->unload();
	  delete texture->transform_obj;
	  delete texture;
	}

};
// PNG LOADER --------------------------------------------------------------------------------------

class module_load_png : public vsx_module {
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
  
  static void* png_worker_v(void *ptr) {
    printf("thread starting\n");
    //if (((module_load_png*)ptr)->bitm.data) {
      //delete ((module_load_png*)ptr)->bitm.data;
    //}
    
    ((module_load_png*)ptr)->pp = new pngRawInfo;
    if (pngLoadRaw(((module_load_png*)ptr)->current_filename.c_str(), ((module_load_png*)ptr)->pp,((module_load_png*)ptr)->engine->filesystem)) {
      ((module_load_png*)ptr)->bitm.valid = true;
      ((module_load_png*)ptr)->thread_state = 2;
    } else {
      printf("thread: bitmap not valid\n");
      ((module_load_png*)ptr)->bitm.valid = false;
      ((module_load_png*)ptr)->thread_state = -1;
      ((module_load_png*)ptr)->last_modify_time = 0;
    }
    printf("png thread ending\n");
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
    bitm_timestamp = bitm.timestamp;
  
    bitmap_out->set_p(bitm);
    thread_state = 0;
    texture = new vsx_texture;
    texture->locked = true;
    texture->init_opengl_texture();
    texture->valid = false;
    
  	texture_out = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture");
  	texture_out->valid = false;
  	
  	texture_timestamp = -1;
  	//texture_out->set_p(*texture);
  }
  
  time_t last_modify_time;
  t_stat st;
  void run() {
		
    //int state_checked = 0;
    /*if (poll_updates->get() && current_filename != "")
    {
      
      stat(current_filename.c_str(), &st);
      //printf("mtime: %d\n", st.st_mtime);
      if (st.st_mtime != last_modify_time)
      {
        last_modify_time = st.st_mtime;
        state_checked = 1;
        printf("STATE CHECK AND CHONG\n");
      }
    }*/
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
        bitm.data = (unsigned long*)(pp->Data);
        //bitm.valid = true;
        //printf("%d %d %d\n",bitm.bpp,bitm.size_x,bitm.size_y);
        bitm.timestamp++;
        bitmap_out->set_p(bitm);
      }
      loading_done = true;
  }
  //bitm = bitm_in->get();
  //if (bitm.valid && bitm_timestamp != bitm.timestamp) {
    // ok, new version
    //bitm_timestamp = bitm.timestamp;
    //texture->upload_ram_bitmap(&bitm);
  //}  
  //result_texture->set_p(*texture);
}

void output(vsx_module_param_abs* param)
{
  if (param == (vsx_module_param_abs*)texture_out)
  {
    if (texture_timestamp != bitm.timestamp)
    {
      texture->upload_ram_bitmap(&bitm,true);
      texture->valid = true;
      texture_out->set_p(*texture);
      texture_timestamp = bitm.timestamp;
    }
  }
}

void stop() {
  texture->unload();
}

void start() {
  texture->init_opengl_texture();
  texture->upload_ram_bitmap(&bitm,true);
  texture->valid = true;
  texture_out->set_p(*texture);
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
    delete texture->transform_obj;
    delete texture;
  }
}

};

//************************************************************************************
//************************************************************************************
//************************************************************************************
//************************************************************************************
/*
void module_load_png_bitmap::module_info(vsx_module_info* info)
{

  info->identifier = "bitmaps;loaders;png_bitm_load";
#ifndef VSX_NO_CLIENT
  info->description = "";
  info->in_param_spec = "filename:resource";
  info->out_param_spec = "bitmap:bitmap";
  info->component_class = "bitmap";
#endif
}

void module_load_png_bitmap::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = false;
  
	filename_in = (vsx_module_param_resource*)in_parameters.create(VSX_MODULE_PARAM_ID_RESOURCE,"filename");
	filename_in->set("");
	current_filename = "";
	
	// out
	bitmap_out = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");

  bitm.data = 0;
  bitm.size_x = 0;
  bitm.size_y = 0;
  bitm.valid = false;
  bitm_timestamp = bitm.timestamp;

  bitmap_out->set_p(bitm);
  thread_state = 0;
}

void module_load_png_bitmap::run() {
  if (current_filename != filename_in->get()) {
    // time to decode a new png
    if (thread_state) {
      if (thread_state == 1) {
        long* aa;
        pthread_join(worker_t, (void **)&aa);
      }
      free(pp->Data);
      delete pp;
      bitm.data = 0;
    }  
   	if (!verify_filesuffix(filename_in->get(),"png")) {
   		filename_in->set(current_filename);
   		message = "module||ERROR! This is not a PNG image file!";
   		return;
   	} else message = "module||ok";
    
    current_filename = filename_in->get();
    pthread_attr_init(&worker_t_attr);
    thread_state = 1;
    pthread_create(&(worker_t), &(worker_t_attr), &png_worker_v, (void*)this);
  }
  //printf("foobar\n");
  if (thread_state == 2) {
    //printf("bitmap creation done\n");
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
    bitm.data = (unsigned long*)(pp->Data);
    //printf("%d %d %d\n",bitm.bpp,bitm.size_x,bitm.size_y);
    
    bitm.valid = true;
    ++bitm.timestamp;
    thread_state = 3;
    bitmap_out->set_p(bitm);
    loading_done = true;
  }
  //bitm = bitm_in->get();
  //if (bitm.valid && bitm_timestamp != bitm.timestamp) {
    // ok, new version
    //bitm_timestamp = bitm.timestamp;
    //texture->upload_ram_bitmap(&bitm);
  //}  
  //result_texture->set_p(*texture);
}

void module_load_png_bitmap::on_delete() {
  if (thread_state == 1)
  pthread_join(worker_t,0);
  if (thread_state) {
    free(pp->Data);
    delete pp;
  } 
}
*/
//************************************************************************************
//************************************************************************************
//************************************************************************************
//************************************************************************************


class module_load_jpeg : public vsx_module {
  // in
  float time;
  vsx_module_param_resource* filename_in;
  // out
  vsx_module_param_bitmap* bitmap_out;
  vsx_module_param_texture* texture_out;
  // internal
  vsx_texture* texture;
  // threading stuff
  void* pti_l;
  
  static void* jpeg_worker_v(void *ptr) {
    module_load_jpeg* mod = (module_load_jpeg*)ptr;
    mod->cj = (void*)new CJPEGTest;
    vsx_string ret;
    if (!(((CJPEGTest*)(mod->cj))->LoadJPEG(mod->current_filename,ret,mod->engine->filesystem))) {
      //printf("jpeg error was: %s\n",ret.c_str());
      mod->message = "module||"+ret+"\n"+((module_load_jpeg*)ptr)->current_filename;
      mod->thread_state = -1;
    } else {
      //int size_x = ((module_load_jpeg_bitmap*)ptr)->bitm.size_x;
      //int size_y = ((module_load_jpeg_bitmap*)ptr)->bitm.size_y;
      mod->bitm.size_x = ((CJPEGTest*)mod->cj)->GetResX();
      mod->bitm.size_y = ((CJPEGTest*)mod->cj)->GetResY();
      unsigned long b_c = (mod->bitm.size_x) * (mod->bitm.size_y);
      char* rb = (char*)((CJPEGTest*)mod->cj)->m_pBuf;
      mod->bitm.data = new unsigned long[b_c*2];
      for (unsigned long i = 0; i < b_c; ++i) {
        mod->bitm.data[i] = 0xFF000000 | (unsigned char)rb[i*3+2] << 16 | (unsigned char)rb[i*3+1] << 8 | (unsigned char)rb[i*3]; 
      }
      /*unsigned char* data2 = new unsigned char[b_c * 4];  
      int dy = 0;
      for (int y = size_y-1; y >= 0; --y) {
        for (int x = 0; x < size_x*4; ++x) {
          data2[dy*size_x*4 + x] = ((unsigned char*)((module_load_jpeg_bitmap*)ptr)->bitm.data)[y*size_x*4 + x];
        }
        ++dy;
      }*/
      //delete[] ((module_load_jpeg_bitmap*)ptr)->bitm.data;
      //((module_load_jpeg_bitmap*)ptr)->bitm.data = (unsigned long*)data2;
      delete (CJPEGTest*)mod->cj;
      //while (((module_load_jpeg_bitmap*)ptr)->thread_state != 2)
      mod->thread_state = 2;
      //printf("jpeg thread ending %d %d\n",((module_load_jpeg_bitmap*)ptr)->thread_state_a,ptr);
    }
    return 0;
  }
  
public:
  int m_type;
  void* cj;
  vsx_string current_filename;
  vsx_bitmap bitm;
  int bitm_timestamp; // keep track of the timestamp for the bitmap internally 
  int               thread_state;
  bool              thread_working;
  pthread_t         worker_t;
  pthread_attr_t    worker_t_attr;
  int texture_timestamp;
  
  void module_info(vsx_module_info* info)
  {
    //info->identifier = "bitmaps;loaders;jpeg_bitm_load||texture;loaders;jpeg_tex_load";
  #ifndef VSX_NO_CLIENT
    info->description = "Loads a JPEG image from\ndisk and outputs a \n - VSXu bitmap \n and\n - texture.\nTexture is only loaded when used.\nThis is to preserve memory.";
    info->in_param_spec = "filename:resource";
    info->out_param_spec = "texture:texture,bitmap:bitmap";
  #endif
    if (m_type == 0)
    {
      info->identifier = "bitmaps;loaders;jpeg_bitm_load";
      info->component_class = "bitmap";
    } else
    {
      info->identifier = "texture;loaders;jpeg_tex_load";
      info->component_class = "texture";
    }
    
  }
  
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    texture_timestamp = -1;
    loading_done = false;
    
  	filename_in = (vsx_module_param_resource*)in_parameters.create(VSX_MODULE_PARAM_ID_RESOURCE,"filename");
  	filename_in->set("");
  	current_filename = "";
  	
  	// out
  	bitmap_out = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
  
    bitm.size_x = 0;
    bitm.size_y = 0;
    bitm_timestamp = 0;
    bitm.valid = false;
  
    bitmap_out->set_p(bitm);
    thread_state = 0;
    texture_out = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture");

  	texture = new vsx_texture;
    texture->locked = true;
    texture->init_opengl_texture();
    //texture->valid = false;
  	
  	//texture_out->set_p(*texture);
  	//printf("this is %d\n",this);
  }
  
  void run() {
		
    if (current_filename != filename_in->get()) {
      if (thread_state == -1) {
        message = "module||ok";
      }
      // time to decode a new png
      if (thread_state == 1) {
        long* aa;
        pthread_join(worker_t, (void **)&aa);
      }
     	if (!verify_filesuffix(filename_in->get(),"jpg")) {
     		filename_in->set(current_filename);
     		message = "module||ERROR! This is not a JPG image file!";
     		return;
     	} else message = "module||ok";
      
      current_filename = filename_in->get();
      pthread_attr_init(&worker_t_attr);
      thread_state = 1;
      pthread_create(&(worker_t), &(worker_t_attr), &jpeg_worker_v, (void*)this);
//      printf("setting thread state to 1\n");
    }
    //printf("foobar\n");
  //  printf("%d %d:",thread_state_a,thread_state_b);
    if (thread_state == 2) {
//			printf("thread finished loading\n");
      bitm.bpp = 4;
      bitm.bformat = GL_RGBA;
      bitm.valid = true;
      ++bitm.timestamp;
      thread_state = 3;
      loading_done = true;
      bitmap_out->set_p(bitm);
    }
  }

  void output(vsx_module_param_abs* param)
  {
    if (param == (vsx_module_param_abs*)texture_out)
    {
      if (texture_timestamp != bitm.timestamp && bitm.valid)
      {
        texture->upload_ram_bitmap(&bitm,true);
        texture->valid = true;
        texture_out->set_p(*texture);
        texture_timestamp = bitm.timestamp;
      }
    }
  }
  
  void on_delete() {
    if (thread_state == 1) 
    pthread_join(worker_t, 0);
    if (bitm.valid) {
      delete[] bitm.data;
    }
  }  
};



// MODULE INTERFACE

#if BUILDING_DLL
bool glewinit = false;

vsx_module* create_new_module(unsigned long module) {
  if (!glewinit) {
    // we sneak in and init glew :) as we need it
//    printf("textures.bitmaps-lib: glewinit\n");
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
  //    printf("Error: %s\n", glewGetErrorString(err));
    }
  }  

  switch(module) {
    case 0: 
    case 1:
    {
      module_load_png* m = new module_load_png;
      m->m_type = module % 2;
      return (vsx_module*)m;
    }
    case 2: 
    case 3:
    {
      module_load_jpeg* m2 = new module_load_jpeg;
      m2->m_type = module % 2;
      return (vsx_module*)m2;
    }
    case 4:
    return (vsx_module*)(new texture_loaders_bitmap2texture);
  };
  return 0;
}

void destroy_module(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: case 1: delete (module_load_png*)m; break;
    case 2: case 3: delete (module_load_jpeg*)m; break;
    case 4: delete (texture_loaders_bitmap2texture*)m; break;
  }
}

unsigned long get_num_modules() {
  // we have only one module. it's id is 0
  return 5;
}  
#endif
