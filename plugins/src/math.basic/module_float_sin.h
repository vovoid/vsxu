class module_float_sin : public vsx_module
{
  // in
  vsx_module_param_float* float_in;

  // out
  vsx_module_param_float* result_float;

  // internal

public:
  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;arithmetics;binary;sin";
    info->description = "sin:\n"
                        "result = sin( float_in )\n"
                        "returns the sine of float_in given\n"
                        "in radians";
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
    result_float->set((float)sin(float_in->get()));
  }
};
