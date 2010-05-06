#include "_configuration.h"
#include "pthread.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include <vector>
#include <map>
#include "main.h"
#ifndef VSXU_OPENGL_ES
  #ifndef VSXU_MAC_XCODE
    #include "render_texture.h"
  #endif
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
		ab.data = new unsigned long[4];
		ab.data[0] = 0xFFFF0000;
		ab.data[1] = 0xFF000000;
		ab.data[2] = 0xFF000000;
		ab.data[3] = 0xFFFF0000;
		ab.valid = true;

		bb.bpp = 4;
		bb.bformat = GL_RGBA;
		bb.size_x = bb.size_y = 2;
		bb.data = new unsigned long[4];
		bb.data[0] = 0xFF000000;
		bb.data[1] = 0xFF0000FF;
		bb.data[2] = 0xFF0000FF;
		bb.data[3] = 0xFF000000;
		bb.valid = true;

		texture_a.locked = true;
	  texture_a.init_opengl_texture();

		texture_b.locked = true;
	  texture_b.init_opengl_texture();

		texture_a.upload_ram_bitmap(&ab,false);
		texture_b.upload_ram_bitmap(&bb,false);

		texture_a_out->set_p(texture_a);
		texture_b_out->set_p(texture_b);
	}

	void output(vsx_module_param_abs* param)
	{
		vsx_texture* t_a;
			t_a = texture_a_in->get_addr();
		vsx_texture* t_b;
			t_b = texture_b_in->get_addr();
		if (t_a && t_b) {
			((vsx_module_param_texture*)texture_a_out)->set_p(*t_a);
			((vsx_module_param_texture*)texture_b_out)->set_p(*t_b);
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

//GLuint fbo_stack[32];
//int fbo_stack_pos = 0;



class vsx_module_rendered_texture_single : public vsx_module {
  // in
	vsx_module_param_render* my_render;
	vsx_module_param_int* texture_size;
	vsx_module_param_int* support_feedback;
	vsx_module_param_int* float_texture;
	vsx_module_param_float4* clear_color;
	// out
	vsx_module_param_texture* texture_result;
	// internal
	int res_x, res_y;
	int dbuff;
	int tex_size_internal;
	vsx_texture* texture;
	vsx_texture* texture2;
	bool which_buffer;
	bool allocate_second_texture;
	int float_texture_int;

  GLint	viewport[4];
public:
	vsx_module_rendered_texture_single() : texture(0) {};

void module_info(vsx_module_info* info) {
  info->identifier = "texture;buffers;render_surface_single";
#ifndef VSX_NO_CLIENT
  info->in_param_spec = "render_in:render,"
  "texture_size:enum?2048x2048|1024x1024|512x512|256x256|128x128|64x64|32x32|16x16|8x8|4x4|VIEWPORT_SIZE|VIEWPORT_SIZE_DIV_2|VIEWPORT_SIZE_DIV_4|VIEWPORT_SIZEx2|VIEWPORT_SIZEx4,"
  "support_feedback:enum?no|yes,float_texture:enum?no|yes,clear_color:float4";
  info->out_param_spec = "texture_out:texture";
  info->component_class = "texture";
#endif
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
  //printf("a\n");
	my_render = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER, "render_in",false,false);
	//res_x = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "screen_width");
	//res_y = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "screen_height");
	res_x = 512;
	//res_x->set(f);
	//res_y->set(f);

	support_feedback = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "support_feedback");
	support_feedback->set(1);

	float_texture = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "float_texture");
	float_texture->set(0);
	float_texture_int = 0;

	clear_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "clear_color");
	clear_color->set(0,0);
	clear_color->set(0,1);
	clear_color->set(0,2);
	clear_color->set(1,3);


	texture_size = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "texture_size");
	texture_size->set(2);

	tex_size_internal = -1;

	texture_result = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture_out");
/**
    texture2->init_opengl_texture();
//    unsigned char* data2 = new unsigned char[res_x*res_y*4];
    texture2->bind();
    //gluBuild2DMipmaps(GL_TEXTURE_2D,4,res_x,res_y,GL_RGBA,GL_UNSIGNED_BYTE,data2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_LEVEL,0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, res_x, res_x, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    texture2->_bind();
//    delete[] data2;
    texture2->valid = true;
*/
  //glTexImage2D(GL_TEXTURE_2D, 0, 32, size_x, size_y, 0, bpp2, GL_UNSIGNED_BYTE, data2);
  allocate_second_texture = true;
  start();
  //
}

void start() {

  texture = new vsx_texture;

  which_buffer = false;
  texture->init_buffer(res_x,res_x);
  texture->valid = false;
  texture_result->set(*texture);

  allocate_second_texture = texture->get_fbo_status();

  if (allocate_second_texture) {
    texture2 = new vsx_texture;
    texture2->init_buffer(res_x,res_x);
    texture2->valid = false;
  }
}

bool activate_offscreen() {
  //printf("ac1\n");
  //printf("activate offscreen\n");
  //glGetIntegerv(GL_VIEWPORT, viewport);
  //printf("old viewport is %d %d %d %d\n",viewport[0],viewport[1],viewport[2],viewport[3]);
#if defined(VSXU_OPENGL_ES) || defined (__APPLE__)
  glGetIntegerv (GL_VIEWPORT, viewport);
#endif
  //**use_fbo = GLEW_EXT_framebuffer_object;

  /***if (use_fbo) {
    glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, (GLint *)&prev_buf);
    glPushAttrib(GL_ALL_ATTRIB_BITS);
  }*/

  bool rebuild = false;

  if (float_texture->get() != float_texture_int)
  {
    float_texture_int = float_texture->get();
    rebuild = true;
  }

  if (texture_size->get() >= 10)
  {
    glGetIntegerv (GL_VIEWPORT, viewport);
    int t_res_x = abs(viewport[2] - viewport[0]);
    int t_res_y = abs(viewport[3] - viewport[1]);

    if (texture_size->get() == 10) {
      if (t_res_x != res_x || t_res_y != res_y) rebuild = true;
    }

    if (texture_size->get() == 11) {
      if (t_res_x / 2 != res_x || t_res_y / 2 != res_y) rebuild = true;
    }

    if (texture_size->get() == 12) {
      if (t_res_x / 4 != res_x || t_res_y / 4 != res_y) rebuild = true;
    }

    if (texture_size->get() == 13) {
      if (t_res_x * 2 != res_x || t_res_y * 2 != res_y) rebuild = true;
    }

    if (texture_size->get() == 14) {
      if (t_res_x * 4 != res_x || t_res_y * 4 != res_y) rebuild = true;
    }
  }


  if (texture_size->get() != tex_size_internal || rebuild) {
    //printf("generating new framebuffer\n");
    tex_size_internal = texture_size->get();
    switch (tex_size_internal) {
      case 0: res_y = res_x = 2048; break;
      case 1: res_y = res_x = 1024; break;
      case 2: res_y = res_x = 512; break;
      case 3: res_y = res_x = 256; break;
      case 4: res_y = res_x = 128; break;
      case 5: res_y = res_x = 64; break;
      case 6: res_y = res_x = 32; break;
      case 7: res_y = res_x = 16; break;
      case 8: res_y = res_x = 8; break;
      case 9: res_y = res_x = 4; break;
      case 10: res_x = abs(viewport[2] - viewport[0]); res_y = abs(viewport[3] - viewport[1]); break;
      case 11: res_x = abs(viewport[2] - viewport[0]) / 2; res_y = abs(viewport[3] - viewport[1]) / 2; break;
      case 12: res_x = abs(viewport[2] - viewport[0]) / 4; res_y = abs(viewport[3] - viewport[1]) / 4; break;
      case 13: res_x = abs(viewport[2] - viewport[0]) * 2; res_y = abs(viewport[3] - viewport[1]) * 2; break;
      case 14: res_x = abs(viewport[2] - viewport[0]) * 4; res_y = abs(viewport[3] - viewport[1]) * 4; break;
    };

    //printf("res_x %d res_y %d\n",res_x, res_y);
    /**if (use_fbo) {
      texture2->bind();
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_LEVEL,0);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, res_x, res_x, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
      texture2->_bind();


    GLint prev_buf_l;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, (GLint *)&prev_buf_l);
    glGenFramebuffersEXT(1, &framebuffer_id);
    glGenTextures(1, &tex_id);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer_id);
    // Setup texture
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, res_x, res_x, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, tex_id, 0);

    glGenRenderbuffersEXT(1, &depthbuffer_id);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthbuffer_id);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, res_x, res_x);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthbuffer_id);
    texture->texture_info.ogl_id = tex_id;
    texture->texture_info.ogl_type = GL_TEXTURE_2D;
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, prev_buf_l);
    } else {*/
    texture->reinit_buffer(res_x, res_y,float_texture->get());
    if (texture->get_fbo_status() && support_feedback->get())
    texture2->reinit_buffer(res_x, res_y, float_texture->get());
    /**}*/
  }

  /***if (use_fbo) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer_id);
  }
  else {****/

  if (!which_buffer || support_feedback->get() == 0 || (!texture->get_fbo_status()))
    texture->begin_capture();
  else
    texture2->begin_capture();
  //**}

  //printf("changing viewport to %d\n",res_x);
	glViewport(0,0,res_x,res_y);
	glDepthMask(GL_TRUE);
	//glDisable(GL_DEPTH_TEST);
	glClearColor(clear_color->get(0),clear_color->get(1),clear_color->get(2),clear_color->get(3));
  //printf("clear buffer\n");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer
#ifdef __APPLE__
	glEnable(GL_BLEND);
#endif
  //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
  loading_done = true;
  return true;
};

void deactivate_offscreen() {
  //printf("dac1\n");
  //printf("texture end capture: %d\n",fbo_stack_pos);

  //glBindTexture(_iTextureTarget, _iTextureID);
/***  if (use_fbo) {
    //printf("fbo present\n");
    if (copy_to_texture->get()) {
      texture2->bind();
      glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, res_x, res_x);
      texture2->_bind();
    }
    //printf("%d re-binding framebuffer_id=%d\n",this,prev_buf);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, prev_buf);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPopAttrib();
  } else {
    texture->end_capture();
  }
*/

  if (!which_buffer || support_feedback->get() == 0 || (!texture->get_fbo_status()))
  {
    texture->end_capture();
    texture->valid = true;
    ((vsx_module_param_texture*)texture_result)->set_p(*texture);
  }
  else
  {
    texture2->end_capture();
    texture2->valid = true;
    ((vsx_module_param_texture*)texture_result)->set_p(*texture2);
  }

  which_buffer = !which_buffer;

//  printf("%d unbinding framebuffer_id=%d\n",this,framebuffer_id);

/*  --fbo_stack_pos;
  if (fbo_stack_pos-1 == -1) {
    printf("binding screen\n");
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  }
  else {
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    printf("%d re-binding framebuffer_id=%d\n",this,fbo_stack[fbo_stack_pos-1]);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_stack[fbo_stack_pos-1]);
	  //glDepthMask(GL_FALSE);
	  //glDisable(GL_DEPTH_TEST);
  }*/





  //
//  printf("single deactivateoff\n");
  //printf("resetting viewport to %d %d %d %d\n",viewport[0],viewport[1],viewport[2],viewport[3]);

#if defined(VSXU_OPENGL_ES) || defined (__APPLE__)
//	printf("resetting viewport to %d %d %d %d\n",viewport[0],viewport[1],viewport[2],viewport[3]);
	glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);
#endif
  /**if (copy_to_texture->get() && use_fbo)
  ((vsx_module_param_texture*)texture_result)->set_p(*texture2);
  else {
    texture->valid = true;
    ((vsx_module_param_texture*)texture_result)->set_p(*texture);
  }**/



}

void stop() {
  if (texture) {
    texture->deinit_buffer();
#ifdef VSXU_DEBUG
    printf("deleting texture\n");
#endif
    delete texture;
    texture = 0;

    if (allocate_second_texture) {
      texture2->deinit_buffer();
      delete texture2;
    }
	}
}

void on_delete() {
  stop();
}

~vsx_module_rendered_texture_single() {
	if (texture)
  delete texture;
}

};


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
  vsx_texture* texture_info_in = texture_info_param_in->get_addr();
  if (texture_info_in)
  {
    texture_out->valid = texture_info_in->valid;
    texture_out->rt = texture_info_in->rt;
  	texture_out->texture_info = texture_info_in->texture_info;
  	float x = translation_vec->get(0);
  	float y = translation_vec->get(1);
  	float z = translation_vec->get(2);
  	vsx_transform_obj* prev_transform = texture_info_in->get_transform();
  	transform.set_previous_transform(prev_transform);
  	transform.update(x, y, z);
  	texture_out->set_transform(&transform);
    ((vsx_module_param_texture*)texture_result)->set_p(*texture_out);

  } else texture_result->valid = false;

/*  //printf("translate_begin\n");
  //if (texture_info_param_in) {
  	vsx_texture* texture_info_in = &(texture_info_param_in->get());
  //	if (texture_info_in) {
//    	if (texture_info_in->texture_info) {
    	  texture_out->rt = texture_info_in->rt;
      	texture_out->texture_info.set_id(texture_info_in->texture_info.get_id());
      	texture_out->texture_info.set_type(texture_info_in->texture_info.get_type());

//    	}

    	vsx_transform_obj* prev_transform = texture_info_in->get_transform();
    	transform.set_previous_transform(prev_transform);
      //if (texture_out)
      texture_out->set_transform(&transform);
//    }
//  }
  //printf("hejeee\n");
*/
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
	vsx_texture* texture_info_in = texture_info_param_in->get_addr();
	if (texture_info_in)
 {
	  texture_out->valid = texture_info_in->valid;
    //if (texture_info_in->rt)
    texture_out->rt = texture_info_in->rt;
  	texture_out->texture_info = texture_info_in->texture_info;
  	float x = scale_vec->get(0);
  	float y = scale_vec->get(1);
  	float z = scale_vec->get(2);
  	vsx_transform_obj* prev_transform = texture_info_in->get_transform();
  	transform.set_previous_transform(prev_transform);
  	transform.update(x, y, z);
  	texture_out->set_transform(&transform);
  	((vsx_module_param_texture*)texture_result)->set_p(*texture_out);
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

	vsx_texture* texture_info_in = texture_info_param_in->get_addr();
	//printf("validness: %d\n",texture_info_param_in->valid);
//	if (texture_info_in->valid)
  if (texture_info_in)
  {

    //if (texture_info_in->rt)
    texture_out->rt = texture_info_in->rt;
    texture_out->valid = texture_info_in->valid;

//	if (texture_info_in->texture_info) {
  	texture_out->texture_info = texture_info_in->texture_info;
//  }

  	float x = rotation_axis->get(0);
  	float y = rotation_axis->get(1);
  	float z = rotation_axis->get(2);
  	float a = rotation_angle->get()*360;
  	vsx_transform_obj* prev_transform = texture_info_in->get_transform();
  	transform.set_previous_transform(prev_transform);
  	transform.update(a, x, y, z);
    //	if (texture_out)
  	texture_out->set_transform(&transform);
  	((vsx_module_param_texture*)texture_result)->set_p(*texture_out);
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
  vsx_texture* texture_out;

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
        texture_out->bind();
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
          glTexParameterf(texture_out->texture_info.ogl_type, GL_TEXTURE_MAX_ANISOTROPY_EXT, rMaxAniso);
          else
          glTexParameterf(texture_out->texture_info.ogl_type, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
        }

        float vals[4];
        vals[0] = border_color->get(0);
        vals[1] = border_color->get(1);
        vals[2] = border_color->get(2);
        vals[3] = border_color->get(3);

        glTexParameteri(texture_out->texture_info.ogl_type,GL_TEXTURE_MIN_FILTER, tex_filter[min_filter->get()]);
        glTexParameteri(texture_out->texture_info.ogl_type,GL_TEXTURE_MAG_FILTER, tex_filter[mag_filter->get()]);
  #ifndef VSXU_OPENGL_ES
        glTexParameterfv(texture_out->texture_info.ogl_type, GL_TEXTURE_BORDER_COLOR, vals);
  #endif
        glTexParameteri(texture_out->texture_info.ogl_type, GL_TEXTURE_WRAP_T, tex_wrap[wrap_t->get()]);
        glTexParameteri(texture_out->texture_info.ogl_type, GL_TEXTURE_WRAP_S, tex_wrap[wrap_s->get()]);
        texture_out->_bind();

        --param_updates;
      }
      ((vsx_module_param_texture*)texture_result)->set_p(*texture_out);

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
class vsx_module_texture_blur : public vsx_module {
  // in
	vsx_module_param_texture* glow_source;
	vsx_module_param_float* start_value;
	vsx_module_param_float* attenuation;
	vsx_module_param_int* texture_size;
	int tex_size_internal;

  int res_x, res_y;
	// out
	vsx_module_param_texture* texture_result;
	// internal
	vsx_texture* texture;
	vsx_texture* texture2;
	vsx_glsl shader;
	GLuint tex_id;
	GLuint glsl_offset_id,glsl_tex_id,glsl_attenuation;
  GLint	viewport[4];
public:


  ~vsx_module_texture_blur() {
		if (texture)
    delete texture;
  }

void module_info(vsx_module_info* info) {
	glewInit();
	texture = 0;
  info->identifier = "texture;effects;blur";
#ifndef VSX_NO_CLIENT
  info->in_param_spec = "glow_source:texture,start_value:float,attenuation:float,texture_size:enum?2048x2048|1024x1024|512x512|256x256|128x128|64x64|32x32|16x16|8x8|4x4|VIEWPORT_SIZE|VIEWPORT_SIZE_DIV_2|VIEWPORT_SIZE_DIV_4|VIEWPORT_SIZEx2";
  info->out_param_spec = "texture_out:texture";
  info->component_class = "texture";
#endif
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
	glow_source = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "glow_source",false,false);
	texture_result = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture_out");

  texture_size = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"texture_size");
  texture_size->set(3);
  tex_size_internal = 3;

  texture = new vsx_texture;
  res_x = res_y = 256;
  texture->init_buffer(res_x,res_y);
  texture->valid = true;

  texture2 = new vsx_texture;
  texture2->init_buffer(res_x,res_y);
  texture2->valid = true;


  start_value = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"start_value");
  start_value->set(1.0f);
  attenuation = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"attenuation");
  attenuation->set(1.0f);

  texture_result->set_p(*texture);

#ifdef __APPLE__
	return;
	shader.vertex_program = "\
	void main()\n\
	{\n\
	  gl_TexCoord[0] = gl_MultiTexCoord0;\n\
	  gl_Position = ftransform();\n\
	}\n\
	";

	shader.fragment_program = "\
	uniform sampler2D GlowTexture;\n\
	uniform vec2      texOffset;\n\
	uniform float     attenuation;\n\
	void main(void)\n\
	{\n\
	  vec4  finalColor = vec4(0.0,0.0,0.0,0.0);\n\
	\n\
	  vec2 offset = vec2(-4.0,-4.0) * (texOffset) + gl_TexCoord[0].xy;\n\
	  finalColor += 0.0217 * texture2D(GlowTexture, offset);\n\
	  offset += texOffset;\n\
	  finalColor += 0.0434 * texture2D(GlowTexture, offset);\n\
	  offset += texOffset;\n\
	  finalColor += 0.0869 * texture2D(GlowTexture, offset);\n\
	  offset += texOffset;\n\
	  finalColor += 0.1739 * texture2D(GlowTexture, offset);\n\
	  offset += texOffset;\n\
	  finalColor += 0.3478 * texture2D(GlowTexture, offset);\n\
	  offset += texOffset;\n\
	  finalColor += 0.1739 * texture2D(GlowTexture, offset);\n\
	  offset += texOffset;\n\
	  finalColor += 0.0869 * texture2D(GlowTexture, offset);\n\
	  offset += texOffset;\n\
	  finalColor += 0.0434 * texture2D(GlowTexture, offset);\n\
	  offset += texOffset;\n\
	  finalColor += 0.0217 * texture2D(GlowTexture, offset);\n\
	  gl_FragColor = finalColor * attenuation; \n\
	}\n\
	";
#else
shader.vertex_program = "\
varying vec2 texcoord;\n\
void main()\n\
{\n\
  texcoord = gl_MultiTexCoord0.st;\n\
  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n\
}\n\
";

shader.fragment_program = "\
uniform sampler2D GlowTexture;\n\
uniform vec2      texOffset;\n\
uniform float     attenuation;\n\
varying vec2 texcoord;\n\
void main(void)\n\
{\n\
  vec4  finalColor = vec4(0.0,0.0,0.0,0.0);\n\
\n\
  vec2 offset = vec2(-4.0,-4.0) * (texOffset) + texcoord;\n\
  finalColor += 0.0217 * texture2D(GlowTexture, offset);\n\
  offset += texOffset;\n\
  finalColor += 0.0434 * texture2D(GlowTexture, offset);\n\
  offset += texOffset;\n\
  finalColor += 0.0869 * texture2D(GlowTexture, offset);\n\
  offset += texOffset;\n\
  finalColor += 0.1739 * texture2D(GlowTexture, offset);\n\
  offset += texOffset;\n\
  finalColor += 0.3478 * texture2D(GlowTexture, offset);\n\
  offset += texOffset;\n\
  finalColor += 0.1739 * texture2D(GlowTexture, offset);\n\
  offset += texOffset;\n\
  finalColor += 0.0869 * texture2D(GlowTexture, offset);\n\
  offset += texOffset;\n\
  finalColor += 0.0434 * texture2D(GlowTexture, offset);\n\
  offset += texOffset;\n\
  finalColor += 0.0217 * texture2D(GlowTexture, offset);\n\
  gl_FragColor = finalColor * attenuation; \n\
}\n\
";
#endif

/*shader.fragment_program = "\
uniform sampler2D GlowTexture;\n\
uniform vec2      texOffset;\n\
void main(void)\n\
{\n\
  vec4  blurFilter[9],\n\
        finalColor = vec4(0.0,0.0,0.0,0.0);\n\
\n\
  float xOffset    = texOffset.x,\n\
        yOffset    = texOffset.y;\n\
\n\
  blurFilter[0]  = vec4( 4.0*xOffset, 4.0*yOffset, 0.0, 0.0217);\n\
  blurFilter[1]  = vec4( 3.0*xOffset, 3.0*yOffset, 0.0, 0.0434);\n\
  blurFilter[2]  = vec4( 2.0*xOffset, 2.0*yOffset, 0.0, 0.0869);\n\
  blurFilter[3]  = vec4( 1.0*xOffset, 1.0*yOffset, 0.0, 0.1739);\n\
  blurFilter[4]  = vec4(         0.0,         0.0, 0.0, 0.3478);\n\
  blurFilter[5]  = vec4(-1.0*xOffset,-1.0*yOffset, 0.0, 0.1739);\n\
  blurFilter[6]  = vec4(-2.0*xOffset,-2.0*yOffset, 0.0, 0.0869);\n\
  blurFilter[7]  = vec4(-3.0*xOffset,-3.0*yOffset, 0.0, 0.0434);\n\
  blurFilter[8]  = vec4(-4.0*xOffset,-4.0*yOffset, 0.0, 0.0217);\n\
\n\
  for (int i = 0;i< 9;i++)\n\
    finalColor += texture2D(GlowTexture, gl_TexCoord[0].xy + blurFilter[i].xy) * blurFilter[i].w;\n\
\n\
  gl_FragColor = finalColor*1.15;\n\
}\n\
";
*/
  vsx_string shader_res = shader.link();
//  printf("shader res: %s\n",shader_res.c_str());
  glsl_tex_id = glGetUniformLocationARB(shader.prog,"GlowTexture");
	glsl_offset_id = glGetUniformLocationARB(shader.prog,"texOffset");
	glsl_attenuation = glGetUniformLocationARB(shader.prog,"attenuation");

}

bool activate_offscreen() {
  return true;
};

void deactivate_offscreen() {
}

void run() {
#ifdef __APPLE__
	return;
#endif
  if (texture == 0) {
	//printf("############################# texture is 0 and creating new one...\n");
    texture = new vsx_texture;

    tex_size_internal = 3;
    texture->init_buffer(res_x,res_y);
    //texture->valid = false;

    texture2 = new vsx_texture;
    texture2->init_buffer(res_x,res_y);
    //texture2->valid = false;
  }
  bool rebuild = false;
  if (texture_size->get() >= 10)
  {
    glGetIntegerv (GL_VIEWPORT, viewport);
    int t_res_x = abs(viewport[2] - viewport[0]);
    int t_res_y = abs(viewport[3] - viewport[1]);

    if (texture_size->get() == 10) {
      if (t_res_x != res_x || t_res_y != res_y) rebuild = true;
    }

    if (texture_size->get() == 11) {
      if (t_res_x / 2 != res_x || t_res_y / 2 != res_y) rebuild = true;
    }

    if (texture_size->get() == 12) {
      if (t_res_x / 4 != res_x || t_res_y / 4 != res_y) rebuild = true;
    }

    if (texture_size->get() == 13) {
      if (t_res_x * 2 != res_x || t_res_y * 2 != res_y) rebuild = true;
    }
  }


  if (texture_size->get() != tex_size_internal || rebuild) {
    //printf("generating new framebuffer\n");
    tex_size_internal = texture_size->get();
    switch (tex_size_internal) {
      case 0: res_y = res_x = 2048; break;
      case 1: res_y = res_x = 1024; break;
      case 2: res_y = res_x = 512; break;
      case 3: res_y = res_x = 256; break;
      case 4: res_y = res_x = 128; break;
      case 5: res_y = res_x = 64; break;
      case 6: res_y = res_x = 32; break;
      case 7: res_y = res_x = 16; break;
      case 8: res_y = res_x = 8; break;
      case 9: res_y = res_x = 4; break;
      case 10: res_x = abs(viewport[2] - viewport[0]); res_y = abs(viewport[3] - viewport[1]); break;
      case 11: res_x = abs(viewport[2] - viewport[0]) / 2; res_y = abs(viewport[3] - viewport[1]) / 2; break;
      case 12: res_x = abs(viewport[2] - viewport[0]) / 4; res_y = abs(viewport[3] - viewport[1]) / 4; break;
      case 13: res_x = abs(viewport[2] - viewport[0]) * 2; res_y = abs(viewport[3] - viewport[1]) * 2; break;
    };
    //printf("############################# texture init due to size change...\n");
    texture->init_buffer(res_x,res_y);
    texture2->init_buffer(res_x,res_y);
  }


  vsx_texture* ti = glow_source->get_addr();

  if (ti) {
    ti->bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,0);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    ti->_bind();

    //glGetIntegerv(GL_VIEWPORT, viewport);
	//
    //printf("sprog: %d\n",shader.prog);
	 //printf("tex id: %d\n",glsl_tex_id);
	//printf("attenuation id: %d\n",glsl_attenuation);
	//printf("ofs id: %d\n",glsl_offset_id);
    float a = start_value->get()*0.001f;
//printf("a = %f\n",a);

    texture->begin_capture();
      glViewport(0,0,res_x,res_y);
      loading_done = true;
      glColor4f(1,1,1,1);
      glDisable(GL_BLEND);

      if (GLEW_VERSION_1_3)
        glActiveTexture(GL_TEXTURE0);
      ti->bind();
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        shader.begin();
          glUniform1iARB(glsl_tex_id,0);
          glUniform2fARB(glsl_offset_id,a,0.0f);
          glUniform1fARB(glsl_attenuation, attenuation->get());
      	  glBegin(GL_QUADS);
        	  glTexCoord2f(0.0f,0.0f);
            glVertex3f(-1.0f, -1.0f, 0.0f);
        	  glTexCoord2f(0.0f,1.0f);
            glVertex3f(-1.0f,  1.0f, 0.0f);
        	  glTexCoord2f(1.0f,1.0f);
            glVertex3f( 1.0f,  1.0f, 0.0f);
        	  glTexCoord2f(1.0f,0.0f);
            glVertex3f( 1.0f, -1.0f, 0.0f);
        	glEnd();
        shader.end();
      ti->_bind();
    texture->end_capture();
    //
    texture->valid = true;
    texture2->begin_capture();
      glViewport(0,0,res_x,res_y);
      loading_done = true;
      glColor4f(1,1,1,1);
      glDisable(GL_BLEND);
      if (GLEW_VERSION_1_3)
      glActiveTexture(GL_TEXTURE0);
      texture->bind();
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);

        shader.begin();
          glUniform1iARB(glsl_tex_id,0);
          //glUniform2fARB(glsl_offset_id,0.0f,a);
          glUniform2fARB(glsl_offset_id,0.0f,a);
          glUniform1fARB(glsl_attenuation, attenuation->get());
      	  glBegin(GL_QUADS);
        	  glTexCoord2f(0.0f,0.0f);
            glVertex3f(-1.0f, -1.0f, 0.0f);
        	  glTexCoord2f(0.0f,1.0f);
            glVertex3f(-1.0f,  1.0f, 0.0f);
        	  glTexCoord2f(1.0f,1.0f);
            glVertex3f( 1.0f,  1.0f, 0.0f);
        	  glTexCoord2f(1.0f,0.0f);
            glVertex3f( 1.0f, -1.0f, 0.0f);
        	glEnd();
        shader.end();
      texture->_bind();
    texture2->end_capture();
    texture2->valid = true;

    //glTexParameteri(texture->texture_info.ogl_type,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(texture->texture_info.ogl_type,GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /*
    //texture2->begin_capture();
    //glViewport (0,0,TSIZE,TSIZE);
    //glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer
    //glActiveTexture(GL_TEXTURE0);
    texture->bind();
    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
    //glScalef(1,1,1);
    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    //glColor4f(1,1,1,1);
    //glBlendFunc(GL_ONE,GL_ONE);
    //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    //glEnable(GL_BLEND);
    //shader.begin();

    //glDisable(GL_BLEND);
    glUniform2fARB(glsl_offset_id,0,a);
    glUniform1fARB(glsl_attenuation,attenuation->get());
    shader.begin();
    //b = 0;
    //a = start_value->get()*0.001f;
    //while (b < round(iterations->get())) {
  	  glBegin(GL_QUADS);
    	  glTexCoord2f(0.0f,0.0f);
        glVertex3f(-1.0f, -1.0f, 0.0f);
    	  glTexCoord2f(0.0f,1.0f);
        glVertex3f(-1.0f,  1.0f, 0.0f);
    	  glTexCoord2f(1.0f,1.0f);
        glVertex3f( 1.0f,  1.0f, 0.0f);
    	  glTexCoord2f(1.0f,0.0f);
        glVertex3f( 1.0f, -1.0f, 0.0f);
    	glEnd();
    	//a *= multiplier->get();
    	//++b;
    //}
    texture->_bind();
    texture2->end_capture();
    texture2->valid = true;
    shader.end();
    glTexParameteri(texture2->texture_info.ogl_type,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(texture2->texture_info.ogl_type,GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //glPopMatrix();
    while (a < 0.02) {
      glUniform4fARB(glsl_offset_id,0,a,1,1);
  	  glBegin(GL_QUADS);
    	  glTexCoord2f(0.0f,0.0f);
        glVertex3f(-1.0f, -1.0f, 0.0f);
    	  glTexCoord2f(0.0f,1.0f);
        glVertex3f(-1.0f,  1.0f, 0.0f);
    	  glTexCoord2f(1.0f,1.0f);
        glVertex3f( 1.0f,  1.0f, 0.0f);
    	  glTexCoord2f(1.0f,0.0f);
        glVertex3f( 1.0f, -1.0f, 0.0f);
    	glEnd();
    	a += 0.002;
    }*/
    //}

    //glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);

    texture_result->set_p(*texture2);
  }
}

void stop() {
#ifdef __APPLE__
	return;
#endif
  shader.stop();
  if (texture) {
    texture->deinit_buffer();
    texture2->deinit_buffer();
    delete texture->transform_obj;
    delete texture2->transform_obj;
  	delete texture;
  	delete texture2;
    texture = 0;
    texture2 = 0;
  }
}

void start() {
#ifdef __APPLE__
	return;
#endif
  shader.link();
	glsl_offset_id = glGetUniformLocationARB(shader.prog,"texOffset");
	glsl_tex_id = glGetUniformLocationARB(shader.prog,"GlowTexture");
	glsl_attenuation = glGetUniformLocationARB(shader.prog,"attenuation");
}

void on_delete() {
  if (texture) {
    texture->deinit_buffer();
    texture2->deinit_buffer();
    delete texture->transform_obj;
    delete texture2->transform_obj;
  	delete texture;
  	delete texture2;
    texture = 0;
    texture2 = 0;
  }
}

};
#endif    // OPENGL_ES

#if BUILDING_DLL
vsx_module* create_new_module(unsigned long module) {
  /*if (!glewinit) {
    // we sneak in and init glew :) as we need it
//    printf("textures.bitmaps-lib: glewinit\n");
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
  //    printf("Error: %s\n", glewGetErrorString(err));
    }
  } */
glewInit();
  switch(module) {
    case 0: return (vsx_module*)(new vsx_module_rendered_texture_single);
    case 1: return (vsx_module*)(new vsx_module_texture_translate);
    case 2: return (vsx_module*)(new vsx_module_texture_scale);
    case 3: return (vsx_module*)(new vsx_module_texture_rotate);
    case 4: return (vsx_module*)(new vsx_module_texture_parameter);
    case 5: return (vsx_module*)(new vsx_module_texture_blur);
    case 6: return (vsx_module*)(new vsx_module_visual_fader);
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
  }
}

unsigned long get_num_modules() {
  return 7;

}

#endif

//#!
