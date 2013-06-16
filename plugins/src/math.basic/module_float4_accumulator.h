class module_float4_accumulator : public vsx_module
{
  // in
  vsx_module_param_float4* float4_in;
  vsx_module_param_int* reset;

  // out
  vsx_module_param_float4* result_float4;

  // internal
  float value[4];

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;accumulators;float4_accumulator";
    info->description = "accumulator - float4\n"
                        "result += in\n"
                        "\n"
                        "adds the value in float4_in\n"
                        "to its current value once\n"
                        "per frame";
    info->out_param_spec = "result_float4:float4";
    info->in_param_spec = "float4_in:float4,reset:enum?ok";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    value[0] = 0;
    value[1] = 0;
    value[2] = 0;
    value[3] = 0;

    //--------------------------------------------------------------------------------------------------

    float4_in = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"float4_in");

    reset = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"reset");
    reset->set(-1);

    //--------------------------------------------------------------------------------------------------

    result_float4 = (vsx_module_param_float4*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"result_float4");
    result_float4->set(0,0);
    result_float4->set(0,1);
    result_float4->set(0,2);
    result_float4->set(0,3);
  }

  void run()
  {
    if (reset->get() == 0)
    {
      reset->set(-1);
      value[0] = 0;
      value[1] = 0;
      value[2] = 0;
      value[3] = 0;
    }

    value[0] += float4_in->get(0);
    value[1] += float4_in->get(1);
    value[2] += float4_in->get(2);
    value[3] += float4_in->get(3);

    result_float4->set(value[0],0);
    result_float4->set(value[1],1);
    result_float4->set(value[2],2);
    result_float4->set(value[3],3);
  }
};


