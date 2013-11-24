class module_texture_rotate : public vsx_module 
{
  // in
  vsx_module_param_texture* texture_info_param_in;
  vsx_module_param_float3* rotation_axis;
  vsx_module_param_float* rotation_angle;
  // out
  vsx_module_param_texture* texture_result;
  // internal
  vsx_texture* texture_out;
  vsx_transform_rotate transform;

public:
  module_texture_rotate() : transform(0, 0, 0, 1) {}
  void module_info(vsx_module_info* info);
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
  void run();
  void on_delete();
};

void module_texture_rotate::module_info(vsx_module_info* info) {
  info->identifier = "texture;modifiers;rotate";
#ifndef VSX_NO_CLIENT
  info->in_param_spec = "texture_in:texture,rotation_angle:float,rotation_axis:float3";
  info->out_param_spec = "texture_rotate_out:texture";
  info->component_class = "texture";
#endif
}

void module_texture_rotate::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
	texture_info_param_in = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "texture_in");
//	texture_info_param_in->set(new vsx_texture_info);
  loading_done = true;
	texture_out = new vsx_texture;

	rotation_axis = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "rotation_axis");
	rotation_axis->set(0, 0);
	rotation_axis->set(0, 1);
	rotation_axis->set(1, 2);
	rotation_angle = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "rotation_angle");
	rotation_angle->set(0.0f);
//	out_parameter_type_id = VSX_PARAM_ID_TEXTURE;
	texture_result = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture_rotate_out");
//	texture_result->set(new vsx_texture_info);
}

void module_texture_rotate::run() {

	vsx_texture** texture_info_in = texture_info_param_in->get_addr();
  if (texture_info_in)
  {

    texture_out->valid = (*texture_info_in)->valid;

    (*texture_out->texture_info) = (*(*texture_info_in)->texture_info);

  	float x = rotation_axis->get(0);
  	float y = rotation_axis->get(1);
  	float z = rotation_axis->get(2);
  	float a = rotation_angle->get()*360;
  	vsx_transform_obj* prev_transform = (*texture_info_in)->get_transform();
  	transform.set_previous_transform(prev_transform);
  	transform.update(a, x, y, z);
  	texture_out->set_transform(&transform);
  	((vsx_module_param_texture*)texture_result)->set(texture_out);
  }
  else
  {
    texture_result->valid = false;
  }
}

void module_texture_rotate::on_delete()
{
  delete texture_out;
}
