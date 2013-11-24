class module_render_gl_line_width : public vsx_module
{
  // in
	vsx_module_param_render* render_in;
  vsx_module_param_float* width;

  // out
	vsx_module_param_render* render_out;

  // internal
  GLfloat prev_width;

  vsx_gl_state* gl_state;

public:
  void module_info(vsx_module_info* info) {
    info->identifier = "renderers;opengl_modifiers;gl_line_width";
    info->description = "";
    info->in_param_spec = "render_in:render,width:float";
    info->out_param_spec = "render_out:render";
    info->component_class = "render";
    loading_done = true;
    info->tunnel = true; // always run this
  }

	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
    render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
    render_in->set(0);
    render_in->run_activate_offscreen = true;
    width = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"width");
    width->set(1.0f);
    render_out = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_out->set(0);
    gl_state = vsx_gl_state::get_instance();
  }
  bool activate_offscreen()
  {
    prev_width = gl_state->line_width_get();
    gl_state->line_width_set( width->get() );
    return true;
  }

	void deactivate_offscreen() {
    gl_state->line_width_set( prev_width );
  }
};


