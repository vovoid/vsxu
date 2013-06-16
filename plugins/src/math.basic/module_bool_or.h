class module_bool_or : public vsx_module
{
  // in
  vsx_module_param_float* float_a;
  vsx_module_param_float* float_b;

  // out
  vsx_module_param_float* result_float;

  // internal

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;arithmetics;boolean;or";
    info->description = "boolean or:\n"
                        "result = a or b\n"
                        "the logic operation or:\n"
                        "1 or 0 = 1\n"
                        "0 or 1 = 1\n"
                        "0 or 0 = 0\n"
                        "1 or 1 = 1\n"
                        "\n"
                        "Note: 1 is defined as\n"
                        "larger than +/-0.5";
    info->in_param_spec = "a:float,b:float";
    info->out_param_spec = "result_float:float";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    float_a = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"a");
    float_b = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"b");

    //--------------------------------------------------------------------------------------------------

    result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
    result_float->set(0);
  }

  void run()
  {
    result_float->set((float)(round(float_a->get()) || round(float_b->get())));
  }
};
