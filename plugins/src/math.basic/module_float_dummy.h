class module_float_dummy : public vsx_module
{
  // in
  vsx_module_param_float* float_in;

  // out
  vsx_module_param_float* result_float;

  // internal

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;dummies;float_dummy";
    info->description =
      "[floatin] is copied without modification into\n"
      "[result_float]\n"
      "\n"
      "This module is good for distributing a value you want to\n"
      "set to many components. Or host to code\n"
      "a filter in VSXL. For instance if you\n"
      "have many components in a macro which need\n"
      "the same value but you don't want 10\n"
      "aliases going out, it's a good idea to\n"
      "keep one inside your macro.\n"
      ;
    info->out_param_spec = "result_float:float";
    info->in_param_spec = "float_in:float";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    float_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"float_in");
    float_in->set(1);

    //--------------------------------------------------------------------------------------------------

    result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
    result_float->set(1);
  }

  void run()
  {
    result_float->set(float_in->get());
  }

};
