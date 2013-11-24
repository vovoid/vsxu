class module_render_gl_normalize : public vsx_module
{
  // in
  vsx_module_param_render* render_in;
  // out
  vsx_module_param_render* render_out;
  // internal
public:
  void module_info(vsx_module_info* info) {
    info->identifier = "renderers;opengl_modifiers;auto_normalize";
    info->description = "";
    info->in_param_spec = "render_in:render";
    info->out_param_spec = "render_out:render";
    info->component_class = "render";
    loading_done = true;
    info->tunnel = true; // always run this
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
    render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
    render_out = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_in->set(0);
    render_in->run_activate_offscreen = true;
    render_out->set(0);
  }

  bool activate_offscreen() {
    glEnable(GL_NORMALIZE);
    return true;
  }

  void deactivate_offscreen() {
    glDisable(GL_NORMALIZE);
  }
};

