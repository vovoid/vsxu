/**
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Lesser General Public License (LGPL)
*
* VSXu Engine is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU Lesser General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


#include "_configuration.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include "vsx_math_3d.h"
#include "vsx_quaternion.h"

//-----------------------------------------------------------------------
//--- BUILD CUBEMAP TEXTURE ---------------------------------------------
//-----------------------------------------------------------------------

class vsx_build_cubemap_texture : public vsx_module {
  // in
	vsx_module_param_bitmap* positive_x;
	vsx_module_param_bitmap* negative_x;
	vsx_module_param_bitmap* positive_y;
	vsx_module_param_bitmap* negative_y;
	vsx_module_param_bitmap* positive_z;
	vsx_module_param_bitmap* negative_z;
	// out
	vsx_module_param_texture* texture_out;
	// internal
	vsx_texture my_tex;
	int need_to_run;
	vsx_bitmap* p_x;
	vsx_bitmap* n_x;
	vsx_bitmap* p_y;
	vsx_bitmap* n_y;
	vsx_bitmap* p_z;
	vsx_bitmap* n_z;
public:
  void module_info(vsx_module_info* info) {
    info->identifier = "texture;opengl;6bitm2cubemap";
    info->description = "Takes 6 bitmaps and builds a cubemap\ntexture.";
    info->in_param_spec = "bitmaps:complex{positive_x:bitmap,negative_x:bitmap,positive_y:bitmap,negative_y:bitmap,positive_z:bitmap,negative_z:bitmap}";
    info->out_param_spec = "texture_out:texture";
    info->component_class = "texture";
  }

	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
    p_x = n_x = p_y = n_y = p_z = n_z = 0;
    positive_x = (vsx_module_param_bitmap*)in_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"positive_x");
    negative_x = (vsx_module_param_bitmap*)in_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"negative_x");
    positive_y = (vsx_module_param_bitmap*)in_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"positive_y");
    negative_y = (vsx_module_param_bitmap*)in_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"negative_y");
    positive_z = (vsx_module_param_bitmap*)in_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"positive_z");
    negative_z = (vsx_module_param_bitmap*)in_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"negative_z");
    texture_out = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture_out");
    //texture_out->set(&my_tex);
    need_to_run = 1;
  }

	void run() {
    if (positive_x->get_addr() != p_x) {
      if (negative_x->get_addr() != n_x) {
        if (positive_y->get_addr() != p_y) {
          if (negative_y->get_addr() != n_y) {
            if (positive_z->get_addr() != p_z) {
              if (negative_z->get_addr() != n_z) {
                p_x = positive_x->get_addr();
                n_x = negative_x->get_addr();
                p_y = positive_y->get_addr();
                n_y = negative_y->get_addr();
                p_z = positive_z->get_addr();
                n_z = negative_z->get_addr();
                glEnable(GL_TEXTURE_CUBE_MAP_EXT);
                GLuint texture;
                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_CUBE_MAP_EXT, texture);

                gluBuild2DMipmaps( GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB, GL_RGB, p_x->size_x, p_x->size_y, p_x->bformat, GL_UNSIGNED_BYTE, p_x->data );
                gluBuild2DMipmaps( GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB, GL_RGB, n_x->size_x, n_x->size_y, n_x->bformat, GL_UNSIGNED_BYTE, n_x->data );
                gluBuild2DMipmaps( GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB, GL_RGB, p_y->size_x, p_y->size_y, p_y->bformat, GL_UNSIGNED_BYTE, p_y->data );
                gluBuild2DMipmaps( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB, GL_RGB, n_y->size_x, n_y->size_y, n_y->bformat, GL_UNSIGNED_BYTE, n_y->data );
                gluBuild2DMipmaps( GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB, GL_RGB, p_z->size_x, p_z->size_y, p_z->bformat, GL_UNSIGNED_BYTE, p_z->data );
                gluBuild2DMipmaps( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB, GL_RGB, n_z->size_x, n_z->size_y, n_z->bformat, GL_UNSIGNED_BYTE, n_z->data );

                glDisable(GL_TEXTURE_CUBE_MAP_EXT);
                my_tex.texture_info->ogl_id = texture;
                my_tex.texture_info->ogl_type = GL_TEXTURE_CUBE_MAP_EXT;
                my_tex.valid = true;
                texture_out->set(&my_tex);
                need_to_run = 0;
                loading_done = true;
                //printf("vsx_build_cubemap_texture: COMPLETE\n");
                return;
              }
            }
          }
        }
      }
    }
    if (
      (positive_x->get_addr() == 0) ||
      (negative_x->get_addr() == 0) ||
      (positive_y->get_addr() == 0) ||
      (negative_y->get_addr() == 0) ||
      (positive_z->get_addr() == 0) ||
      (negative_z->get_addr() == 0)
    ) {

      p_x = n_x = p_y = n_y = p_z = n_z = 0;
      my_tex.unload();
      texture_out->set(&my_tex);
    }
    need_to_run = 1;
  }

	void on_delete() {
  }
};


//-----------------------------------------------------------------------
//--- GL TEXTURE COORDINATE GENERATOR -----------------------------------
//-----------------------------------------------------------------------

const unsigned int genmodes[] = {
GL_OBJECT_LINEAR,
GL_EYE_LINEAR,
GL_SPHERE_MAP,
GL_NORMAL_MAP,
GL_REFLECTION_MAP
};


class vsx_texture_coord_gen : public vsx_module {
  // in
	vsx_module_param_render* render_in;
	vsx_module_param_int* gen_s;
  vsx_module_param_int* gen_t;
  vsx_module_param_int* gen_r;
  vsx_module_param_float4* parameter;
	// out
	vsx_module_param_render* render_out;
	// internal
public:
  void module_info(vsx_module_info* info) {
    info->identifier = "texture;opengl;texture_coord_gen";
    info->description = "Uses OpenGL to generate texture\ncoordinates on the fly for\nreflection/normal maps.";
    info->in_param_spec = "render_in:render,\
gen_s:enum?OFF|OBJECT_LINEAR|EYE_LINEAR|SPHERE_MAP|NORMAL_MAP|REFLECTION_MAP,\
gen_t:enum?OFF|OBJECT_LINEAR|EYE_LINEAR|SPHERE_MAP|NORMAL_MAP|REFLECTION_MAP,\
gen_r:enum?OFF|OBJECT_LINEAR|EYE_LINEAR|SPHERE_MAP|NORMAL_MAP|REFLECTION_MAP,\
parameter:float4\
";
    info->out_param_spec = "render_out:render";
    info->component_class = "texture";
    loading_done = true;
    info->tunnel = true;
  }

	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
    render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
    gen_s = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"gen_s");
    gen_s->set(0);
    gen_t = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"gen_t");
    gen_t->set(0);
    gen_r = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"gen_r");
    gen_r->set(0);
    parameter = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"parameter");
    parameter->set(0.0f,0);
    parameter->set(1.0f,0);
    parameter->set(0.0f,0);
    parameter->set(0.0f,0);
    render_out = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_in->set(0);
    render_out->set(0);
  }

	bool activate_offscreen() {
    GLfloat params[4];
    params[0] = parameter->get(0);
    params[1] = parameter->get(1);
    params[2] = parameter->get(2);
    params[3] = parameter->get(3);
    if (gen_s->get()) {
      glEnable(GL_TEXTURE_GEN_S);
      int mode = genmodes[gen_s->get()-1];
      glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, mode);
    }
    if (gen_t->get()) {
      glEnable(GL_TEXTURE_GEN_T);
      int mode = genmodes[gen_t->get()-1];
      glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, mode);
    }
    if (gen_r->get()) {
      glEnable(GL_TEXTURE_GEN_R);
      int mode = genmodes[gen_r->get()-1];
      glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, mode);
    }
    return true;
  }

	void deactivate_offscreen() {
    if (gen_s->get()) glDisable(GL_TEXTURE_GEN_S);
    if (gen_t->get()) glDisable(GL_TEXTURE_GEN_T);
    if (gen_r->get()) glDisable(GL_TEXTURE_GEN_R);
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

vsx_module* create_new_module(unsigned long module, void* args)
{
  VSX_UNUSED(args);

  glewInit();
  // as we have only one module available, don't look at the module variable, just return - for speed
  // otherwise you'd have something like,, switch(module) { case 0: break; }
  switch(module) {
    case 0 : return (vsx_module*)(new vsx_build_cubemap_texture);
    case 1 : return (vsx_module*)(new vsx_texture_coord_gen);
  }
  return 0;
}

void destroy_module(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (vsx_build_cubemap_texture*)m; break;
    case 1: delete (vsx_texture_coord_gen*)m; break;
  }
}

unsigned long get_num_modules() {
  // we have only one module. it's id is 0
  return 2;
}
