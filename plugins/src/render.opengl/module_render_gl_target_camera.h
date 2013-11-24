class module_render_gl_target_camera : public vsx_module
{
  GLfloat matrix_projection[16];
  GLfloat matrix_modelview[16];
  // in
  vsx_module_param_float3* position;
  vsx_module_param_float3* destination;
  vsx_module_param_float3* upvector;
  vsx_module_param_float* fov;
  vsx_module_param_float* near_clipping;
  vsx_module_param_float* far_clipping;
  vsx_module_param_int* perspective_correct;
  vsx_module_param_render* render_in;

  // out
  vsx_module_param_render* render_result;

  // internal
  vsx_gl_state* gl_state;

public:

void module_info(vsx_module_info* info)
{
  info->identifier = "renderers;opengl_modifiers;cameras;target_camera";
  info->description = "A camera that looks from one \nposition to a destination.\n\
Useful if you want to follow an object\n\
with the camera.";
  info->in_param_spec =
"\
render_in:render,\
camera:complex{\
  position:float3,\
  destination:float3,\
  upvector:float3,\
  fov:float,\
  near_clipping:float,\
  far_clipping:float,\
  perspective_correct:enum?no|yes\
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
  destination = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"destination");
  upvector = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"upvector");
  fov = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"fov");
  near_clipping = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"near_clipping");
  far_clipping = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"far_clipping");
  perspective_correct = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"perspective_correct");

  position->set(0,0);
  position->set(0,1);
  position->set(1,2);

  destination->set(0,0);
  destination->set(0,1);
  destination->set(0,2);
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

  if (perspective_correct->get())
  {
    gl_state->matrix_glu_perspective(
      fov->get(),
      (float)gl_state->viewport_get_width()/(float)gl_state->viewport_get_height(),
      fabs(near_clipping->get()),
      far_clipping->get()
    );
  }
  else
  {
    gl_state->matrix_glu_perspective(
      fov->get(),
      1.0,
      fabs(near_clipping->get()),
      far_clipping->get()
    );
  }
  gl_state->matrix_mode( VSX_GL_MODELVIEW_MATRIX );

  gl_state->matrix_glu_lookat
  (
	  position->get(0),
	  position->get(1),
	  position->get(2),

	  destination->get(0),
	  destination->get(1),
	  destination->get(2),

	  upvector->get(0),
	  upvector->get(1),
	  upvector->get(2)
	);

  return true;
}

void deactivate_offscreen()
{
  // reset the matrix to previous value
  gl_state->matrix_mode(VSX_GL_PROJECTION_MATRIX);
  gl_state->matrix_load_identity();
  gl_state->matrix_mult_f(matrix_projection);

  gl_state->matrix_mode(VSX_GL_MODELVIEW_MATRIX);
  gl_state->matrix_load_identity();
  gl_state->matrix_mult_f(matrix_modelview);
}

void output(vsx_module_param_abs* param) { VSX_UNUSED(param);
  render_result->set(render_in->get());
}

};

