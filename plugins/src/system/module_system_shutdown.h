class module_system_shutdown : public vsx_module
{
  // in
	vsx_module_param_float* float_in;
	// out
	vsx_module_param_render* render_out;
	// internal
public:

void module_info(vsx_module_info* info)
{
  info->identifier = "system;shutdown";
  info->description = "If the shutdown value is above 1.0\n\
VSXu is shut down. This is\n\
mainly intended to end automatic\n\
presentations such as demos etc.";
  info->out_param_spec = "render_out:render";
  info->in_param_spec = "shutdown:float";
  info->component_class = "system";
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
	float_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"shutdown");
	float_in->set(0);
	render_out = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
  //--------------------------------------------------------------------------------------------------	
}

void run() {
  if (float_in->get() > 1.0) exit(0);
  render_out->set(1);
}

};

