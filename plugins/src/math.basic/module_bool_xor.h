class module_bool_xor : public vsx_module
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
    info->identifier = "maths;arithmetics;boolean;xor";
    info->description = "";
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
    result_float->set((float)((bool)round(float_a->get()) ^ (bool)round(float_b->get())));
  }
};
