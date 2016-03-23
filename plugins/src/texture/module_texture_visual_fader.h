class module_texture_visual_fader : public vsx_module
{
  // in
  vsx_module_param_texture* texture_a_in;
  vsx_module_param_texture* texture_b_in;
  vsx_module_param_float* fade_pos_in;
  vsx_module_param_float* fade_pos_from_engine;

  // out
  vsx_module_param_texture* texture_a_out;
  vsx_module_param_texture* texture_b_out;
  vsx_module_param_float* fade_pos_out;

  // internal

public:

  void module_info(vsx_module_specification* info) {
    info->identifier = "system;visual_fader";
    info->in_param_spec = "texture_a_in:texture,texture_b_in:texture,fade_pos_in:float";
    info->out_param_spec = "texture_a_out:texture,texture_b_out:texture,fade_pos_out:float";
    info->component_class = "system";
    loading_done = true;
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
    texture_a_in = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "texture_a_in");
    texture_b_in = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "texture_b_in");

    fade_pos_from_engine = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "fade_pos_from_engine");
    fade_pos_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "fade_pos_in");
    fade_pos_out = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "fade_pos_out");
    fade_pos_out->set(0.0f);

    texture_a_out = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "texture_a_out");
    texture_b_out = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "texture_b_out");
  }

  void output(vsx_module_param_abs* param)
  {
    VSX_UNUSED(param);
    vsx_texture<>** t_a;
      t_a = texture_a_in->get_addr();
    vsx_texture<>** t_b;
      t_b = texture_b_in->get_addr();
    if (t_a && t_b) {
      ((vsx_module_param_texture*)texture_a_out)->set(*t_a);
      ((vsx_module_param_texture*)texture_b_out)->set(*t_b);
    }
    if (fade_pos_from_engine->get() != 0.0f)
    {
      fade_pos_out->set(fade_pos_in->get());
    } else
    {
      fade_pos_out->set((float)fmod(engine_state->real_vtime, 1.0f));
    }
  }
};

