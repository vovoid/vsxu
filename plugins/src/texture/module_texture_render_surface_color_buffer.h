
class module_texture_render_surface_color_buffer : public vsx_module {
  // in
  vsx_module_param_render* render_in;
  vsx_module_param_int* texture_size;
  vsx_module_param_int* float_texture;
  vsx_module_param_int* alpha_channel;
  // out
  vsx_module_param_texture* texture_result;
  // internal
  int res_x, res_y;
  int dbuff;
  int tex_size_internal;
  vsx_texture* texture;

  int float_texture_int;
  int alpha_channel_int;

  GLuint glsl_prog;

  GLint	viewport[4];

  vsx_gl_state* gl_state;

public:
  module_texture_render_surface_color_buffer() : texture(0) {};

  void module_info(vsx_module_info* info)
  {
    info->identifier = "texture;buffers;render_surface_color_buffer";
    info->description =
      "This module captures rendering to a texture\n"
      "- only color buffer\n"
      "  thus RGB and optional Alpha\n"
      "Everything you render connected to this module\n"
      "is stored in the texture and can be reused when\n"
      "rendering other objects.\n"
      "It supports floating point textures and"
      "optional Alpha channel.\n"
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
      "alpha_channel:enum?no|yes"
    ;
    info->out_param_spec =
      "color_buffer:texture"
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

    texture_size = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "texture_size");
    texture_size->set(2);

    tex_size_internal = -1;

    texture_result = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"color_buffer");

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
    texture = new vsx_texture;
    texture->set_gl_state(gl_state);
    texture->init_color_depth_buffer(res_x,res_x);
    texture->valid = false;
    texture_result->set(texture);
  }

  bool activate_offscreen()
  {
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

      texture->reinit_color_depth_buffer
      (
        res_x,
        res_y,
        float_texture_int,
        alpha_channel_int
      );
    }


    texture->begin_capture_to_buffer();

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
      delete texture;
      texture = 0;
    }
  }

  void on_delete() {
    stop();
  }

  ~module_texture_render_surface_color_buffer() {
    if (texture)
    delete texture;
  }

};

