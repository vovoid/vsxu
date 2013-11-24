class vsx_module_rendered_texture_single : public vsx_module {
  // in
  vsx_module_param_render* render_in;
  vsx_module_param_int* texture_size;
  vsx_module_param_int* support_feedback;
  vsx_module_param_int* float_texture;
  vsx_module_param_int* alpha_channel;
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
  int support_feedback_int;
  int float_texture_int;
  int alpha_channel_int;

  GLuint glsl_prog;

  GLint	viewport[4];

  vsx_gl_state* gl_state;

public:
  vsx_module_rendered_texture_single() : texture(0),texture2(0),which_buffer(false) {};

void module_info(vsx_module_info* info) {
  info->identifier = "texture;buffers;render_surface_single";
  info->description =
    "This module captures rendering to a texture\n"
    "- color buffer RGBA\n"
    "- depth buffer 24-bit\n"
    "\n"
    "Everything you render connected to this module\n"
    "is stored in the texture and can be reused when\n"
    "rendering other objects.\n"
    "Via double-buffering, this module supports\n"
    "feedback loops - you can use its own texture\n"
    "to draw on itself.\n"
    "It supports floating point textures\n"
    "and optional alpha channel.\n"
    "Dynamic textures can be very useful!";

#ifndef VSX_NO_CLIENT
  info->in_param_spec = "render_in:render,"
    "texture_size:enum?"
      "2048x2048|"
      "1024x1024|"
      "512x512|"
      "256x256|"
      "128x128|"
      "64x64|"
      "32x32|"
      "16x16|"
      "8x8|"
      "4x4|"
      "VIEWPORT_SIZE|"
      "VIEWPORT_SIZE_DIV_2|"
      "VIEWPORT_SIZE_DIV_4|"
      "VIEWPORT_SIZEx2|"
      "VIEWPORT_SIZEx4,"
    "support_feedback:enum?no|yes,"
    "float_texture:enum?no|yes,"
    "alpha_channel:enum?no|yes,"
    "clear_color:float4"
  ;
  info->out_param_spec =
    "texture_out:texture"
  ;
  info->component_class = "texture";
#endif
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
  render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER, "render_in",false,false);
  render_in->run_activate_offscreen = true;
  res_x = 512;

  support_feedback = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "support_feedback");
  support_feedback->set(1);
  support_feedback_int = 1;

  float_texture = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "float_texture");
  float_texture->set(0);
  float_texture_int = 0;

  alpha_channel = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "alpha_channel");
  alpha_channel->set(1);
  alpha_channel_int = 1;

  clear_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "clear_color");
  clear_color->set(0,0);
  clear_color->set(0,1);
  clear_color->set(0,2);
  clear_color->set(1,3);


  texture_size = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "texture_size");
  texture_size->set(2);

  tex_size_internal = -1;

  texture_result = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture_out");

  allocate_second_texture = true;

  gl_state = vsx_gl_state::get_instance();

  start();
}

bool can_run()
{
  vsx_texture tex;
  return tex.has_buffer_support();
}

void start()
{

  which_buffer = false;
  texture = new vsx_texture;
  texture->set_gl_state(gl_state);
  texture->init_color_depth_buffer(res_x,res_x);
  texture->valid = false;
  texture_result->set(texture);

  texture2 = new vsx_texture;
  texture2->set_gl_state(gl_state);
  texture2->init_color_depth_buffer(res_x,res_x);
  texture2->valid = false;
}

bool activate_offscreen() {
  #if defined(VSXU_OPENGL_ES) || defined (__APPLE__)
    gl_state->viewport_get( viewport );
  #endif

  bool rebuild = false;

  if (support_feedback->get() != support_feedback_int)
  {
    support_feedback_int = support_feedback->get();
    rebuild = true;
  }

  if (alpha_channel->get() != alpha_channel_int)
  {
    alpha_channel_int = alpha_channel->get();
    rebuild = true;
  }

  if (float_texture->get() != float_texture_int)
  {
    float_texture_int = float_texture->get();
    rebuild = true;
  }


  if (texture_size->get() >= 10)
  {
    gl_state->viewport_get( viewport );
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

    if (0 == support_feedback_int)
    {
      texture->reinit_color_depth_buffer
      (
        res_x,
        res_y,
        float_texture_int,
        alpha_channel_int
      );
      texture->bind();
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,0);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      texture->_bind();

    }
    if (1 == support_feedback_int)
    {
      // feedback textures ignores foreign depth buffer...
      texture->reinit_color_depth_buffer
      (
        res_x,
        res_y,
        float_texture_int,
        alpha_channel_int
      );
      texture->bind();
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,0);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      texture->_bind();
      texture2->reinit_color_depth_buffer
      (
        res_x,
        res_y,
        float_texture_int,
        alpha_channel_int
      );
      texture2->bind();
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,0);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      texture2->_bind();
    }
  }

  if (!which_buffer || support_feedback->get() == 0)
    texture->begin_capture_to_buffer();
  else
    texture2->begin_capture_to_buffer();

  //printf("changing viewport to %d\n",res_x);
  glViewport(0,0,res_x,res_y);
  glDepthMask(GL_TRUE);
  glClearColor(clear_color->get(0),clear_color->get(1),clear_color->get(2),clear_color->get(3));
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer
  glEnable(GL_BLEND);
  glUseProgram(0);

  loading_done = true;
  return true;
}

void deactivate_offscreen()
{
  if (!which_buffer || support_feedback_int == 0)
  {
    if (texture)
    {
      texture->end_capture_to_buffer();
      texture->valid = true;
    }
    ((vsx_module_param_texture*)texture_result)->set(texture);
  }
  else
  {
    if (texture2)
    {
      texture2->end_capture_to_buffer();
      texture2->valid = true;
    }
    ((vsx_module_param_texture*)texture_result)->set(texture2);
  }

  which_buffer = !which_buffer;

  #if defined(VSXU_OPENGL_ES) || defined (__APPLE__)
  //printf("resetting viewport to %d %d %d %d\n",viewport[0],viewport[1],viewport[2],viewport[3]);
  glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);
  #endif
}

void stop()
{
  if (texture)
  {
    texture->deinit_buffer();
    #ifdef VSXU_DEBUG
      printf("deleting texture\n");
    #endif
    delete texture;
    texture = 0;

    if (allocate_second_texture && texture2) {
      texture2->deinit_buffer();
      delete texture2;
      texture2 = 0;
    }
  }
}

void on_delete() {
  stop();
}

~vsx_module_rendered_texture_single() {
  if (texture)
  delete texture;
  if (texture2)
  delete texture2;
}

};
