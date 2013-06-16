class module_float_abs : public vsx_module
{
  // in
  vsx_module_param_float* float_in;

  // out
  vsx_module_param_float* result_float;

  // internal

public:
  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;arithmetics;binary;abs";
    info->description = "abs:\n"
                        "result = abs( float_in )\n"
                        "turns negative values into positive\n"
                        "for instance, -0.67 becomes 0.67";
    info->out_param_spec = "result_float:float";
    info->in_param_spec = "float_in:float";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    float_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"float_in");

    //--------------------------------------------------------------------------------------------------

    result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
    result_float->set(0);
  }

  void run()
  {
    result_float->set((float)fabs(float_in->get()));
  }
};
