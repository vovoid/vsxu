class module_float_accumulator_limits : public vsx_module
{
  // in
  vsx_module_param_float* float_in;
  vsx_module_param_float* limit_lower;
  vsx_module_param_float* limit_upper;
  vsx_module_param_int* reset;

  // out
  vsx_module_param_float* result_float;

  // internal
  float value;

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;accumulators;float_accumulator_limits";
    info->description = "accumulator - float with limits\n"
                        "result += in\n"
                        "\n"
                        "adds the value in float_in\n"
                        "to its current value once\n"
                        "per frame";
    info->out_param_spec = "result_float:float";
    info->in_param_spec = "float_in:float,limit_lower:float,limit_upper:float,reset:enum?ok";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    value = 0.0f;

    //--------------------------------------------------------------------------------------------------

    float_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"float_in");

    limit_lower = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"limit_lower");

    limit_upper = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"limit_upper");
    limit_upper->set(1.0f);

    reset = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"reset");
    reset->set(-1);

    //--------------------------------------------------------------------------------------------------

    result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
    result_float->set(0.0f);
  }

  void run()
  {
    if (reset->get() == 0) {
      reset->set(-1);
      value = 0;
    }
    value += float_in->get();
    if (value > limit_upper->get()) value = limit_upper->get();
    if (value < limit_lower->get()) value = limit_lower->get();
    result_float->set(value);
  }

};
