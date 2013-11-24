class module_render_gl_buffer_clear : public vsx_module
{
  // in
  vsx_module_param_render* render_in;
  vsx_module_param_int* clear_color_buffer;
  vsx_module_param_int* clear_depth_buffer;
  vsx_module_param_float4* clear_color;
  // out
  vsx_module_param_render* render_result;
  // internal
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "renderers;opengl_modifiers;buffer_clear";
    info->description = "";

    info->in_param_spec =
      "render_in:render,"
      "color_buffer:enum?no|yes,"
      "clear_color:float4?default_controller=controller_col,"
      "depth_buffer:enum?no|yes"
    ;
    info->out_param_spec = "render_out:render";
    info->component_class = "render";
    info->tunnel = true;
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
    render_in->run_activate_offscreen = true;
    clear_color_buffer = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"color_buffer");
    clear_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"clear_color");
    clear_depth_buffer = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"depth_buffer");
    render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
  }

  bool activate_offscreen()
  {
    glClearColor
    (
      clear_color->get(0),
      clear_color->get(1),
      clear_color->get(2),
      clear_color->get(3)
    );
    glClear(
          GL_DEPTH_BUFFER_BIT * clear_depth_buffer->get()
          |
          GL_COLOR_BUFFER_BIT * clear_color_buffer->get()
    );
    return true;
  }

  void deactivate_offscreen() {
  }

  void output(vsx_module_param_abs* param) { VSX_UNUSED(param);
    render_result->set(render_in->get());
  }
};

