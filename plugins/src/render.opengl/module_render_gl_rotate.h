class module_render_gl_rotate : public vsx_module
{
  // in
  GLfloat tmpMat[16];

  vsx_module_param_float3* axis;
  vsx_module_param_float* angle;

  vsx_module_param_render* render_in;
  // out
  vsx_module_param_render* render_result;
  // internal
  vsx_gl_state* gl_state;

public:
  void module_info(vsx_module_info* info);
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
  bool activate_offscreen();
  void deactivate_offscreen();
//  void run();
  void output(vsx_module_param_abs* param);
};


void module_render_gl_rotate::module_info(vsx_module_info* info)
{
  info->identifier = "renderers;opengl_modifiers;gl_rotate";
  info->description = "Multiplies the current modelview\n\
matrix with a rotation around an \n\
axis. \n\
I.e. rotates anything connected \n\
via it.";
  info->in_param_spec =
"\
render_in:render,\
axis:float3,\
angle:float\
";
  info->out_param_spec = "render_out:render";
  info->component_class = "render";
  info->tunnel = true; // always run this
}


void module_render_gl_rotate::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  axis = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"axis");
  axis->set(0,0);
  axis->set(0,1);
  axis->set(1,2);
  angle = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"angle");

	render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
  render_in->run_activate_offscreen = true;

	render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");

  gl_state = vsx_gl_state::get_instance();

}

bool module_render_gl_rotate::activate_offscreen() {
  vsx_vector bb(axis->get(0),axis->get(1),axis->get(2));
  bb.normalize();

  // save current matrix
  gl_state->matrix_get_v(VSX_GL_MODELVIEW_MATRIX,tmpMat);
  gl_state->matrix_mode(VSX_GL_MODELVIEW_MATRIX);
  gl_state->matrix_rotate_f(angle->get()*360.0f, bb.x, bb.y, bb.z);
  return true;
}

void module_render_gl_rotate::deactivate_offscreen() {
  // reset the matrix to previous value
  gl_state->matrix_mode(VSX_GL_MODELVIEW_MATRIX);
  gl_state->matrix_load_identity();
  gl_state->matrix_mult_f(tmpMat);
}

void module_render_gl_rotate::output(vsx_module_param_abs* param) { VSX_UNUSED(param);
  render_result->set(render_in->get());
}

