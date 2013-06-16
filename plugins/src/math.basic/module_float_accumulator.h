class module_float_accumulator : public vsx_module
{
  // in
  vsx_module_param_float* float_in;
  vsx_module_param_int* reset;

  // out
  vsx_module_param_float* result_float;

  // internal
  float value;

public:


  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;accumulators;float_accumulator";
    info->description = "accumulator - float\n"
                        "result += in\n"
                        "\n"
                        "adds the value in float_in\n"
                        "to its current value once\n"
                        "per frame";
    info->out_param_spec = "result_float:float";
    info->in_param_spec = "float_in:float,reset:enum?ok";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    value = 0;

    //--------------------------------------------------------------------------------------------------

    float_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"float_in");

    reset = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"reset");
    reset->set(-1);

    //--------------------------------------------------------------------------------------------------

    result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
    result_float->set(0.0f);
  }

  void run()
  {
    if (reset->get() == 0)
    {
      reset->set(-1);
      value = 0;
    }
    value += float_in->get();
    result_float->set(value);
  }

};
