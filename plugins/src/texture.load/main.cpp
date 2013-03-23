/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Vovoid Media Technologies Copyright (C) 2003-2011
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

#include "_configuration.h"
#include "vsx_gl_global.h"
#include "vsx_math_3d.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include "pthread.h"
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
	    result_texture->set(texture);
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
	    result_texture->set(texture);
	  }
	}  
	
	void on_delete() {
	  texture->unload();
	  delete texture;
	}

};


//************************************************************************************
//************************************************************************************
//************************************************************************************
//************************************************************************************


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
    //printf("thread starting\n");
    //if (((module_load_png*)ptr)->bitm.data) {
      //delete ((module_load_png*)ptr)->bitm.data;
    //}
    
    ((module_load_png*)ptr)->pp = new pngRawInfo;
    if (pngLoadRaw(((module_load_png*)ptr)->current_filename.c_str(), ((module_load_png*)ptr)->pp,((module_load_png*)ptr)->engine->filesystem)) {
      ((module_load_png*)ptr)->bitm.valid = true;
      ((module_load_png*)ptr)->thread_state = 2;
    } else {
      //printf("thread: bitmap not valid\n");
      ((module_load_png*)ptr)->bitm.valid = false;
      ((module_load_png*)ptr)->thread_state = -1;
      ((module_load_png*)ptr)->last_modify_time = 0;
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
        texture->locked = true;
        texture->init_opengl_texture();
        texture->valid = false;
      }
      texture->upload_ram_bitmap(&bitm,true);
      texture->valid = true;
      texture_out->set(texture);
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

//************************************************************************************
//************************************************************************************
//************************************************************************************
//************************************************************************************


class module_load_jpeg : public vsx_module
{
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
  
  static void* jpeg_worker_v(void *ptr)
  {
    module_load_jpeg* mod = (module_load_jpeg*)ptr;

    CJPEGTest* cj = new CJPEGTest;
    vsx_string ret;
    if
    (
     !(cj
       ->LoadJPEG(
       mod->current_filename,
       ret,
       mod->engine->filesystem
     ))
    )
    {
      mod->message = "module||"+ret+"\n"+((module_load_jpeg*)ptr)->current_filename;
      mod->thread_state = -1;
      delete cj;
      return 0;
    }
    mod->bitm.size_x = cj->GetResX();
    mod->bitm.size_y = cj->GetResY();

    // allocate data in the vsx_bitmap
    vsx_bitmap_32bt b_c = (mod->bitm.size_x) * (mod->bitm.size_y);

    unsigned char* rb = (unsigned char*)cj->m_pBuf;
    mod->bitm.data = new vsx_bitmap_32bt[b_c*2];

    for (unsigned long i = 0; i < b_c; ++i)
    {
      ((vsx_bitmap_32bt*)mod->bitm.data)[i] =
          0xFF000000 |
          rb[i*3+2] << 16 |
          rb[i*3+1] << 8 |
          rb[i*3];
    }
    delete cj;
    mod->thread_state = 2;

    return 0;
  }
  
public:
  int m_type;

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
    info->description = "Loads a JPEG image from\ndisk and outputs a \n - VSXu bitmap \n and\n - texture.\nTexture is only loaded when used.\nThis is to preserve memory.";
    info->in_param_spec = "filename:resource";
    info->out_param_spec = "texture:texture,bitmap:bitmap";

    if (m_type == 0)
    {
      info->identifier = "bitmaps;loaders;jpeg_bitm_load";
      info->component_class = "bitmap";
    }
    else
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
  }
  
  void run()
  {
    if (current_filename != filename_in->get())
    {
      if (thread_state == -1)
      {
        message = "module||ok";
      }

      // time to decode a new jpg
      if (thread_state == 1)
      {
        long* aa;
        pthread_join(worker_t, (void **)&aa);
      }

      if (!verify_filesuffix(filename_in->get(),"jpg"))
      {
     		filename_in->set(current_filename);
     		message = "module||ERROR! This is not a JPG image file!";
     		return;
      }
      message = "module||ok";
      
      current_filename = filename_in->get();
      pthread_attr_init(&worker_t_attr);
      thread_state = 1;
      pthread_create(&(worker_t), &(worker_t_attr), &jpeg_worker_v, (void*)this);
    }
    if (thread_state == 2)
    {
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
        texture_out->set(texture);
        texture_timestamp = bitm.timestamp;
      }
    }
  }
  
  void on_delete()
  {
    if (thread_state == 1) 
    pthread_join(worker_t, 0);
    if (bitm.valid)
    {
      delete[] (vsx_bitmap_32bt*)bitm.data;
    }
  }  
};


//************************************************************************************
//************************************************************************************
//************************************************************************************
//************************************************************************************


class module_load_jpeg_alpha : public vsx_module
{
  // in
  float time;
  vsx_module_param_resource* filename_in;
  vsx_module_param_resource* filename_alpha_in;
  // out
  vsx_module_param_bitmap* bitmap_out;
  vsx_module_param_texture* texture_out;
  // internal
  vsx_texture* texture;

  // threading stuff
  void* pti_l;

  static void* jpeg_worker_v(void *ptr)
  {
    module_load_jpeg_alpha* mod = (module_load_jpeg_alpha*)ptr;

    vsx_string ret;

    CJPEGTest* cj = new CJPEGTest;
    if
    (
     !(cj
       ->LoadJPEG(
       mod->current_filename,
       ret,
       mod->engine->filesystem
     ))
    )
    {
      mod->message = "module||"+ret+"\n"+((module_load_jpeg*)ptr)->current_filename;
      mod->thread_state = -1;
      delete cj;
      return 0;
    }

    if (mod->current_alpha_filename != "")
    {

      CJPEGTest* cj_a = new CJPEGTest;
      if
      (
        !(cj_a
         ->LoadJPEG(
         mod->current_alpha_filename,
         ret,
         mod->engine->filesystem
       ))
      )
      {
        mod->message = "module||"+ret+"\n"+((module_load_jpeg*)ptr)->current_filename;
        mod->thread_state = -1;
        delete cj;
        delete cj_a;
        return 0;
      }

      mod->bitm.size_x = cj->GetResX();
      mod->bitm.size_y = cj->GetResY();

      // allocate data in the vsx_bitmap
      vsx_bitmap_32bt b_c = (mod->bitm.size_x) * (mod->bitm.size_y);

      unsigned char* rgbcp = (unsigned char*)cj->m_pBuf;

      unsigned char* acp = (unsigned char*)cj_a->m_pBuf;

      mod->bitm.data = new vsx_bitmap_32bt[b_c*2];

      for (unsigned long i = 0; i < b_c; ++i)
      {
        ((vsx_bitmap_32bt*)mod->bitm.data)[i] =
            acp[i*3] << 24 |
            rgbcp[i*3+2] << 16 |
            rgbcp[i*3+1] << 8 |
            rgbcp[i*3];
      }

      delete cj;
      delete cj_a;

      mod->thread_state = 2;
      return 0;
    }

    // no alpha filename selected

    mod->bitm.size_x = cj->GetResX();
    mod->bitm.size_y = cj->GetResY();

    // allocate data in the vsx_bitmap
    vsx_bitmap_32bt b_c = (mod->bitm.size_x) * (mod->bitm.size_y);

    unsigned char* rgbcp = (unsigned char*)cj->m_pBuf;

    mod->bitm.data = new vsx_bitmap_32bt[b_c*2];

    for (unsigned long i = 0; i < b_c; ++i)
    {
      ((vsx_bitmap_32bt*)mod->bitm.data)[i] =
          0xFF000000 |
          rgbcp[i*3+2] << 16 |
          rgbcp[i*3+1] << 8 |
          rgbcp[i*3];
    }

    delete cj;

    mod->thread_state = 2;

    return 0;
  }

public:
  int m_type;

  vsx_string current_filename;
  vsx_string current_alpha_filename;

  vsx_bitmap bitm;
  int bitm_timestamp; // keep track of the timestamp for the bitmap internally
  int               thread_state;
  bool              thread_working;
  pthread_t         worker_t;
  pthread_attr_t    worker_t_attr;
  int texture_timestamp;

  void module_info(vsx_module_info* info)
  {
    info->description =
      "Loads 2 JPEGs from\n"
      "disk and outputs a \n"
      " - VSXu bitmap with alpha\n"
      "and\n"
      "- texture\n"
      "Alpha channel from 2nd jpeg\n"
      "since jpeg's don't have alpha chan.\n"
      "Texture is only loaded when used.\n"
      "This is to preserve memory."
    ;

    info->in_param_spec =
      "filename_rgb:resource,"
      "filename_alpha:resource"
    ;

    info->out_param_spec =
      "texture:texture,"
      "bitmap:bitmap"
    ;

    if (m_type == 0)
    {
      info->identifier = "bitmaps;loaders;jpeg_bitm_load_alpha";
      info->component_class = "bitmap";
    }
    else
    {
      info->identifier = "texture;loaders;jpeg_tex_load_alpha";
      info->component_class = "texture";
    }
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    texture_timestamp = -1;
    loading_done = false;

    filename_in = (vsx_module_param_resource*)in_parameters.create(VSX_MODULE_PARAM_ID_RESOURCE,"filename_rgb");
    filename_in->set("");
    filename_alpha_in = (vsx_module_param_resource*)in_parameters.create(VSX_MODULE_PARAM_ID_RESOURCE,"filename_alpha");
    filename_alpha_in->set("");
    current_filename = "";
    current_alpha_filename = "";

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
  }

  void run()
  {
    if (current_filename != filename_in->get())
    {
      if (thread_state == -1)
      {
        message = "module||ok";
      }

      // time to decode a new jpg
      if (thread_state == 1)
      {
        long* aa;
        pthread_join(worker_t, (void **)&aa);
      }

      if (!verify_filesuffix(filename_in->get(),"jpg"))
      {
        filename_in->set(current_filename);
        message = "module||ERROR! This is not a JPG image file!";
        return;
      }
      message = "module||ok";

      current_filename = filename_in->get();
      current_alpha_filename = filename_alpha_in->get();

      pthread_attr_init(&worker_t_attr);
      thread_state = 1;
      pthread_create(&(worker_t), &(worker_t_attr), &jpeg_worker_v, (void*)this);
    }
    if (thread_state == 2)
    {
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
        texture_out->set(texture);
        texture_timestamp = bitm.timestamp;
      }
    }
  }

  void on_delete()
  {
    if (thread_state == 1)
    pthread_join(worker_t, 0);
    if (bitm.valid)
    {
      delete[] (vsx_bitmap_32bt*)bitm.data;
    }
  }
};


//******************************************************************************
//*** F A C T O R Y ************************************************************
//******************************************************************************

#ifndef _WIN32
#define __declspec(a)
#endif

extern "C" {
__declspec(dllexport) vsx_module* create_new_module(unsigned long module, void* args);
__declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
__declspec(dllexport) unsigned long get_num_modules();
}


bool glewinit = false;

vsx_module* create_new_module(unsigned long module, void* args) {
  if (!glewinit)
  {
    // we sneak in and init glew :) as we need it
    glewInit();
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
    case 5:
    {
      module_load_jpeg_alpha* m2 = new module_load_jpeg_alpha;
      m2->m_type = module % 2;
      return (vsx_module*)m2;
    }
    case 6:
    return (vsx_module*)(new texture_loaders_bitmap2texture);
  };
  return 0;
}

void destroy_module(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: case 1: delete (module_load_png*)m; break;
    case 2: case 3: delete (module_load_jpeg*)m; break;
    case 4: case 5: delete (module_load_jpeg_alpha*)m; break;
    case 6: delete (texture_loaders_bitmap2texture*)m; break;
  }
}

unsigned long get_num_modules() {
  // we have only one module. it's id is 0
  return 7;
}  
