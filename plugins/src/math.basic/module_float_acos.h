class module_float_acos : public vsx_module
{
  // in
  vsx_module_param_float* float_in;

  // out
  vsx_module_param_float* result_float;

  // internal

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;arithmetics;binary;acos";
    info->description = "acos:\n"
                        "result = acos( float_in )\n"
                        "returns the arcus cosine\n"
                        "of float_in given\n"
                        "in radians";
    info->in_param_spec = "float_in:float";
    info->out_param_spec = "result_float:float";
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
    result_float->set((float)acos(float_in->get()));
  }

};
