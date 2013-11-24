class module_render_gl_fog : public vsx_module
{
  // in
  vsx_module_param_render* render_in;
  vsx_module_param_int* status;
  vsx_module_param_float4* fog_color;
  vsx_module_param_float* fog_start;
  vsx_module_param_float* fog_end;
  // out
  vsx_module_param_render* render_result;
  // internal
public:

void module_info(vsx_module_info* info)
{
  info->identifier = "renderers;opengl_modifiers;gl_fog";
  info->description = "Enables the fog built into OpenGL";
  info->in_param_spec =
"render_in:render,status:enum?DISABLED|ENABLED,fog_color:float4?default_controller=controller_col,fog_start:float,fog_end:float";
  info->out_param_spec = "render_out:render";
  info->component_class = "render";
  info->tunnel = true;
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  status = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"status");
  fog_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"fog_color");
  fog_color->set(0,0);
  fog_color->set(0,1);
  fog_color->set(0,2);
  fog_color->set(0,3);
  fog_start = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"fog_start");
  fog_start->set(1.0f);
  fog_end = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"fog_end");
  fog_end->set(5.0f);
	render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
  render_in->run_activate_offscreen = true;
	render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
}
bool activate_offscreen() {
  GLfloat fogColor[] = {fog_color->get(0), fog_color->get(1), fog_color->get(2), fog_color->get(3)};
	#ifndef VSXU_OPENGL_ES
  glFogi (GL_FOG_MODE, GL_LINEAR);   // Fog fade using exponential function
	#endif
  glFogfv (GL_FOG_COLOR, fogColor);   // Set the fog color
  glFogf (GL_FOG_DENSITY, 0.35f);   // Set the density, don't make it too high.
  //glHint (GL_FOG_HINT, GL_NICEST);
  glFogf(GL_FOG_START, fog_start->get());				// Fog Start Depth
  glFogf(GL_FOG_END, fog_end->get());				// Fog End Depth
  // comments?! what are those? :) real coders don't need comments. real coders read opcodes in a hex editor.
  if (status->get()) {
    glEnable (GL_FOG);   // turn on fog, otherwise you won't see any
  } else {
    glDisable(GL_FOG);
  }
  return true;
}

void output(vsx_module_param_abs* param) { VSX_UNUSED(param);
  render_result->set(render_in->get());
}

void deactivate_offscreen() {
  glDisable(GL_FOG);
}
};

