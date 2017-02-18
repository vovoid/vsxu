class module_texture_dummy: public vsx_module
{
  // in
  vsx_module_param_texture* texture_in;

  // out
  vsx_module_param_texture* texture_out;

  // internal

public:

  module_texture_dummy()
  {
  }

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "texture;dummies;texture_dummy"
      "||"
      "dummies;texture_dummy"
    ;

    info->in_param_spec =
      "texture_in:texture";

    info->out_param_spec =
      "texture_out:texture";

    info->component_class =
      "texture";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    texture_in = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "texture_in");
    loading_done = true;

    texture_out = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture_out");
  }


  void run()
  {
    vsx_texture<>** texture_info_in = texture_in->get_addr();

    if (!texture_info_in)
      return;

    texture_out->set( *texture_info_in );
  }

};

