class module_render_gl_backface_culling : public vsx_module
{
  // in
  vsx_module_param_render* render_in;
  vsx_module_param_int* status;
  // out
  vsx_module_param_render* render_result;
  // internal
public:


void module_info(vsx_module_info* info)
{
  info->identifier = "renderers;opengl_modifiers;backface_culling";
  info->description = "Enables/Disables backfacing faces.";
  info->in_param_spec =
"render_in:render,status:enum?DISABLED|ENABLED";
  info->out_param_spec = "render_out:render";
  info->component_class = "render";
  info->tunnel = true;
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  status = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"status");
	render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
  render_in->run_activate_offscreen = true;
	render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
}
bool activate_offscreen() {
  // comments?! what are those? :) real coders don't need comments. real coders read opcodes in a hex editor.
  if (status->get()) {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

  } else {
    glDisable(GL_CULL_FACE);
  }
  return true;
}

void deactivate_offscreen() {
  glDisable(GL_CULL_FACE);
}

void output(vsx_module_param_abs* param) { VSX_UNUSED(param);
  render_result->set(render_in->get());
}
};

