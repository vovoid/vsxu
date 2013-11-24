const unsigned int genmodes[] = {
GL_OBJECT_LINEAR,
GL_EYE_LINEAR,
GL_SPHERE_MAP,
GL_NORMAL_MAP,
GL_REFLECTION_MAP
};


class module_render_gl_texture_coord_gen : public vsx_module {
  // in
	vsx_module_param_render* render_in;
	vsx_module_param_int* gen_s;
  vsx_module_param_int* gen_t;
  vsx_module_param_int* gen_r;
  vsx_module_param_float4* parameter;
	// out
	vsx_module_param_render* render_out;
	// internal
public:
  void module_info(vsx_module_info* info) {
    info->identifier = "texture;opengl;texture_coord_gen";
    info->description = "Uses OpenGL to generate texture\ncoordinates on the fly for\nreflection/normal maps.";
    info->in_param_spec = "render_in:render,\
gen_s:enum?OFF|OBJECT_LINEAR|EYE_LINEAR|SPHERE_MAP|NORMAL_MAP|REFLECTION_MAP,\
gen_t:enum?OFF|OBJECT_LINEAR|EYE_LINEAR|SPHERE_MAP|NORMAL_MAP|REFLECTION_MAP,\
gen_r:enum?OFF|OBJECT_LINEAR|EYE_LINEAR|SPHERE_MAP|NORMAL_MAP|REFLECTION_MAP,\
parameter:float4\
";
    info->out_param_spec = "render_out:render";
    info->component_class = "texture";
    loading_done = true;
    info->tunnel = true;
  }

	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
    render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
    gen_s = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"gen_s");
    gen_s->set(0);
    gen_t = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"gen_t");
    gen_t->set(0);
    gen_r = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"gen_r");
    gen_r->set(0);
    parameter = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"parameter");
    parameter->set(0.0f,0);
    parameter->set(1.0f,0);
    parameter->set(0.0f,0);
    parameter->set(0.0f,0);
    render_out = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_in->set(0);
    render_in->run_activate_offscreen = true;
    render_out->set(0);
  }

	bool activate_offscreen() {
    if (gen_s->get()) {
      glEnable(GL_TEXTURE_GEN_S);
      int mode = genmodes[gen_s->get()-1];
      glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, mode);
    }
    if (gen_t->get()) {
      glEnable(GL_TEXTURE_GEN_T);
      int mode = genmodes[gen_t->get()-1];
      if (mode != GL_SPHERE_MAP)
      glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, mode);
    }
    if (gen_r->get())
    {
      glEnable(GL_TEXTURE_GEN_R);
      int mode = genmodes[gen_r->get()-1];

      if (mode != GL_SPHERE_MAP)
        glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, mode);
    }
    return true;
  }

	void deactivate_offscreen() {
    if (gen_s->get()) glDisable(GL_TEXTURE_GEN_S);
    if (gen_t->get()) glDisable(GL_TEXTURE_GEN_T);
    if (gen_r->get()) glDisable(GL_TEXTURE_GEN_R);
  }
};

