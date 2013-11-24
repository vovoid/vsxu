class vsx_module_render_gravity_lines : public vsx_module
{
  // in
  vsx_module_param_float3* pos;
  vsx_module_param_float4* color0;
  vsx_module_param_float4* color1;

  vsx_module_param_float* friction;
  vsx_module_param_float* step_length;

  // out
  vsx_module_param_render* render_result;

  // internal
  gravity_lines gr;
  float last_updated;

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "renderers;vovoid;gravity_lines";

    info->in_param_spec = "pos:float3,"
                          "params:complex"
                          "{"
                            "friction:float,"
                            "step_length:float,"
                            "color0:float4?default_controller=controller_col,"
                            "color1:float4?default_controller=controller_col"
                          "}";

    info->out_param_spec = "render_out:render";
    info->component_class = "render";
  }


  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    last_updated = -1.0f;
    loading_done = true;
    pos = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "pos");
    pos->set(0,0);
    pos->set(0,1);
    pos->set(0,2);

    color0 = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "color0");
    color0->set(1.0f,0);
    color0->set(1.0f,1);
    color0->set(1.0f,2);
    color0->set(0.3f,3);

    color1 = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "color1");
    color1->set(1.0f,0);
    color1->set(1.0f,1);
    color1->set(1.0f,2);
    color1->set(1.0f,3);

    // parameters for the effect
    friction = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "friction");
    friction->set(1);

    step_length = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "step_length");
    step_length->set(10);

    render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_result->set(0);
    gr.init();
  }

  void output(vsx_module_param_abs* param)
  {
    VSX_UNUSED(param);
    gr.friction = friction->get();
    gr.color0[0] = color0->get(0);
    gr.color0[1] = color0->get(1);
    gr.color0[2] = color0->get(2);
    gr.color0[3] = color0->get(3);

    gr.color1[0] = color1->get(0);
    gr.color1[1] = color1->get(1);
    gr.color1[2] = color1->get(2);
    gr.step_freq = 10.0f * step_length->get();
    if (last_updated != engine->vtime)
    {
      gr.update(engine->dtime, pos->get(0), pos->get(1), pos->get(2));
      last_updated = engine->vtime;
    }
    gr.render();
    render_result->set(1);
  }
};
