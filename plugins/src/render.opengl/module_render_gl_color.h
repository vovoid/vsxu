class module_render_gl_color : public vsx_module
{
  // in
  vsx_module_param_float4* color;
  vsx_module_param_render* render_in;
  // out
  vsx_module_param_render* render_result;
  // internal
public:

void module_info(vsx_module_info* info)
{
  info->identifier = "renderers;opengl_modifiers;gl_color";
  info->description = "Sets the color for modules\n\
that don't do this themselves.";
  info->in_param_spec =
    "render_in:render,"
    "color:float4?default_controller=controller_col"
  ;
  info->out_param_spec = "render_out:render";
  info->component_class = "render";
  info->tunnel = true; // always run this
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"color");
  color->set(1,0);
  color->set(1,1);
  color->set(1,2);
  color->set(1,3);
  render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
  render_in->run_activate_offscreen = true;

  render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
}

bool activate_offscreen()
{
  // save current matrix
  glColor4f(color->get(0),color->get(1),color->get(2),color->get(3));
  return true;
}

void output(vsx_module_param_abs* param)
{
  VSX_UNUSED(param);
  render_result->set(render_in->get());
}
};

