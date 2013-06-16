class module_float3_accumulator : public vsx_module
{
  // in
  vsx_module_param_float3* float3_in;
  vsx_module_param_int* reset;

  // out
  vsx_module_param_float3* result_float3;

  // internal
  float value[3];

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;accumulators;float3_accumulator";
    info->description = "accumulator - float3\n"
                        "result += in\n"
                        "\n"
                        "adds the value in float3_in\n"
                        "to its current value once\n"
                        "per frame";
    info->out_param_spec = "result_float3:float3";
    info->in_param_spec = "float3_in:float3,reset:enum?ok";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    value[0] = 0;
    value[1] = 0;
    value[2] = 0;

    //--------------------------------------------------------------------------------------------------

    float3_in = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"float3_in");

    reset = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"reset");
    reset->set(-1);

    //--------------------------------------------------------------------------------------------------

    result_float3 = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"result_float3");
    result_float3->set(0,0);
    result_float3->set(0,1);
    result_float3->set(0,2);
  }

  void run()
  {
    if (reset->get() == 0)
    {
      reset->set(-1);
      value[0] = 0;
      value[1] = 0;
      value[2] = 0;
    }
    value[0] += float3_in->get(0);
    value[1] += float3_in->get(1);
    value[2] += float3_in->get(2);
    result_float3->set(value[0],0);
    result_float3->set(value[1],1);
    result_float3->set(value[2],2);
  }
};
