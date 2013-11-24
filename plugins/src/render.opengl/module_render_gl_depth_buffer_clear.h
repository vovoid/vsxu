class module_render_gl_depth_buffer_clear : public vsx_module
{
  // in
	vsx_module_param_render* render_in;
	// out
	vsx_module_param_render* render_result;
	// internal
public:

	void module_info(vsx_module_info* info)
	{
  	info->identifier = "renderers;opengl_modifiers;depth_buffer_clear";
  	info->description = "";

  	info->in_param_spec = "render_in:render";
  	info->out_param_spec = "render_out:render";
  	info->component_class = "render";
  	info->tunnel = true;
	}

	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
	{
	  loading_done = true;
		render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
    render_in->run_activate_offscreen = true;
		render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
	}
	bool activate_offscreen() {
    glClear(GL_DEPTH_BUFFER_BIT);
	  return true;
	}

	void deactivate_offscreen() {
	}

  void output(vsx_module_param_abs* param) { VSX_UNUSED(param);
	  render_result->set(render_in->get());
	}
};

