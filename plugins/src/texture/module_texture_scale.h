class module_texture_scale : public vsx_module
{
  // in
  vsx_module_param_texture* texture_info_param_in;
  vsx_module_param_float3* scale_vec;

  // out
  vsx_module_param_texture* texture_result;

  // internal
  vsx_texture* texture_out;
  vsx_transform_scale transform;

public:
  module_texture_scale() : transform(1, 1, 1) {}
  void module_info(vsx_module_info* info);
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
  void run();
  void on_delete();
};


void module_texture_scale::module_info(vsx_module_info* info)
{

  info->identifier = "texture;modifiers;scale";
  info->in_param_spec = "scale_vector:float3,texture_in:texture";
  info->out_param_spec = "texture_scale_out:texture";
  info->component_class = "texture";
}

void module_texture_scale::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;

  texture_info_param_in = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "texture_in");
  texture_out = new vsx_texture;

  scale_vec = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "scale_vector");
  scale_vec->set(1.0f, 0);
  scale_vec->set(1.0f, 1);
  scale_vec->set(1.0f, 2);
  texture_result = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture_scale_out");
}

void module_texture_scale::run()
{
  vsx_texture** texture_info_in = texture_info_param_in->get_addr();
  if (!texture_info_in)
  {
    texture_result->valid = false;
    return;
  }

  texture_out->valid = (*texture_info_in)->valid;
  (*texture_out->texture_info) = (*(*texture_info_in)->texture_info);
  float x = scale_vec->get(0);
  float y = scale_vec->get(1);
  float z = scale_vec->get(2);
  vsx_transform_obj* prev_transform = (*texture_info_in)->get_transform();
  transform.set_previous_transform(prev_transform);
  transform.update(x, y, z);
  texture_out->set_transform(&transform);
  ((vsx_module_param_texture*)texture_result)->set(texture_out);
}

void module_texture_scale::on_delete()
{
  delete texture_out;
}


