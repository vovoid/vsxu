#include <texture/buffer/vsx_texture_buffer_color_depth.h>

class vsx_module_rendered_texture_single : public vsx_module {
  // in
  vsx_module_param_render* render_in;
  vsx_module_param_int* texture_size;
  vsx_module_param_int* support_feedback;
  vsx_module_param_int* float_texture;
  vsx_module_param_int* alpha_channel;
  vsx_module_param_int* multisample_in;
  vsx_module_param_int* min_mag_filter_in;
  vsx_module_param_float4* clear_color;
  vsx_module_param_float* size_x;
  vsx_module_param_float* size_y;

  // out
  vsx_module_param_texture* texture_result;

  // internal
  int res_x, res_y;
  int dbuff;
  int tex_size_internal;
  vsx_texture<>* texture = 0x0;
  vsx_texture<>* texture2 = 0x0;
  vsx_texture_buffer_color_depth buffer;
  vsx_texture_buffer_color_depth buffer2;

  bool which_buffer = false;
  bool allocate_second_texture;
  int support_feedback_cache;
  int float_texture_cache;
  int alpha_channel_cache;
  int multisample_cache;
  int min_mag_filter_cache;

  vsx_gl_state* gl_state;

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "texture;buffers;render_surface_single";

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

    info->in_param_spec = "render_in:render,"
        "options:complex"
        "{"
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
            "VIEWPORT_SIZEx4|"
            "CUSTOM_SIZE&nc=1,"
          "size_x:float,"
          "size_y:float,"
          "support_feedback:enum?no|yes,"
          "float_texture:enum?no|yes,"
        "alpha_channel:enum?no|yes&nc=1,"
        "multisample:enum?no|yes&nc=1,"
        "min_mag_filter:enum?nearest|linear&nc=1,"
          "clear_color:float4"
        "}"
    ;

    info->out_param_spec =
      "texture_out:texture"
    ;

    info->component_class =
      "texture";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
    render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER, "render_in",false,false);
    render_in->run_activate_offscreen = true;
    res_x = 512;

    support_feedback = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "support_feedback");
    support_feedback->set(1);
    support_feedback_cache = 1;

    float_texture = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "float_texture");
    float_texture->set(0);
    float_texture_cache = 0;

    alpha_channel = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "alpha_channel");
    alpha_channel->set(1);
    alpha_channel_cache = 1;

    clear_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "clear_color");
    clear_color->set(0,0);
    clear_color->set(0,1);
    clear_color->set(0,2);
    clear_color->set(1,3);

    multisample_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "multisample");
    multisample_in->set(0);
    multisample_cache = 0;

    min_mag_filter_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "min_mag_filter");
    min_mag_filter_in->set(1); // linear
    min_mag_filter_cache = 1;

    texture_size = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "texture_size");
    texture_size->set(2);

    size_x = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "size_x");
    size_x->set(32.0);

    size_y = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "size_y");
    size_y->set(32.0);

    tex_size_internal = -1;

    texture_result = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture_out");

    allocate_second_texture = true;

    gl_state = vsx_gl_state::get_instance();
  }

  bool can_run()
  {
    return vsx_texture_buffer_base::has_buffer_support();
  }

  void start()
  {
  }

  bool activate_offscreen() {
    bool rebuild = false;

    if (support_feedback->get() != support_feedback_cache)
    {
      support_feedback_cache = support_feedback->get();
      rebuild = true;
    }

    if (alpha_channel->get() != alpha_channel_cache)
    {
      alpha_channel_cache = alpha_channel->get();
      rebuild = true;
    }

    if (float_texture->get() != float_texture_cache)
    {
      float_texture_cache = float_texture->get();
      rebuild = true;
    }

    if (multisample_in->get() != multisample_cache)
    {
      multisample_cache = multisample_in->get();
      rebuild = true;
    }

    if (min_mag_filter_in->get() != min_mag_filter_cache)
    {
      min_mag_filter_cache = min_mag_filter_in->get();
      rebuild = true;
    }

    if (texture_size->get() >= 10)
    {
      int t_res_x = gl_state->viewport_get_width();
      int t_res_y = gl_state->viewport_get_height();

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


    if (texture_size->get() == 15)
    {
      tex_size_internal = texture_size->get();

      float i_size_x = size_x->get();
      float i_size_y = size_y->get();

      if (i_size_x < 1.0)
        i_size_x = 1.0;

      if (i_size_y < 1.0)
        i_size_y = 1.0;

      if ((int)i_size_x != res_x || (int)i_size_y != res_y)
      {
        res_x = (int)i_size_x;
        res_y = (int)i_size_y;
        rebuild = true;
      }
    }

    if (texture_size->get() != tex_size_internal || rebuild) {
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
        case 10:
          res_x = gl_state->viewport_get_width();
          res_y = gl_state->viewport_get_height();
        break;
        case 11:
          res_x = gl_state->viewport_get_width() * 0.5;
          res_y = gl_state->viewport_get_height() * 0.5;
        break;
        case 12:
          res_x = gl_state->viewport_get_width() * 0.25;
          res_y = gl_state->viewport_get_height() * 0.25;
        break;
        case 13:
          res_x = gl_state->viewport_get_width() * 2.0;
          res_y = gl_state->viewport_get_height() * 2.0;
        break;
        case 14:
          res_x = gl_state->viewport_get_width() * 4.0;
          res_y = gl_state->viewport_get_height() * 4.0;
        break;
      };

      if (0 == support_feedback_cache)
      {
        if (!texture)
          texture = new vsx_texture<>;

        if (texture2)
          buffer2.deinit(texture2);

        buffer.reinit
        (
          texture,
          res_x,
          res_y,
          float_texture_cache,
          alpha_channel_cache,
          multisample_cache,
          min_mag_filter_cache,
          0
        );

        texture->bind();
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,0);
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        texture->_bind();
      }

      if (1 == support_feedback_cache)
      {
        if (!texture)
          texture = new vsx_texture<>;

        buffer.reinit
        (
          texture,
          res_x,
          res_y,
          float_texture_cache,
          alpha_channel_cache,
          multisample_cache,
          min_mag_filter_cache,
          0
        );
        texture->bind();
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,0);
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        texture->_bind();


        if (!texture2)
          texture2 = new vsx_texture<>;

        buffer2.reinit
        (
          texture2,
          res_x,
          res_y,
          float_texture_cache,
          alpha_channel_cache,
          multisample_cache,
          min_mag_filter_cache,
          0
        );
        texture2->bind();
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,0);
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        texture2->_bind();
      }
    }

    if (!which_buffer || support_feedback->get() == 0)
      buffer.begin_capture_to_buffer();
    else
      buffer2.begin_capture_to_buffer();

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
    if (!which_buffer || support_feedback_cache == 0)
    {
      if (texture)
        buffer.end_capture_to_buffer();
      ((vsx_module_param_texture*)texture_result)->set(texture);
    }
    else
    {
      if (texture2)
        buffer2.end_capture_to_buffer();
      ((vsx_module_param_texture*)texture_result)->set(texture2);
    }

    which_buffer = !which_buffer;
  }

  void stop()
  {
    if (texture)
    {
      buffer.deinit(texture);
      delete texture;
      texture = 0;
    }

    if (allocate_second_texture && texture2)
    {
      buffer2.deinit(texture2);
      delete texture2;
      texture2 = 0;
    }
  }

  void on_delete()
  {
    stop();
  }

  ~vsx_module_rendered_texture_single()
  {
    if (texture)
      delete texture;

    if (texture2)
      delete texture2;
  }

};
