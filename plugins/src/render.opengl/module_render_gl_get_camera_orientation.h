#include <vsx_gl_state.h>

class module_render_gl_get_camera_orientation : public vsx_module
{
  // in
  vsx_module_param_render* render_in;

  // out
  vsx_module_param_render* render_out;
  vsx_module_param_float3* direction_out;

  // internal
  vsx_matrix<float> matrix;

  vsx_gl_state* gl_state;

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "renderers;opengl_modifiers;gl_get_camera_orientation";

    info->description =
      "Gets the angle the camera is looking\n"
      "from the modelview matrix."
    ;

    info->in_param_spec =
      "render_in:render"
    ;

    info->out_param_spec =
      "render_out:render,"
      "direction_out:float3"
    ;

    info->component_class =
      "render";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
    render_in->set(0);

    direction_out = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"direction_out");

    render_out = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_out->set(1);

    gl_state = vsx_gl_state::get_instance();
  }

  void output(vsx_module_param_abs* param)
  {
    if (param != render_out)
      return;

    gl_state->matrix_get_v( VSX_GL_MODELVIEW_MATRIX, matrix.m);

    direction_out->set( matrix.m[2], 0 );
    direction_out->set( matrix.m[6], 1 );
    direction_out->set( matrix.m[10], 2 );

    render_out->set(1);
  }
};


