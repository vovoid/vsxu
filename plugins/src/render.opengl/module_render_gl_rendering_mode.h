unsigned int rendermodes[] =
{
  VSX_GL_POINT,
  VSX_GL_LINE,
  VSX_GL_FILL
};

class module_render_gl_rendering_mode : public vsx_module
{
  // in
  vsx_module_param_render* render_in;
  vsx_module_param_int* front;
  GLint p_mode[2];
  vsx_module_param_int* back;
  vsx_module_param_int* smooth;
  GLboolean p_smooth;

  // out
  vsx_module_param_render* render_result;

  // internal
  vsx_gl_state* gl_state;

public:

void module_info(vsx_module_info* info)
{
  info->identifier = "renderers;opengl_modifiers;rendering_mode";
  info->description = "Changes the way triangles are drawn\n - either as points, lines (wireframe\nor solid (default)";

  info->in_param_spec =
"\
render_in:render,\
back_facing:enum?points|lines|solid,\
front_facing:enum?points|lines|solid,\
smooth_edges:enum?no|yes\
";
  info->out_param_spec = "render_out:render";
  info->component_class = "render";
  info->tunnel = true;
}

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
    render_in->run_activate_offscreen = true;

    front = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"front_facing");
    front->set(2);
    back = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"back_facing");
    back->set(2);
    smooth = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"smooth_edges");
    smooth->set(0);
    render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");

    gl_state = vsx_gl_state::get_instance();
  }

  bool activate_offscreen()
  {
    p_mode[0] = gl_state->polygon_mode_get( VSX_GL_FRONT );
    p_mode[1] = gl_state->polygon_mode_get( VSX_GL_BACK );
    gl_state->polygon_mode_set(VSX_GL_FRONT, rendermodes[ front->get() ] );
    gl_state->polygon_mode_set(VSX_GL_BACK, rendermodes[ back->get() ] );
    return true;
  }

  void deactivate_offscreen()
  {
    gl_state->polygon_mode_set(VSX_GL_FRONT, p_mode[0] );
    gl_state->polygon_mode_set(VSX_GL_BACK, p_mode[1] );
  }

  void output(vsx_module_param_abs* param) { VSX_UNUSED(param);
    render_result->set(render_in->get());
  }
};

