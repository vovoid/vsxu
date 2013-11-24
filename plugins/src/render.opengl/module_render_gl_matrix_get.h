class module_render_gl_matrix_get : public vsx_module
{
  // in
	vsx_module_param_render* render_in;
	vsx_module_param_int* matrix_target;

  // out
	vsx_module_param_render* render_result;
  vsx_module_param_matrix* matrix_out;

  // internal
	vsx_matrix matrix;

  vsx_gl_state* gl_state;

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "renderers;opengl_modifiers;gl_matrix_get";
    info->description = "Gets the current matrix from openGL";
    info->in_param_spec =
  "\
  render_in:render,\
  matrix_target:enum?MODELVIEW|PROJECTION|TEXTURE";
    info->out_param_spec = "render_out:render,matrix_out:matrix";
    info->component_class = "render";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    matrix_target = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"matrix_target");
  	render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
  	render_in->set(0);
    matrix_out = (vsx_module_param_matrix*)out_parameters.create(VSX_MODULE_PARAM_ID_MATRIX,"matrix_out");
    matrix_out->set(matrix);
  	render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
  	render_result->set(1);

    gl_state = vsx_gl_state::get_instance();
  }


  void output(vsx_module_param_abs* param) {
    if (param == render_result) {
      // save current matrix
      gl_state->matrix_get_v(matrix_target_get_vsx[matrix_target->get()],matrix.m);
   	  matrix_out->set(matrix);
      render_result->set(1);
    }
  }
};


