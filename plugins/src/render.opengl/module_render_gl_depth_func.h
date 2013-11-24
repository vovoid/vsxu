class module_render_gl_depth_func : public vsx_module 
{
  // in
  vsx_module_param_render* render_in;
  vsx_module_param_int* depth_func;
  // out
  vsx_module_param_render* render_result;
  // internal
  vsx_gl_state* gl_state;
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "renderers;opengl_modifiers;depth_function";
    info->description = "";

    info->in_param_spec =
        "render_in:render,"
        "depth_func:enum?NEVER|LESS|EQUAL|LESS_OR_EQUAL|GREATER|NOT_EQUAL|GREATER_OR_EQUAL|ALWAYS"
    ;
    info->out_param_spec = "render_out:render";
    info->component_class = "render";
    info->tunnel = true;
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    depth_func = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"depth_func");
    depth_func->set( 1 );
    render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
    render_in->run_activate_offscreen = true;
    render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");

    gl_state = vsx_gl_state::get_instance();
  }
  int old_depth_func;
  bool activate_offscreen()
  {
    old_depth_func = gl_state->depth_function_get();
    gl_state->depth_function_set( depth_func->get() );
    return true;
  }

  void deactivate_offscreen()
  {
    gl_state->depth_function_set( old_depth_func );
  }

  void output(vsx_module_param_abs* param)
  {
    VSX_UNUSED(param);
    render_result->set(render_in->get());
  }
};


