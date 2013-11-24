class module_render_gl_orbit_camera : public vsx_module
{
  // in
  GLfloat matrix_projection[16];
  GLfloat matrix_modelview[16];


  vsx_module_param_float3* rotation;
  vsx_module_param_float* distance;
  vsx_module_param_float3 * destination;
  vsx_module_param_float3 * upvector;
  vsx_module_param_float* fov;
  vsx_module_param_float* near_clipping;
  vsx_module_param_float* far_clipping;

  vsx_module_param_int* perspective_correct;

	vsx_module_param_render* render_in;
	// out
	vsx_module_param_render* render_result;
	// internal
	vsx_vector rotation_;

  vsx_gl_state* gl_state;

public:






void module_info(vsx_module_info* info)
{
  info->identifier = "renderers;opengl_modifiers;cameras;orbit_camera";
  info->description = "\
A camera always looking at the\n\
destination, with a distance\n\
and a rotation vector. Useful if you\n\
want to orbit an object like in starlight aurora.";
  info->in_param_spec =
"render_in:render,\
camera:complex{\
rotation:float3,\
distance:float,\
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
  rotation = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"rotation");
  distance = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"distance");
  destination = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"destination");
  upvector = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"upvector");
  fov = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"fov");
  near_clipping = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"near_clipping");
  far_clipping = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"far_clipping");
  perspective_correct = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"perspective_correct");

  rotation->set(0,0);
  rotation->set(0,1);
  rotation->set(1,2);

  distance->set(2);
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
  gl_state->matrix_get_v(VSX_GL_PROJECTION_MATRIX, matrix_projection);
  gl_state->matrix_get_v(VSX_GL_MODELVIEW_MATRIX, matrix_modelview);
  float dist = distance->get();

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

  rotation_.x = rotation->get(0);
  rotation_.y = rotation->get(1);
  rotation_.z = rotation->get(2);
  rotation_.normalize();

  gl_state->matrix_mode( VSX_GL_MODELVIEW_MATRIX );
  gl_state->matrix_load_identity();

#ifdef VSXU_OPENGL_ES
	glRotatef(-90,0,0,1);
#endif
  gl_state->matrix_glu_lookat(
	  rotation_.x*dist+destination->get(0),
	  rotation_.y*dist+destination->get(1),
	  rotation_.z*dist+destination->get(2),

	  destination->get(0),
	  destination->get(1),
	  destination->get(2),

	  upvector->get(0),
	  upvector->get(1),
	  upvector->get(2)
  );
  return true;
}

void deactivate_offscreen() {
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


