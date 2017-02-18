class module_render_gl_frustum : public vsx_module
{
  // in
  vsx_module_param_float* left;
  vsx_module_param_float* right;
  vsx_module_param_float* bottom;
  vsx_module_param_float* top;
  vsx_module_param_float* z_near;
  vsx_module_param_float* z_far;

  vsx_module_param_render* render_in;
  // out
  vsx_module_param_render* render_result;

  // internal
  GLfloat tmpMat[16];
  vsx_gl_state* gl_state;

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "renderers;opengl_modifiers;gl_frustum";

    info->description =
      "";

    info->in_param_spec =
      "render_in:render,"
      "left:float,"
      "right:float,"
      "bottom:float,"
      "top:float,"
      "near:float,"
      "far:float"
    ;

    info->out_param_spec =
      "render_out:render";

    info->component_class =
      "render";

    info->tunnel = true; // always run this
  }


  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    left = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"left");
    left->set(-0.5);
    right = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"right");
    right->set(-0.5);
    bottom = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"bottom");
    bottom->set(-0.5);
    top = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"top");
    top->set(0.5);
    z_near = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"near");
    z_near->set(0.01);
    z_far = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"far");
    z_far->set(2000.0);

    render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
    render_in->run_activate_offscreen = true;

    render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");

    gl_state = vsx_gl_state::get_instance();
  }

  bool activate_offscreen()
  {
    // save current matrix
    gl_state->matrix_get_v( VSX_GL_PROJECTION_MATRIX, tmpMat );
    gl_state->matrix_mode( VSX_GL_PROJECTION_MATRIX );
    gl_state->matrix_load_identity();

    gl_state->matrix_frustum(left->get(), right->get(),
                             bottom->get(), top->get(),
                             z_near->get(), z_far->get()
                             );
    return true;
  }

  void deactivate_offscreen() {
    // reset the matrix to previous value
    gl_state->matrix_mode( VSX_GL_PROJECTION_MATRIX );
    gl_state->matrix_load_identity();
    gl_state->matrix_mult_f( tmpMat );
  }

  void output(vsx_module_param_abs* param) { VSX_UNUSED(param);
    render_result->set(render_in->get());
  }
};


