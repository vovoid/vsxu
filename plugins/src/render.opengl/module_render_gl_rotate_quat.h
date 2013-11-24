class module_render_gl_rotate_quat : public vsx_module
{
  // in
	GLfloat tmpMat[16];

  vsx_module_param_quaternion* rotation;
  vsx_module_param_int* matrix_target_l;
  vsx_module_param_int* invert_rotation;

	vsx_module_param_render* render_in;
	// out
	vsx_module_param_render* render_result;

  // internal
  vsx_gl_state* gl_state;

public:


  void module_info(vsx_module_info* info)
  {
    info->identifier = "renderers;opengl_modifiers;gl_rotate_quat";
    info->description = "Multiplies the current modelview\n"
      "matrix with a rotation defined \n"
      "by a quaternion. \n"
      "I.e. rotates anything connected \n"
      "via it."
    ;
    info->in_param_spec =
      "render_in:render,"
      "rotation:quaternion,"
      "matrix_target:enum?MODELVIEW|PROJECTION|TEXTURE,"
      "invert_rotation:enum?NO|YES"
    ;
    info->out_param_spec = "render_out:render";
    info->component_class = "render";
    info->tunnel = true; // always run this
  }


  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    rotation = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"rotation");
    rotation->set(0,0);
    rotation->set(0,1);
    rotation->set(0,2);
    rotation->set(1,3);
    matrix_target_l = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"matrix_target");

    invert_rotation = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"invert_rotation");
    invert_rotation->set(0);

  	render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
    render_in->run_activate_offscreen = true;

  	render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");

    gl_state = vsx_gl_state::get_instance();

  }
  vsx_quaternion bb;

  bool activate_offscreen() {
    // save current matrix
    gl_state->matrix_get_v(matrix_target_get_vsx[matrix_target_l->get()],tmpMat);
    gl_state->matrix_mode(matrix_target_get_vsx[matrix_target_l->get()]);

  	bb.x = rotation->get(0);
  	bb.y = rotation->get(1);
  	bb.z = rotation->get(2);
  	bb.w = rotation->get(3);
    bb.normalize();

  	vsx_matrix mat;
    if (invert_rotation->get())
    {
      vsx_matrix mat2;
      mat2 = bb.matrix();
      mat.assign_inverse( &mat2 );
    } else
    {
      mat = bb.matrix();
    }
    gl_state->matrix_mult_f(mat.m);
//  	glMultMatrixf(mat.m);
    return true;
  }

  void deactivate_offscreen() {
    // reset the matrix to previous value
    gl_state->matrix_mode(matrix_target_get_vsx[matrix_target_l->get()]);
    gl_state->matrix_load_identity();
    gl_state->matrix_mult_f(tmpMat);
  }

  void output(vsx_module_param_abs* param) { VSX_UNUSED(param);
    render_result->set(render_in->get());
  }
};

