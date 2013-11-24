class module_system_blocker : public vsx_module
{
  // in
  vsx_module_param_float* block;
	vsx_module_param_render* render_in;
	// out
	vsx_module_param_render* render_result;
	// internal
public:
void module_info(vsx_module_info* info)
{
  info->identifier = "system;blocker";
//#ifndef VSX_NO_CLIENT
  info->description = "Blocks a rendering chain.\
If the control value is less\
than 0.5 the chain is blocked,\
if higher or equal it's run.";
  info->in_param_spec = "render_in:render,block:float";
  info->out_param_spec = "render_out:render";
  info->component_class = "system";
//#endif
  info->tunnel = true; // always run this
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  block = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"block");
  block->set(1.0f);
  
	render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
	render_in->set(0);
  render_in->run_activate_offscreen = true;
	render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
	render_result->set(0);
}

bool activate_offscreen() {
  if (engine->state == VSX_ENGINE_LOADING) return true;
  if (block->get() >= 0.5)
  return true;
  else
  return false;
}

};

