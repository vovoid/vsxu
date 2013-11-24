#ifdef _WIN32
typedef struct {
  unsigned short r[256];
  unsigned short g[256];
  unsigned short b[256];
} gamma_s;
#endif

class module_output_screen_opengl : public vsx_module
{
  float internal_gamma;

  vsx_module_param_render* render_in;
  vsx_module_param_float* gamma_correction;
  vsx_module_param_float4* clear_color;

  float pre_material_colors[5][2][4];

  // don't mess with viewport or any opengl settings
  vsx_module_param_int* opengl_silent;

  vsx_gl_state* gl_state;

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "outputs;screen";
    info->output = 1;
    info->description =
      "The screen is the visual output of VSXu.\n"
      "Components that are not in a chain connected\n"
      "in the end to the server will not be run.\n"
    ;
    info->in_param_spec = "screen:render,"
                          "gamma_correction:float?max=4&min=0&nc=1,"
                          "clear_color:float4";
    info->component_class = "screen";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    VSX_UNUSED(out_parameters);
    loading_done = true;
    render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER, "screen");
    render_in->run_activate_offscreen = true;

    opengl_silent = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "opengl_silent");
    opengl_silent->set(0);

    gamma_correction = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"gamma_correction");
    gamma_correction->set(1.0f);
    internal_gamma = 1.0f;

    clear_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"clear_color");
    clear_color->set(0.0f,0);
    clear_color->set(0.0f,1);
    clear_color->set(0.0f,2);
    clear_color->set(1.0f,3);

    gl_state = vsx_gl_state::get_instance();
  }

  void set_gamma(float mgamma)
  {
  #ifdef _WIN32
   HDC hdc = wglGetCurrentDC();
    gamma_s mygamma;
    gamma_s* mgp = &mygamma;

    for (int i = 0; i < 256; ++i) {
      int v = (int)round((double)255 * pow(((double)i)/(double)255, mgamma));
      if (v > 255) v = 255;
      // You can adjust each curve separately, but you usually shouldn't need to.
      mygamma.r[i] = v << 8;
      mygamma.g[i] = v << 8;
      mygamma.b[i] = v << 8;
    }
    SetDeviceGammaRamp(hdc, (void*)mgp);
  #else
    VSX_UNUSED(mgamma);
  #endif
 }

  bool activate_offscreen()
  {
    if (opengl_silent->get() == 1) return true;
    if (internal_gamma != gamma_correction->get()) {
      internal_gamma = gamma_correction->get();
      set_gamma(internal_gamma);
    }

    // start by clearing the screen
    glClearColor(clear_color->get(0),clear_color->get(1),clear_color->get(2),clear_color->get(3));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    // identity matrices
    gl_state->matrix_load_identities();

    // set up blending
    gl_state->blend_set(1);
    gl_state->blend_func(VSX_GL_SRC_ALPHA, VSX_GL_ONE_MINUS_SRC_ALPHA);

    // set up polygon mode
    gl_state->polygon_mode_set(VSX_GL_FRONT, VSX_GL_FILL);
    gl_state->polygon_mode_set(VSX_GL_BACK, VSX_GL_FILL);

    // set up depth test & mask
    gl_state->depth_mask_set(0);
    gl_state->depth_test_set(0);
    //gl_state->depth_function_set( VSX_GL_LESS );

    // set up line width
    gl_state->line_width_set( 1.0f );

    const unsigned int lights[] = {GL_LIGHT0,GL_LIGHT1,GL_LIGHT2,GL_LIGHT3,GL_LIGHT4,GL_LIGHT5,GL_LIGHT6,GL_LIGHT7};
    glDisable(lights[0]);
    glDisable(lights[1]);
    glDisable(lights[2]);
    glDisable(lights[3]);
    glDisable(lights[4]);
    glDisable(lights[5]);
    glDisable(lights[6]);
    glDisable(lights[7]);

    gl_state->get_material_fv_all( &pre_material_colors[0][0][0] );

    // Implement default material settings
    gl_state->material_set_default();


    return true;
  }

  void deactivate_offscreen()
  {
    if (opengl_silent->get() == 1) return;
    #ifndef VSXU_OPENGL_ES_2_0
      gl_state->material_set_fv_all(&pre_material_colors[0][0][0]);
    #endif
  }

  void stop()
  {
    set_gamma(1.0f);
  }
};
