class vsx_module_rendered_texture_color_depth_buffer : public vsx_module
{
  // in
  vsx_module_param_render* my_render;
  vsx_module_param_int* texture_size;
  vsx_module_param_int* float_texture;
  vsx_module_param_int* alpha_channel;
  vsx_module_param_texture* depth_buffer_in;
  // out
  vsx_module_param_texture* texture_result;
  vsx_module_param_texture* depth_buffer_out;
  // internal
  int res_x, res_y;
  int dbuff;
  int tex_size_internal;
  vsx_texture* texture;
  vsx_texture depth_buffer_texture;

  int float_texture_int;
  int alpha_channel_int;
  GLuint depth_buffer_in_int;

  GLuint glsl_prog;

  GLint	viewport[4];
public:
  vsx_module_rendered_texture_color_depth_buffer() : texture(0) {};

  void module_info(vsx_module_info* info)
  {
    info->identifier = "texture;buffers;render_surface_color_depth_buffer";

    info->description =
      "This module captures rendering to a texture\n"
      "- color buffer RGBA\n"
      "- depth buffer 24-bit\n"
      "\n"
      "Everything you render connected to this module\n"
      "is stored in the texture and can be reused when\n"
      "rendering other objects.\n"
      "This module can also output depth buffer for\n"
      "Another identical module to use (shared depth\n"
      "buffer, or it can (via the input) use another\n"
      "module's depth buffer.\n"
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
      "float_texture:enum?no|yes,"
      "alpha_channel:enum?no|yes,"
      "depth_buffer:texture"
    ;
    info->out_param_spec =
      "color_buffer:texture,"
      "depth_buffer:texture"
    ;
    info->component_class = "texture";
  #endif
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
    my_render = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER, "render_in",false,false);
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

    depth_buffer_out = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"depth_buffer");
    depth_buffer_in = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"depth_buffer");
    depth_buffer_in_int = 0;

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
    texture->set_gl_state(engine->gl_state);
    texture->init_color_depth_buffer(res_x,res_x);
    texture->valid = false;
    texture_result->set(texture);
  }

  bool activate_offscreen() {
    #if defined(VSXU_OPENGL_ES) || defined (__APPLE__)
      engine->gl_state->viewport_get( viewport );
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

    if (depth_buffer_in->connected && depth_buffer_in->valid)
    {
      vsx_texture* depth_in = depth_buffer_in->get();
      if ( depth_buffer_in_int != depth_in->texture_info->ogl_id )
      {
        depth_buffer_in_int = depth_in->texture_info->ogl_id;
        rebuild = true;
      }
      // check if the sizes differ, if they do we can't proceed
    } else
    {
      if (0 != depth_buffer_in_int) rebuild = true;
      depth_buffer_in_int = 0;
    }

    if (texture_size->get() >= 10)
    {
      engine->gl_state->viewport_get( viewport );
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

      if (
          depth_buffer_in_int != 0
      )
      {
        vsx_texture* depth_in = depth_buffer_in->get();
        if (
            depth_in->texture_info->size_x != res_x ||
            depth_in->texture_info->size_y != res_y
          )
        {
          res_x = depth_in->texture_info->size_x;
          res_y = depth_in->texture_info->size_y;
        }
      }

      texture->reinit_color_depth_buffer
      (
        res_x,
        res_y,
        float_texture_int,
        alpha_channel_int,
        depth_buffer_in_int
      );
    }

    texture->begin_capture_to_buffer();

    //printf("changing viewport to %d\n",res_x);
//    glViewport(0,0,res_x,res_y);
    glDepthMask(GL_TRUE);

    glEnable(GL_BLEND);
    glUseProgram(0);

    depth_buffer_texture.texture_info->ogl_id = texture->get_depth_buffer_handle();
    depth_buffer_texture.texture_info->ogl_type = GL_TEXTURE_2D;
    depth_buffer_texture.texture_info->size_x = res_x;
    depth_buffer_texture.texture_info->size_y = res_y;
    depth_buffer_out->set(&depth_buffer_texture);


    loading_done = true;
    return true;
  }

  void deactivate_offscreen()
  {
    if (texture)
    {
      texture->end_capture_to_buffer();
      texture->valid = true;
    }
    ((vsx_module_param_texture*)texture_result)->set(texture);


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
    }
  }

  void on_delete() {
    stop();
  }

  ~vsx_module_rendered_texture_color_depth_buffer() {
    if (texture)
    delete texture;
  }

};

