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

#include "_configuration.h"
//----------------------------
#include "vsx_gl_global.h"
#include "vsx_math_3d.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include <pthread.h>

#ifndef _WIN32
#include <unistd.h>
#define Sleep sleep
#endif


class module_texture_to_bitmap : public vsx_module {
  // in
	vsx_module_param_texture* texture_in;
	// out
	vsx_module_param_bitmap* result1;
	// internal
	
	vsx_bitmap bitm;
	int bitm_timestamp;
	
  vsx_texture** texture;

  int p_updates;

public:
  
  void module_info(vsx_module_info* info)
  {
    info->in_param_spec = "texture_in:texture";
    info->identifier = "texture;loaders;texture2bitmap";
    info->out_param_spec = "bitmap:bitmap";
    info->component_class = "bitmap";
    info->description = "transforms a texture into a bitmap (slow!)";
  }
  
  void param_set_notify(const vsx_string& name) {
  };
  
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
  	result1 = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
    result1->set_p(bitm);
    bitm.bpp = 4;
    bitm.bformat = GL_RGBA;
    bitm.size_x = 0;
    bitm.size_y = 0;
    bitm.data = 0;
    bitm.valid = false;
    texture_in = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture_in");  
    loading_done = true;
  }
  void run() {
    texture = texture_in->get_addr();
    if (texture) {
      (*texture)->bind();
      GLint components;
      glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_COMPONENTS,&components);
      //printf("components: %d\n",components);
      GLint pack;
      glGetIntegerv(GL_PACK_ALIGNMENT,&pack);
//          printf("pack: %d\n",pack);
      if (pack == 4) {
        GLint width;
        GLint height;
        glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&height);
//              printf("height: %d\n",height);
        if (bitm.size_x != (unsigned int)width || bitm.size_y != (unsigned int)height) {
//          printf("allocating new memory\n");
//          printf("height: %d\n",height);
          
          if (bitm.data) delete[] (vsx_bitmap_32bt*)bitm.data;
          bitm.data = new vsx_bitmap_32bt[width*height];
          bitm.size_x = width;
          bitm.size_y = height;
        }
          //printf("bwidth: %d\n",width);
        glGetTexImage(GL_TEXTURE_2D,
                   0,
                   GL_RGBA,
                   GL_UNSIGNED_BYTE,
                   bitm.data);
        //printf("bdata: %d\n",(unsigned int)bitm.data[0]);
        bitm.valid = true;
        ++bitm.timestamp;
        result1->set_p(bitm);
      }
      (*texture)->_bind();
    }
  }
  void start() {
  }  
  
  void stop() {
  }
  
  void on_delete() {
    //printf("deleting bitmap..");
    if (bitm.valid)
    delete[] (vsx_bitmap_32bt*)bitm.data;
  }
};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


class module_bitmap_to_particlesystem : public vsx_module {
  // in
	vsx_module_param_bitmap* bitmap_in;
	vsx_module_param_float* size;
	vsx_module_param_float* blobsize;
	vsx_module_param_float* random_weight;
	// out
	vsx_module_param_particlesystem* particlesystem_out;
	// internal
	
	vsx_bitmap* bitm;
	vsx_bitmap bitm2;
	int bitm_timestamp;
	
  vsx_particlesystem particles;

  int p_updates;
  bool first;

public:
  
  void module_info(vsx_module_info* info)
  {
    info->identifier = "particlesystems;generators;bitmap2particlesystem";
    info->in_param_spec = "bitmap_in:bitmap,size:float,blobsize:float,random_weight:float";
    info->out_param_spec = "particlesystem_out:particlesystem";
    info->component_class = "particlesystem";
    info->description = "";
  }
  
  void param_set_notify(const vsx_string& name) {
    first = true;
  };
  
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    bitmap_in = (vsx_module_param_bitmap*)in_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap_in");  
    size = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"size");  
    size->set(5.0f);
    blobsize = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"blobsize");  
    blobsize->set(0.1f);
    random_weight = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"random_weight");  
    random_weight->set(0.5f);
    //bitmap_in->set_p(bitm2);
    particles.timestamp = 0;
    particles.particles = new vsx_array<vsx_particle>;
  	particlesystem_out = (vsx_module_param_particlesystem*)out_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"particlesystem_out");
  	particlesystem_out->set_p(particles);
    loading_done = true;
    first = true;
    p_updates = param_updates;
  }
  void run() {
    bitm = bitmap_in->get_addr();
    if (bitm) {
      if (p_updates != param_updates) {
        first = true;
        p_updates = param_updates;
      }
      int i = 0;
      int width = bitm->size_x;
      //printf("width: %d\n",width);
      //printf("height: %d\n",bitm->size_y);
      float space = size->get()/(float)width;
      float dest = -size->get()*0.5f;
      if (first) {
        for (size_t y = 0; y < bitm->size_y; ++y) {
          for (int x = 0; x < width; ++x) {
            (*particles.particles)[i].pos.x = dest+(float)x*space+random_weight->get()*(-0.5+(float)(rand()%1000)/1000.0f);
            (*particles.particles)[i].pos.y = 0;
            (*particles.particles)[i].pos.z = dest+(float)y*space+random_weight->get()*(-0.5+(float)(rand()%1000)/1000.0f);
            ++i;
          }
        }
        first = false;
      }

      i = 0;
      for (size_t y = 0; y < bitm->size_y; ++y) {
        int cc = y*width;
        if (bitm->bformat == GL_RGBA)
        {
          for (int x = 0; x < width; ++x) {
            (*particles.particles)[i].color.b = ((float)(unsigned char)((((vsx_bitmap_32bt*)bitm->data)[cc+x]&0x00FF0000)>>16))/255.0f;
            (*particles.particles)[i].color.g = ((float)(unsigned char)((((vsx_bitmap_32bt*)bitm->data)[cc+x]&0x0000FF00)>>8))/255.0f;
            (*particles.particles)[i].color.r = ((float)(unsigned char)((((vsx_bitmap_32bt*)bitm->data)[cc+x]&0x000000FF)))/255.0f;
            (*particles.particles)[i].color.a = 1.0f;
            if ((*particles.particles)[i].color.b < 0.01f && (*particles.particles)[i].color.g < 0.01f && (*particles.particles)[i].color.r < 0.01f)
            (*particles.particles)[i].size = 0.0f;
            else
            (*particles.particles)[i].size = (*particles.particles)[i].orig_size = blobsize->get();

            (*particles.particles)[i].speed.x = 0;
            (*particles.particles)[i].speed.y = 0;
            (*particles.particles)[i].speed.z = 0;
            (*particles.particles)[i].time = 0;
            (*particles.particles)[i].lifetime = 1000000000;
            ++i;
          }
        }
      }
      particlesystem_out->set_p(particles);
    }
  }
  ~module_bitmap_to_particlesystem() {
    delete particles.particles;
  }
};


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
  vsx_bitmap_32bt *data_a;
  vsx_bitmap_32bt *data_b;
  int bitm_timestamp;
  vsx_bitmap result_bitm;
  bool first, worker_running, t_done;
  pthread_t         worker_t;
  pthread_attr_t    worker_t_attr;
  int my_ref;

public:



static void* noise_worker(void *ptr) {
  int i_frame = -1;
  //int x,y;
  bool buf = false;
  vsx_bitmap_32bt *p;
  while (((module_bitmap_add_noise*)ptr)->worker_running) {
    if (i_frame != ((module_bitmap_add_noise*)ptr)->frame) {
    //printf("%d ",ptr);
      // time to run baby
      if (buf) p = ((module_bitmap_add_noise*)ptr)->data_a;
      else p = ((module_bitmap_add_noise*)ptr)->data_b;

      unsigned long b_c = ((module_bitmap_add_noise*)ptr)->result_bitm.size_x * ((module_bitmap_add_noise*)ptr)->result_bitm.size_y;

      //((module_bitmap_add_noise*)ptr)->result_bitm->data;
      //unsigned long cc = rand()<<8 | (char)rand();
      if (((module_bitmap_add_noise*)ptr)->t_bitm.bformat == GL_RGBA)
      {
        for (size_t x = 0; x < b_c; ++x)
        {
            p[x] = ((vsx_bitmap_32bt*)((module_bitmap_add_noise*)ptr)->t_bitm.data)[x] | rand() << 8  | (unsigned char)rand(); //bitm->data[x + y*result_bitm.size_x]
        }
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
      if (result_bitm.valid) delete[] (vsx_bitmap_32bt*)result_bitm.data;
      data_a = new vsx_bitmap_32bt[bitm->size_x*bitm->size_y];
      data_b = new vsx_bitmap_32bt[bitm->size_x*bitm->size_y];
      result_bitm.data = data_a;
      result_bitm.valid = true;
      result_bitm.size_x = bitm->size_x;
      result_bitm.size_y = bitm->size_y;

      pthread_attr_init(&worker_t_attr);
      pthread_create(&worker_t, &worker_t_attr, &noise_worker, (void*)this);
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
  delete[] (vsx_bitmap_32bt*)result_bitm.data;
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


vsx_module* create_new_module(unsigned long module, void* args) {
  switch(module) {
    case 0: return (vsx_module*)(new module_texture_to_bitmap);
    case 1: return (vsx_module*)(new module_bitmap_to_particlesystem);
    case 2: return (vsx_module*)(new module_bitmap_add_noise);

  }
  return 0;
}

void destroy_module(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (module_texture_to_bitmap*)m; break;
    case 1: delete (module_bitmap_to_particlesystem*)m; break;
    case 2: delete (module_bitmap_add_noise*)m; break;
  }
}


unsigned long get_num_modules() {
  // we have only one module. it's id is 0
  return 3;
}  



