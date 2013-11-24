class module_texture_translate : public vsx_module {
  // in
  vsx_module_param_texture* texture_info_param_in;
  vsx_module_param_float3* translation_vec;
  // out
  vsx_module_param_texture* texture_result;
  // internal
  vsx_texture* texture_out;
  vsx_transform_translate transform;

public:
  module_texture_translate() : transform(0, 0, 0) {}
  void module_info(vsx_module_info* info);
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
  void run();
  void on_delete();
};

void module_texture_translate::module_info(vsx_module_info* info) {

  info->identifier = "texture;modifiers;translate";
#ifndef VSX_NO_CLIENT
  info->in_param_spec = "translation_vector:float3,texture_in:texture";
  info->out_param_spec = "texture_translate_out:texture";
  info->component_class = "texture";
#endif
}

void module_texture_translate::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
  loading_done = true;
	texture_info_param_in = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "texture_in");
	texture_out = new vsx_texture;
	//texture_info_param_in->set(*texture_out);
	translation_vec = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "translation_vector");
	translation_vec->set(0.0f, 0);
	translation_vec->set(0.0f, 1);
	translation_vec->set(0.0f, 2);
//	out_parameter_type_id = VSX_PARAM_ID_TEXTURE;
	texture_result = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture_translate_out");
//	texture_result->set(new vsx_texture_info);
}

void module_texture_translate::run() {
  vsx_texture** texture_info_in = texture_info_param_in->get_addr();
  if (texture_info_in)
  {
    texture_out->valid = (*texture_info_in)->valid;
    // copy texture info
    (*texture_out->texture_info) = (*(*texture_info_in)->texture_info);
  	float x = translation_vec->get(0);
  	float y = translation_vec->get(1);
  	float z = translation_vec->get(2);
  	vsx_transform_obj* prev_transform = (*texture_info_in)->get_transform();
  	transform.set_previous_transform(prev_transform);
  	transform.update(x, y, z);
  	texture_out->set_transform(&transform);
    ((vsx_module_param_texture*)texture_result)->set(texture_out);

  } else texture_result->valid = false;
}

void module_texture_translate::on_delete()
{
  delete texture_out;
}


