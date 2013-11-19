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
#include "pthread.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include <vector>
#include <map>
#ifndef VSXU_OPENGL_ES
  #include "vsx_glsl.h"
#endif

#ifndef VSXU_OPENGL_ES

class vsx_module_visual_fader : public vsx_module {
  // in
	vsx_module_param_texture* texture_a_in;
	vsx_module_param_texture* texture_b_in;
	vsx_module_param_float* fade_pos_in;
	vsx_module_param_float* fade_pos_from_engine;
	// out
	vsx_module_param_texture* texture_a_out;
	vsx_module_param_texture* texture_b_out;
	vsx_module_param_float* fade_pos_out;
	// internal
	vsx_texture texture_a;
	vsx_texture texture_b;
	vsx_bitmap ab;
	vsx_bitmap bb;

public:
	void module_info(vsx_module_info* info) {
	  info->identifier = "system;visual_fader";
	  info->in_param_spec = "texture_a_in:texture,texture_b_in:texture,fade_pos_in:float";
	  info->out_param_spec = "texture_a_out:texture,texture_b_out:texture,fade_pos_out:float";
	  info->component_class = "system";
	  loading_done = true;
	}

	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
		texture_a_in = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "texture_a_in");
		texture_b_in = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "texture_b_in");

		fade_pos_from_engine = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "fade_pos_from_engine");
		fade_pos_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "fade_pos_in");
		fade_pos_out = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "fade_pos_out");
		fade_pos_out->set(0.0f);

		texture_a_out = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "texture_a_out");
		texture_b_out = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "texture_b_out");

		ab.bpp = 4;
		ab.bformat = GL_RGBA;
		ab.size_x = ab.size_y = 2;
    ab.data = new vsx_bitmap_32bt[4];
    ((vsx_bitmap_32bt*)ab.data)[0] = 0xFFFF0000;
    ((vsx_bitmap_32bt*)ab.data)[1] = 0xFF000000;
    ((vsx_bitmap_32bt*)ab.data)[2] = 0xFF000000;
    ((vsx_bitmap_32bt*)ab.data)[3] = 0xFFFF0000;
		ab.valid = true;

		bb.bpp = 4;
		bb.bformat = GL_RGBA;
		bb.size_x = bb.size_y = 2;
    bb.data = new vsx_bitmap_32bt[4];
    ((vsx_bitmap_32bt*)bb.data)[0] = 0xFF000000;
    ((vsx_bitmap_32bt*)bb.data)[1] = 0xFF0000FF;
    ((vsx_bitmap_32bt*)bb.data)[2] = 0xFF0000FF;
    ((vsx_bitmap_32bt*)bb.data)[3] = 0xFF000000;
		bb.valid = true;

    texture_a.init_opengl_texture_2d();

    texture_b.init_opengl_texture_2d();

    texture_a.upload_ram_bitmap_2d(&ab,false);
    texture_b.upload_ram_bitmap_2d(&bb,false);

		texture_a_out->set(&texture_a);
		texture_b_out->set(&texture_b);
	}

	void output(vsx_module_param_abs* param)
	{
    VSX_UNUSED(param);
		vsx_texture** t_a;
			t_a = texture_a_in->get_addr();
		vsx_texture** t_b;
			t_b = texture_b_in->get_addr();
		if (t_a && t_b) {
			((vsx_module_param_texture*)texture_a_out)->set(*t_a);
			((vsx_module_param_texture*)texture_b_out)->set(*t_b);
		}
		if (fade_pos_from_engine->get() != 0.0f)
		{
			fade_pos_out->set(fade_pos_in->get());
		} else
		{
			fade_pos_out->set((float)fmod(engine->real_vtime, 1.0f));
		}
	}
};
#endif


class vsx_module_texture_translate : public vsx_module {
  // in
  vsx_module_param_texture* texture_info_param_in;
  vsx_module_param_float3* translation_vec;
  // out
  vsx_module_param_texture* texture_result;
  // internal
  vsx_texture* texture_out;
  vsx_transform_translate transform;

public:
  vsx_module_texture_translate() : transform(0, 0, 0) {}
  void module_info(vsx_module_info* info);
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
  void run();
  void on_delete();
};

//-----------------------------------------------------

class vsx_module_texture_scale : public vsx_module {
  // in
  vsx_module_param_texture* texture_info_param_in;
  vsx_module_param_float3* scale_vec;
  // out
  vsx_module_param_texture* texture_result;
  // internal
  vsx_texture* texture_out;
  vsx_transform_scale transform;

public:
  vsx_module_texture_scale() : transform(1, 1, 1) {}
  void module_info(vsx_module_info* info);
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
  void run();
  void on_delete();
};

//-----------------------------------------------------
class vsx_module_texture_rotate : public vsx_module {
  // in
  vsx_module_param_texture* texture_info_param_in;
  vsx_module_param_float3* rotation_axis;
  vsx_module_param_float* rotation_angle;
  // out
  vsx_module_param_texture* texture_result;
  // internal
  vsx_texture* texture_out;
  vsx_transform_rotate transform;

public:
  vsx_module_texture_rotate() : transform(0, 0, 0, 1) {}
  void module_info(vsx_module_info* info);
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
  void run();
  void on_delete();
};

//-----------------------------------------------------



//-------------------------------------------------------------------------------------- FU MO MO
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------

void vsx_module_texture_translate::module_info(vsx_module_info* info) {

  info->identifier = "texture;modifiers;translate";
#ifndef VSX_NO_CLIENT
  info->in_param_spec = "translation_vector:float3,texture_in:texture";
  info->out_param_spec = "texture_translate_out:texture";
  info->component_class = "texture";
#endif
}

void vsx_module_texture_translate::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
  loading_done = true;
	texture_info_param_in = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "texture_in");
	texture_out = new vsx_texture;
	//texture_info_param_in->set(*texture_out);
	translation_vec = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "translation_vector");
	translation_vec->set(0.0f, 0);
	translation_vec->set(0.0f, 1);
	translation_vec->set(0.0f, 2);
//	out_parameter_type_id = VSX_PARAM_ID_TEXTURE;
	texture_result = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture_translate_out");
//	texture_result->set(new vsx_texture_info);
}

void vsx_module_texture_translate::run() {
  vsx_texture** texture_info_in = texture_info_param_in->get_addr();
  if (texture_info_in)
  {
    texture_out->valid = (*texture_info_in)->valid;
    (*texture_out->texture_info) = (*(*texture_info_in)->texture_info);
  	float x = translation_vec->get(0);
  	float y = translation_vec->get(1);
  	float z = translation_vec->get(2);
  	vsx_transform_obj* prev_transform = (*texture_info_in)->get_transform();
  	transform.set_previous_transform(prev_transform);
  	transform.update(x, y, z);
  	texture_out->set_transform(&transform);
    ((vsx_module_param_texture*)texture_result)->set(texture_out);

  } else texture_result->valid = false;
}

void vsx_module_texture_translate::on_delete() {
  delete texture_out;
}

//--------------------------------------------------

void vsx_module_texture_scale::module_info(vsx_module_info* info) {

  info->identifier = "texture;modifiers;scale";
#ifndef VSX_NO_CLIENT
  info->in_param_spec = "scale_vector:float3,texture_in:texture";
  info->out_param_spec = "texture_scale_out:texture";
  info->component_class = "texture";
#endif
}

void vsx_module_texture_scale::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
//printf("scale_begin\n");
  loading_done = true;

	texture_info_param_in = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "texture_in");
//	texture_info_param_in->set(new vsx_texture_info);
	texture_out = new vsx_texture;
	//texture_info_param_in->set(*texture_out);

	scale_vec = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "scale_vector");
	scale_vec->set(1.0f, 0);
	scale_vec->set(1.0f, 1);
	scale_vec->set(1.0f, 2);
//	out_parameter_type_id = VSX_PARAM_ID_TEXTURE;
	texture_result = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture_scale_out");
//	texture_result->set(new vsx_texture_info);
}

void vsx_module_texture_scale::run() {
	vsx_texture** texture_info_in = texture_info_param_in->get_addr();
	if (texture_info_in)
 {
	  texture_out->valid = (*texture_info_in)->valid;
    (*texture_out->texture_info) = (*(*texture_info_in)->texture_info);
  	float x = scale_vec->get(0);
  	float y = scale_vec->get(1);
  	float z = scale_vec->get(2);
  	vsx_transform_obj* prev_transform = (*texture_info_in)->get_transform();
  	transform.set_previous_transform(prev_transform);
  	transform.update(x, y, z);
  	texture_out->set_transform(&transform);
  	((vsx_module_param_texture*)texture_result)->set(texture_out);
  }	else {
    texture_result->valid = false;
  }
}

void vsx_module_texture_scale::on_delete() {
  delete texture_out;
}

//--------------------------------------------------

void vsx_module_texture_rotate::module_info(vsx_module_info* info) {
  info->identifier = "texture;modifiers;rotate";
#ifndef VSX_NO_CLIENT
  info->in_param_spec = "texture_in:texture,rotation_angle:float,rotation_axis:float3";
  info->out_param_spec = "texture_rotate_out:texture";
  info->component_class = "texture";
#endif
}

void vsx_module_texture_rotate::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
	texture_info_param_in = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "texture_in");
//	texture_info_param_in->set(new vsx_texture_info);
  loading_done = true;
	texture_out = new vsx_texture;

	rotation_axis = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "rotation_axis");
	rotation_axis->set(0, 0);
	rotation_axis->set(0, 1);
	rotation_axis->set(1, 2);
	rotation_angle = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "rotation_angle");
	rotation_angle->set(0.0f);
//	out_parameter_type_id = VSX_PARAM_ID_TEXTURE;
	texture_result = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture_rotate_out");
//	texture_result->set(new vsx_texture_info);
}

void vsx_module_texture_rotate::run() {
//printf("rotate_begin\n");

	vsx_texture** texture_info_in = texture_info_param_in->get_addr();
	//printf("validness: %d\n",texture_info_param_in->valid);
//	if (texture_info_in->valid)
  if (texture_info_in)
  {

    texture_out->valid = (*texture_info_in)->valid;

//	if (texture_info_in->texture_info) {
    (*texture_out->texture_info) = (*(*texture_info_in)->texture_info);
//  }

  	float x = rotation_axis->get(0);
  	float y = rotation_axis->get(1);
  	float z = rotation_axis->get(2);
  	float a = rotation_angle->get()*360;
  	vsx_transform_obj* prev_transform = (*texture_info_in)->get_transform();
  	transform.set_previous_transform(prev_transform);
  	transform.update(a, x, y, z);
    //	if (texture_out)
  	texture_out->set_transform(&transform);
  	((vsx_module_param_texture*)texture_result)->set(texture_out);
  }	else {
    //printf("fooble\n");
    texture_result->valid = false;
  }
}

void vsx_module_texture_rotate::on_delete() {
  delete texture_out;
}


//##############################################################################
//##############################################################################
//##############################################################################
//##############################################################################



const unsigned int tex_wrap[] = {
  GL_REPEAT,
#ifndef VSXU_OPENGL_ES
  GL_CLAMP,
#else
  GL_REPEAT,
#endif
  GL_CLAMP_TO_EDGE,
#ifndef VSXU_OPENGL_ES
  GL_CLAMP_TO_BORDER,
  GL_MIRRORED_REPEAT
#else
  GL_REPEAT,
  GL_OES_texture_mirrored_repeat
#endif
};

const unsigned int tex_filter[] = {
  GL_NEAREST,
  GL_LINEAR,
  GL_NEAREST_MIPMAP_NEAREST,
  GL_LINEAR_MIPMAP_NEAREST,
  GL_NEAREST_MIPMAP_LINEAR,
  GL_LINEAR_MIPMAP_LINEAR
};


class vsx_module_texture_parameter : public vsx_module {
  // in
  vsx_module_param_texture* texture_info_param_in;
  vsx_module_param_int* wrap_s;
  vsx_module_param_int* wrap_t;
  vsx_module_param_int* min_filter;
  vsx_module_param_int* mag_filter;
  vsx_module_param_int* anisotropic_filter;
  vsx_module_param_float4* border_color;

  // out
  vsx_module_param_texture* texture_result;
  // internal
  vsx_texture** texture_out;

public:

  void module_info(vsx_module_info* info) {
    info->identifier = "texture;modifiers;tex_parameters";
  #ifndef VSX_NO_CLIENT
    info->in_param_spec = "texture_in:texture,parameters:complex{"
    "wrap_s:enum?repeat|"
  #ifndef VSXU_OPENGL_ES
    "clamp|"
  #endif
    "clamp_to_edge"
    "|clamp_to_border|mirrored_repeat"
    ","
    "wrap_t:enum?repeat|clamp|clamp_to_edge|clamp_to_border|mirrored_repeat,"
    "border_color:float4,"
    "anisotropic_filtering:enum?no|yes,"
    "min_filter:enum?nearest|linear|nearest_mipmap_nearest|linear_mipmap_nearest|nearest_mipmap_linear|linear_mipmap_linear,"
    "mag_filter:enum?nearest|linear"
    "}";
    info->out_param_spec = "texture_rotate_out:texture";
    info->component_class = "texture";
  #endif
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
    texture_info_param_in = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "texture_in");
    loading_done = true;

    wrap_t = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "wrap_s");
    wrap_s = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "wrap_t");
    min_filter = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "min_filter");
    mag_filter = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "mag_filter");
    anisotropic_filter = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "anisotropic_filtering");
    border_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "border_color");
    border_color->set(0,0);
    border_color->set(0,1);
    border_color->set(0,2);
    border_color->set(0,3);
    texture_result = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture_rotate_out");
  }

  void run() {
    texture_out = texture_info_param_in->get_addr();
    if (texture_out)
    {
     if (param_updates)
     {
        (*texture_out)->bind();
  #ifdef VSXU_OPENGL_ES
        if (GL_EXT_texture_filter_anisotropic)
  #endif
  #ifndef VSXU_OPENGL_ES
          if (GLEW_EXT_texture_filter_anisotropic)
  #endif
          {
          float rMaxAniso;
          glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &rMaxAniso);
          if (anisotropic_filter->get())
          glTexParameterf((*texture_out)->texture_info->ogl_type, GL_TEXTURE_MAX_ANISOTROPY_EXT, rMaxAniso);
          else
          glTexParameterf((*texture_out)->texture_info->ogl_type, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
        }

        float vals[4];
        vals[0] = border_color->get(0);
        vals[1] = border_color->get(1);
        vals[2] = border_color->get(2);
        vals[3] = border_color->get(3);

        glTexParameteri((*texture_out)->texture_info->ogl_type,GL_TEXTURE_MIN_FILTER, tex_filter[min_filter->get()]);
        glTexParameteri((*texture_out)->texture_info->ogl_type,GL_TEXTURE_MAG_FILTER, tex_filter[mag_filter->get()]);
  #ifndef VSXU_OPENGL_ES
        glTexParameterfv((*texture_out)->texture_info->ogl_type, GL_TEXTURE_BORDER_COLOR, vals);
  #endif
        glTexParameteri((*texture_out)->texture_info->ogl_type, GL_TEXTURE_WRAP_T, tex_wrap[wrap_t->get()]);
        glTexParameteri((*texture_out)->texture_info->ogl_type, GL_TEXTURE_WRAP_S, tex_wrap[wrap_s->get()]);
        (*texture_out)->_bind();

        --param_updates;
      }
      ((vsx_module_param_texture*)texture_result)->set(*texture_out);

    }	else {
      texture_result->valid = false;
    }
  }

  void start() {
    ++param_updates;
  }

};

//----------------------------------------------------------------------------

#ifndef VSXU_OPENGL_ES
#include "texture_blur.h"
#endif    // OPENGL_ES

#include "vsx_module_rendered_texture_single.h"
#include "vsx_module_rendered_texture_color_buffer.h"
#include "vsx_module_rendered_texture_color_depth_buffer.h"
#include "vsx_module_rendered_texture_render_buffer.h"



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
  switch(module) {
    case 0: return (vsx_module*)(new vsx_module_rendered_texture_single);
    case 1: return (vsx_module*)(new vsx_module_texture_translate);
    case 2: return (vsx_module*)(new vsx_module_texture_scale);
    case 3: return (vsx_module*)(new vsx_module_texture_rotate);
    case 4: return (vsx_module*)(new vsx_module_texture_parameter);
    case 5: return (vsx_module*)(new vsx_module_texture_blur);
    case 6: return (vsx_module*)(new vsx_module_visual_fader);
    case 7: return (vsx_module*)(new vsx_module_rendered_texture_color_depth_buffer);
    case 8: return (vsx_module*)(new vsx_module_rendered_texture_color_buffer);
    case 9: return (vsx_module*)(new vsx_module_render_buffer);
  };
  return 0;
}

void destroy_module(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (vsx_module_rendered_texture_single*)m; break;
    case 1: delete (vsx_module_texture_translate*)m; break;
    case 2: delete (vsx_module_texture_scale*)m; break;
    case 3: delete (vsx_module_texture_rotate*)m; break;
    case 4: delete (vsx_module_texture_parameter*)m; break;
    case 5: delete (vsx_module_texture_blur*)m; break;
    case 6: delete (vsx_module_visual_fader*)m; break;
    case 7: delete (vsx_module_rendered_texture_color_depth_buffer*)m; break;
    case 8: delete (vsx_module_rendered_texture_color_buffer*)m; break;
    case 9: delete (vsx_module_render_buffer*)m; break;
  }
}

unsigned long get_num_modules() {
  return 10;

}

