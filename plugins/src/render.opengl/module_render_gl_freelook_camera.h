class module_render_gl_freelook_camera : public vsx_module
{
  GLfloat matrix_projection[16];
  GLfloat matrix_modelview[16];
  // in
  vsx_module_param_float3* position;
  vsx_module_param_float3* rotation;
  vsx_module_param_float3* upvector;
  vsx_module_param_float* fov;
  vsx_module_param_float* near_clipping;
  vsx_module_param_float* far_clipping;

  vsx_module_param_render* render_in;
  // out
  vsx_module_param_render* render_result;

  // internal
  vsx_gl_state* gl_state;

public:


void module_info(vsx_module_info* info)
{
  info->identifier = "renderers;opengl_modifiers;cameras;freelook_camera";
  info->description = "A camera that has a position and a rotation\n\
axis set by the user.\nUseful for static cameras.";
  info->in_param_spec =
"\
render_in:render,\
camera:complex{\
  position:float3,\
  rotation:float3,\
  upvector:float3,\
  fov:float,\
  near_clipping:float,\
  far_clipping:float,\
}\
";
  info->out_param_spec = "render_out:render";
  info->component_class = "render";
  info->tunnel = true;
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  position = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"position");
  rotation = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"rotation");
  upvector = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"upvector");
  fov = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"fov");
  near_clipping = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"near_clipping");
  far_clipping = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"far_clipping");

  position->set(0,0);
  position->set(0,1);
  position->set(1,2);

  rotation->set(0,0);
  rotation->set(-1,1);
  rotation->set(0,2);
  upvector->set(0,0);
  upvector->set(1,1);
  upvector->set(0,2);

  fov->set(90.0f);
  near_clipping->set(0.01f);
  far_clipping->set(2000.0f);

  render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
  render_in->run_activate_offscreen = true;

  render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");

  gl_state = vsx_gl_state::get_instance();
}

bool activate_offscreen()
{
  gl_state->matrix_get_v( VSX_GL_PROJECTION_MATRIX, matrix_projection );
  gl_state->matrix_get_v( VSX_GL_MODELVIEW_MATRIX, matrix_modelview);

  gl_state->matrix_glu_perspective(fov->get(), 1.0, fabs(near_clipping->get()), far_clipping->get());
  gl_state->matrix_mode( VSX_GL_MODELVIEW_MATRIX );
  gl_state->matrix_glu_lookat(
	  position->get(0),
	  position->get(1),
	  position->get(2),

	  rotation->get(0)+position->get(0),
	  rotation->get(1)+position->get(1),
	  rotation->get(2)+position->get(2),

	  upvector->get(0),
	  upvector->get(1),
	  upvector->get(2)
	);
  gl_state->matrix_mode( VSX_GL_MODELVIEW_MATRIX );
  return true;
}

void deactivate_offscreen() {
  // reset the matrix to previous value
  gl_state->matrix_mode( VSX_GL_PROJECTION_MATRIX );
  gl_state->matrix_load_identity();
  gl_state->matrix_mult_f( matrix_projection );

  gl_state->matrix_mode( VSX_GL_MODELVIEW_MATRIX );
  gl_state->matrix_load_identity();
  gl_state->matrix_mult_f( matrix_modelview );
}


void output(vsx_module_param_abs* param) { VSX_UNUSED(param);
  render_result->set(render_in->get());
}

};

