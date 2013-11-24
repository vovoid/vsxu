class module_texture_render_buffer : public vsx_module {
  // in
  vsx_module_param_render* render_in;
  vsx_module_param_int* texture_size;
  vsx_module_param_int* float_texture;
  vsx_module_param_int* alpha_channel;
  vsx_module_param_int* multisample;
  vsx_module_param_float4* clear_color;
  // out
  vsx_module_param_texture* texture_result;
  // internal
  int res_x, res_y;
  int dbuff;
  int tex_size_internal;
  vsx_texture* texture;
  bool allocate_second_texture;
  int float_texture_int;
  int alpha_channel_int;
  int multisample_int;

  GLuint glsl_prog;

  GLint	viewport[4];

  vsx_gl_state* gl_state;

public:
  module_texture_render_buffer() : texture(0) {}

void module_info(vsx_module_info* info) {
  info->identifier = "texture;buffers;render_buffer";
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
    "It supports floating point textures, optional\n"
    "Alpha channel and optional multisampling.\n"
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
    "float_texture:enum?no|yes,"
    "alpha_channel:enum?no|yes,"
    "multisample:enum?no|yes,"
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

  float_texture = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "float_texture");
  float_texture->set(0);
  float_texture_int = 0;

  alpha_channel = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "alpha_channel");
  alpha_channel->set(1);
  alpha_channel_int = 1;

  multisample = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "multisample");
  multisample_int = 0;

  clear_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "clear_color");
  clear_color->set(0,0);
  clear_color->set(0,1);
  clear_color->set(0,2);
  clear_color->set(1,3);


  texture_size = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "texture_size");
  texture_size->set(2);

  tex_size_internal = -1;

  texture_result = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture_out");

  gl_state = vsx_gl_state::get_instance();

  allocate_second_texture = true;
  start();
}

bool can_run()
{
  vsx_texture tex;
  return tex.has_buffer_support();
}

void start()
{
  texture = new vsx_texture;
  texture->set_gl_state(gl_state);
  texture->init_render_buffer(res_x,res_x);
  texture->valid = false;
  texture_result->set(texture);
}

bool activate_offscreen() {
  #if defined(VSXU_OPENGL_ES) || defined (__APPLE__)
    gl_state->viewport_get( viewport );
  #endif

  bool rebuild = false;

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

  if (multisample->get() != multisample_int)
  {
    multisample_int = multisample->get();
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


  if (texture_size->get() != tex_size_internal || rebuild)
  {
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

    texture->reinit_render_buffer
    (
      res_x,
      res_y,
      float_texture_int,
      alpha_channel_int,
      multisample_int
    );

    texture->bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,0);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    texture->_bind();
  }

  texture->begin_capture_to_buffer();

  glDepthMask(GL_TRUE);
  glEnable(GL_BLEND);
  glUseProgram(0);

  loading_done = true;
  return true;
}

void deactivate_offscreen()
{
  if (texture)
  {
    texture->end_capture_to_buffer();
  }

  ((vsx_module_param_texture*)texture_result)->set(texture);
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
  }
}

void on_delete() {
  stop();
}

~module_texture_render_buffer() {
  if (texture)
  delete texture;
}

};
