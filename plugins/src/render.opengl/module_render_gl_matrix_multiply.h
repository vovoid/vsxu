class module_render_gl_matrix_multiply : public vsx_module
{
  // in
	GLfloat tmpMat[16];

  vsx_module_param_matrix* matrix_in;
	vsx_module_param_render* render_in;
	vsx_module_param_int* matrix_target_l;

  // out
	vsx_module_param_render* render_result;

  // internal
	vsx_matrix* mm;
	int active;

  vsx_gl_state* gl_state;

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "renderers;opengl_modifiers;gl_matrix_multiply";
    info->description = "If a module outputs a matrix\n\
as a result of a mathematical \n\
operation and you want to use it\n\
to transform a scene, this is the module\n\
for you.\n\
\n\
Try it with the bspline_matrix math\n\
module for some interresting results.";
    info->in_param_spec =
  "\
  render_in:render,\
  matrix_in:matrix,\
  matrix_target:enum?MODELVIEW|PROJECTION|TEXTURE";
    info->out_param_spec = "render_out:render";
    info->component_class = "render";
    info->tunnel = true; // always run this
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    //vsx_matrix foo;
    matrix_in = (vsx_module_param_matrix*)in_parameters.create(VSX_MODULE_PARAM_ID_MATRIX,"matrix_in");
    //matrix_in->set(foo);
    matrix_target_l = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"matrix_target");
  	render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
    render_in->run_activate_offscreen = true;

  	render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");

    gl_state = vsx_gl_state::get_instance();
  }

  bool activate_offscreen() {
    // save current matrix
    active = false;
    mm = matrix_in->get_addr();
    if (mm) {
      active = true;
      gl_state->matrix_get_v(matrix_target_get_vsx[matrix_target_l->get()],tmpMat);
      gl_state->matrix_mode( matrix_target_get_vsx[matrix_target_l->get()] );
      gl_state->matrix_mult_f(mm->m);
    }
    return true;
  }

  void deactivate_offscreen() {
    // reset the matrix to previous value
    if (active)
    {
      gl_state->matrix_mode( matrix_target_get_vsx[matrix_target_l->get()] );
      gl_state->matrix_load_identity();
      gl_state->matrix_mult_f( tmpMat );
    }
  }

  void output(vsx_module_param_abs* param) { VSX_UNUSED(param);
    render_result->set(render_in->get());
  }
};


