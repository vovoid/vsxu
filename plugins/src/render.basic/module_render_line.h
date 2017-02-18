class module_render_line : public vsx_module
{
  // in
  vsx_module_param_float3* point_a;
  vsx_module_param_float3* point_b;
  vsx_module_param_float4* color_a;
  vsx_module_param_float4* color_b;
  vsx_module_param_float* width;

  // out
  vsx_module_param_render* render_result;

  // internal

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "renderers;basic;render_line";

    info->description =
      "Renders a line\n"
      "with width and start/stop color."
    ;

    info->in_param_spec =
    "spatial:complex"
    "{"
      "point_a:float3,"
      "point_b:float3,"
      "color_a:float4?default_controller=controller_col,"
      "color_b:float4?default_controller=controller_col,"
      "width:float"
    "}"
    ;

    info->out_param_spec = "render_out:render";
    info->component_class = "render";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    point_a = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "point_a");
    point_a->set(0.0f, 0);
    point_a->set(0.0f, 1);
    point_a->set(0.0f, 2);

    point_b = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "point_b");
    point_b->set(1.0f, 0);
    point_b->set(0.0f, 1);
    point_b->set(0.0f, 2);

    color_a = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "color_a");
    color_a->set(1.0f,0);
    color_a->set(1.0f,1);
    color_a->set(1.0f,2);
    color_a->set(1.0f,3);

    color_b = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "color_b");
    color_b->set(1.0f,0);
    color_b->set(1.0f,1);
    color_b->set(1.0f,2);
    color_b->set(0.0f,3);

    width = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "width");
    width->set(1.0f);

    render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_result->set(0);
  }

  void output(vsx_module_param_abs* param)
  {
    VSX_UNUSED(param);
      glBegin(GL_LINES);
        glColor4f(color_a->get(0),color_a->get(1),color_a->get(2),color_a->get(3));
        glVertex3f(point_a->get(0), point_a->get(1), point_a->get(2));

        glColor4f(color_b->get(0),color_b->get(1),color_b->get(2),color_b->get(3));
        glVertex3f(point_b->get(0), point_b->get(1), point_b->get(2));
      glEnd();
    render_result->set(1);
    loading_done = true;
  }
};
