#include <texture/vsx_texture.h>

class module_texture_rotate : public vsx_module
{
  // in
  vsx_module_param_texture* texture_info_param_in;
  vsx_module_param_float3* rotation_axis;
  vsx_module_param_float* rotation_angle;

  // out
  vsx_module_param_texture* texture_result;

  // internal
  vsx_texture<>* texture_out = 0x0;
  vsx_texture_transform_rotate transform;

public:
  module_texture_rotate() : transform(0, 0, 0, 1)
  {
  }

  void module_info(vsx_module_specification* info);
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
  void run();
  void on_delete();
};

void module_texture_rotate::module_info(vsx_module_specification* info)
{
  info->identifier =
    "texture;modifiers;rotate";

  info->in_param_spec =
    "texture_in:texture,"
    "rotation_angle:float,"
    "rotation_axis:float3"
  ;

  info->out_param_spec =
    "texture_rotate_out:texture";

  info->component_class =
    "texture";
}

void module_texture_rotate::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  texture_info_param_in = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "texture_in");
  loading_done = true;

  rotation_axis = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "rotation_axis");
  rotation_axis->set(0, 0);
  rotation_axis->set(0, 1);
  rotation_axis->set(1, 2);

  rotation_angle = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "rotation_angle");
  rotation_angle->set(0.0f);

  texture_result = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture_rotate_out");
}

void module_texture_rotate::run()
{
  vsx_texture<>** texture_in = texture_info_param_in->get_addr();

  if (!texture_in)
  {
    texture_result->valid = false;
    return;
  }

  if (!texture_out)
  {
    texture_out = new vsx_texture<>();
    delete texture_out->texture;
  }

  // copy texture info
  texture_out->texture = (*texture_in)->texture;

  float x = rotation_axis->get(0);
  float y = rotation_axis->get(1);
  float z = rotation_axis->get(2);
  float a = rotation_angle->get()*360;

  vsx_texture_transform_base* prev_transform = (*texture_in)->get_transform();
  transform.set_previous_transform(prev_transform);
  transform.update(a, x, y, z);
  texture_out->set_transform(&transform);
  ((vsx_module_param_texture*)texture_result)->set(texture_out);
}

void module_texture_rotate::on_delete()
{
  if (texture_out)
  {
    texture_out->texture = 0x0;
    delete texture_out;
  }
}
