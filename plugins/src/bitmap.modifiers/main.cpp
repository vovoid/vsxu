#include "_configuration.h"
//----------------------------
#include "vsx_gl_global.h"
#include "vsx_math_3d.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include <pthread.h>
#include "main.h"


class module_texture_to_bitmap : public vsx_module {
  // in
	vsx_module_param_texture* texture_in;
	// out
	vsx_module_param_bitmap* result1;
	// internal
	
	vsx_bitmap bitm;
	int bitm_timestamp;
	
  vsx_texture* texture;

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
      texture->bind();
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
          
          if (bitm.data) delete[] bitm.data;
          bitm.data = new unsigned long[width*height];
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
      texture->_bind();
    }
  }
  void start() {
  }  
  
  void stop() {
  }
  
  void on_delete() {
    //printf("deleting bitmap..");
    if (bitm.valid)
    delete[] bitm.data;
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
        for (unsigned long y = 0; y < bitm->size_y; ++y) {
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
      for (unsigned long y = 0; y < bitm->size_y; ++y) {
        int cc = y*width;
        for (int x = 0; x < width; ++x) {
          (*particles.particles)[i].color.b = ((float)(unsigned char)((bitm->data[cc+x]&0x00FF0000)>>16))/255.0f;
          (*particles.particles)[i].color.g = ((float)(unsigned char)((bitm->data[cc+x]&0x0000FF00)>>8))/255.0f;
          (*particles.particles)[i].color.r = ((float)(unsigned char)((bitm->data[cc+x]&0x000000FF)))/255.0f;
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
      particlesystem_out->set_p(particles);
    }
  }
  void on_delete() {
    delete particles.particles;
  }
};






//###########################################################################################



vsx_module* create_new_module(unsigned long module) {
  switch(module) {
    case 0: return (vsx_module*)(new module_texture_to_bitmap);
    case 1: return (vsx_module*)(new module_bitmap_to_particlesystem);
  }
  return 0;
}

void destroy_module(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (module_texture_to_bitmap*)m; break;
    case 1: delete (module_bitmap_to_particlesystem*)m; break;
  }
}


unsigned long get_num_modules() {
  // we have only one module. it's id is 0
  return 2;
}  



