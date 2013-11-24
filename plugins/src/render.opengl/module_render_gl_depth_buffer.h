class module_render_gl_depth_buffer : public vsx_module
{
  // in
	vsx_module_param_render* render_in;
	vsx_module_param_int* depth_test;
	vsx_module_param_int* depth_mask;

  // out
	vsx_module_param_render* render_result;

  // internal
  int old_depth_mask;
  int old_depth_test;

  vsx_gl_state* gl_state;

public:

	void module_info(vsx_module_info* info)
	{
  	info->identifier = "renderers;opengl_modifiers;depth_buffer";
  	info->description = "Enables or disables depth testing\n\
for all below in this chain.\n\
This is useful when you have a mesh. \n\
If no depth testing the faces will be drawn in the order that they appear\n\
in the mesh which is almost never good. (unless you\n\
have satadding blend mode). However in order to use this\n\
you have to enable the depth mask which can be done\n\
with the sister module to this one - depth_mask.\n\
More hints...\n\
|||\n\
If you have faces that are semi-transarent,\n\
disable depth testing and use the transparency\n\
renderer.\n\
";

  	info->in_param_spec = "render_in:render,depth_test:enum?DISABLED|ENABLED,depth_mask:enum?DISABLED|ENABLED";
  	info->out_param_spec = "render_out:render";
  	info->component_class = "render";
  	info->tunnel = true;
	}

	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
	{
	  loading_done = true;
	  depth_test = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"depth_test");
	  depth_mask = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"depth_mask");
		render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
    render_in->run_activate_offscreen = true;
		render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");

    gl_state = vsx_gl_state::get_instance();
	}

  bool activate_offscreen()
  {
    old_depth_mask = gl_state->depth_mask_get();
    old_depth_test = gl_state->depth_test_get();

    gl_state->depth_mask_set( depth_mask->get(), true );
    gl_state->depth_test_set( depth_test->get(), true );

	  return true;
	}

  void deactivate_offscreen()
  {
    gl_state->depth_mask_set( old_depth_mask );
    gl_state->depth_test_set( old_depth_test );
	}

  void output(vsx_module_param_abs* param) { VSX_UNUSED(param);
	  render_result->set(render_in->get());
	}
};


