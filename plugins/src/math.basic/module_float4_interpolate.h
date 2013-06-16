class module_float4_interpolate : public vsx_module
{
  // in
  vsx_module_param_float4* float4_in_a;
  vsx_module_param_float4* float4_in_b;
  vsx_module_param_float* pos;

  // out
  vsx_module_param_float4* result_float4;

  // internal

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;interpolation;float4_interpolate||maths;color;color_linear_interpolate";
    info->description = "linear interpolation for the value\npos ranges from 0.0 to 1.0";
    info->in_param_spec = "float4_in_a:float4,float4_in_b:float4,pos:float";
    info->out_param_spec = "result_float4:float4";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    float4_in_a = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"float4_in_a");
    float4_in_b = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"float4_in_b");

    pos = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"pos");

    //--------------------------------------------------------------------------------------------------

    result_float4 = (vsx_module_param_float4*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"result_float4");
    result_float4->set(0.0f, 0);
    result_float4->set(0.0f, 1);
    result_float4->set(0.0f, 2);
    result_float4->set(0.0f, 3);
  }

  void run()
  {
    float p = pos->get();
    if (p < 0.0f) p = 0.0f;
    else // optimization
    if (p > 1.0f) p = 1.0f;
    float p1 = 1.0f - p;
    result_float4->set( float4_in_a->get(0) * p1 + float4_in_b->get(0) * p, 0);
    result_float4->set( float4_in_a->get(1) * p1 + float4_in_b->get(1) * p, 1);
    result_float4->set( float4_in_a->get(2) * p1 + float4_in_b->get(2) * p, 2);
    result_float4->set( float4_in_a->get(3) * p1 + float4_in_b->get(3) * p, 3);
  }
};
