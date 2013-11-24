class module_render_gl_material_param : public vsx_module
{
  // in
	vsx_module_param_render* render_in;
	vsx_module_param_int* faces_affected;

	vsx_module_param_float4  *ambient_reflectance,
                           *diffuse_reflectance,
                           *specular_reflectance,
                           *emission_intensity;
  vsx_module_param_float *specular_exponent;
	// out
	vsx_module_param_render* render_result;
	// internal
  GLfloat ambient_front[4];
  GLfloat diffuse_front[4];
  GLfloat specular_front[4];
  GLfloat emission_front[4];
  GLfloat spec_exp_front[4];

  GLfloat ambient_back[4];
  GLfloat diffuse_back[4];
  GLfloat specular_back[4];
  GLfloat emission_back[4];
  GLfloat spec_exp_back[4];

  vsx_gl_state* gl_state;

public:

	void module_info(vsx_module_info* info)
	{
	  info->identifier = "renderers;opengl_modifiers;material_param";
	  info->description = "Modifies the way triangles process\n"
												"light from a lightsource.";
	  info->in_param_spec =	"render_in:render,"
													"faces_affected:enum?front_facing|back_facing|front_and_back,"
													"properties:complex{"
                            "ambient_reflectance:float4?default_controller=controller_col,"
                            "diffuse_reflectance:float4?default_controller=controller_col,"
                            "specular_reflectance:float4?default_controller=controller_col,"
                            "emission_intensity:float4?default_controller=controller_col,"
                            "specular_exponent:float?max=128&min=0"
													"}";
	  info->out_param_spec = "render_out:render";
	  info->component_class = "render";
	  info->tunnel = true;
	}

	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
	{
	  loading_done = true;
    // initialize memory to make valgrind happy
    ambient_front[0] = 0.0f;
    ambient_front[1] = 0.0f;
    ambient_front[2] = 0.0f;
    ambient_front[3] = 0.0f;

    diffuse_front[0] = 0.0f;
    diffuse_front[1] = 0.0f;
    diffuse_front[2] = 0.0f;
    diffuse_front[3] = 0.0f;

    specular_front[0] = 0.0f;
    specular_front[1] = 0.0f;
    specular_front[2] = 0.0f;
    specular_front[3] = 0.0f;

    emission_front[0] = 0.0f;
    emission_front[1] = 0.0f;
    emission_front[2] = 0.0f;
    emission_front[3] = 0.0f;

    spec_exp_front[0] = 0.0f;

    ambient_back[0] = 0.0f;
    ambient_back[1] = 0.0f;
    ambient_back[2] = 0.0f;
    ambient_back[3] = 0.0f;

    diffuse_back[0] = 0.0f;
    diffuse_back[1] = 0.0f;
    diffuse_back[2] = 0.0f;
    diffuse_back[3] = 0.0f;

    specular_back[0] = 0.0f;
    specular_back[1] = 0.0f;
    specular_back[2] = 0.0f;
    specular_back[3] = 0.0f;

    emission_back[0] = 0.0f;
    emission_back[1] = 0.0f;
    emission_back[2] = 0.0f;
    emission_back[3] = 0.0f;

    spec_exp_back[0] = 0.0f;

		render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
    render_in->run_activate_offscreen = true;

	  faces_affected = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"faces_affected");
	  faces_affected->set(2);
	  ambient_reflectance = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"ambient_reflectance");
	  ambient_reflectance->set(0.2f,0);
	  ambient_reflectance->set(0.2f,1);
	  ambient_reflectance->set(0.2f,2);
	  ambient_reflectance->set(1.0f,3);

	  diffuse_reflectance = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"diffuse_reflectance");
	  diffuse_reflectance->set(0.8f,0);
	  diffuse_reflectance->set(0.8f,1);
	  diffuse_reflectance->set(0.8f,2);
	  diffuse_reflectance->set(1.0f,3);

	  specular_reflectance = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"specular_reflectance");
	  specular_reflectance->set(0.0f,0);
	  specular_reflectance->set(0.0f,1);
	  specular_reflectance->set(0.0f,2);
	  specular_reflectance->set(1.0f,3);

	  emission_intensity = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"emission_intensity");
	  emission_intensity->set(0.0f,0);
	  emission_intensity->set(0.0f,1);
	  emission_intensity->set(0.0f,2);
	  emission_intensity->set(1.0f,3);

	  specular_exponent = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"specular_exponent");
    specular_exponent->set(0.0f);

		render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");

    gl_state = vsx_gl_state::get_instance();
	}


  bool activate_offscreen()
  {
    unsigned int ff = faces_affected->get();

    float specular[4];
    specular[0] = specular_exponent->get();
    if (specular[0] > 120.0f)
      specular[0] = 120.0f;

    if (specular[0] < 0.0f)
      specular[0] = 0.0f;

    if (ff == 0 || ff == 2)
    {
      gl_state->material_get_fv( VSX_GL_FRONT, VSX_GL_AMBIENT, &ambient_front[0] );
      gl_state->material_get_fv( VSX_GL_FRONT, VSX_GL_DIFFUSE, &diffuse_front[0] );
      gl_state->material_get_fv( VSX_GL_FRONT, VSX_GL_SPECULAR, &specular_front[0] );
      gl_state->material_get_fv( VSX_GL_FRONT, VSX_GL_EMISSION, &emission_front[0] );
      gl_state->material_get_fv( VSX_GL_FRONT, VSX_GL_SHININESS, &spec_exp_front[0] );

      gl_state->material_set_fv( VSX_GL_FRONT, VSX_GL_AMBIENT, ambient_reflectance->get_addr() );
      gl_state->material_set_fv( VSX_GL_FRONT, VSX_GL_DIFFUSE, diffuse_reflectance->get_addr() );
      gl_state->material_set_fv( VSX_GL_FRONT, VSX_GL_SPECULAR, specular_reflectance->get_addr() );
      gl_state->material_set_fv( VSX_GL_FRONT, VSX_GL_EMISSION, emission_intensity->get_addr() );
      gl_state->material_set_fv( VSX_GL_FRONT, VSX_GL_SHININESS, specular );

    }
    if (ff == 1 || ff == 2)
    {
      gl_state->material_get_fv( VSX_GL_BACK, VSX_GL_AMBIENT, &ambient_back[0] );
      gl_state->material_get_fv( VSX_GL_BACK, VSX_GL_DIFFUSE, &diffuse_back[0] );
      gl_state->material_get_fv( VSX_GL_BACK, VSX_GL_SPECULAR, &specular_back[0] );
      gl_state->material_get_fv( VSX_GL_BACK, VSX_GL_EMISSION, &emission_back[0] );
      gl_state->material_get_fv( VSX_GL_BACK, VSX_GL_SHININESS, &spec_exp_back[0] );

      gl_state->material_set_fv( VSX_GL_BACK, VSX_GL_AMBIENT, ambient_reflectance->get_addr() );
      gl_state->material_set_fv( VSX_GL_BACK, VSX_GL_DIFFUSE, diffuse_reflectance->get_addr() );
      gl_state->material_set_fv( VSX_GL_BACK, VSX_GL_SPECULAR, specular_reflectance->get_addr() );
      gl_state->material_set_fv( VSX_GL_BACK, VSX_GL_EMISSION, emission_intensity->get_addr() );
      gl_state->material_set_fv( VSX_GL_BACK, VSX_GL_SHININESS, specular );
    }

	  return true;
	}

  void deactivate_offscreen()
  {
    unsigned int ff = faces_affected->get();
    if (ff == 0 || ff == 2)
    {
      gl_state->material_set_fv( VSX_GL_FRONT, VSX_GL_AMBIENT, &ambient_front[0] );
      gl_state->material_set_fv( VSX_GL_FRONT, VSX_GL_DIFFUSE, &diffuse_front[0] );
      gl_state->material_set_fv( VSX_GL_FRONT, VSX_GL_SPECULAR, &specular_front[0] );
      gl_state->material_set_fv( VSX_GL_FRONT, VSX_GL_EMISSION, &emission_front[0] );
      gl_state->material_set_fv( VSX_GL_FRONT, VSX_GL_SHININESS, &spec_exp_front[0] );
    }
    if (ff == 1 || ff == 2)
    {      
      gl_state->material_set_fv( VSX_GL_BACK, VSX_GL_AMBIENT, &ambient_back[0] );
      gl_state->material_set_fv( VSX_GL_BACK, VSX_GL_DIFFUSE, &diffuse_back[0] );
      gl_state->material_set_fv( VSX_GL_BACK, VSX_GL_SPECULAR, &specular_back[0] );
      gl_state->material_set_fv( VSX_GL_BACK, VSX_GL_EMISSION, &emission_back[0] );
      gl_state->material_set_fv( VSX_GL_BACK, VSX_GL_SHININESS, &spec_exp_back[0] );
    }
  }

  void output(vsx_module_param_abs* param)
  {
    VSX_UNUSED(param);
	  render_result->set(render_in->get());
	}

};


