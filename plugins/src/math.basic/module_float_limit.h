class module_float_limit : public vsx_module
{
  // in
  vsx_module_param_float* float_in;
  vsx_module_param_float* limit_value;
  vsx_module_param_int* type;

  // out
  vsx_module_param_float* result_float;

  // internal

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;limiters;float_limiter";
    info->description = "If value_in is la\n"
                        "limit_value, result is set to\n"
                        "limit_value, otherwise result\n"
                        "is set to value_in.\n";
    info->out_param_spec = "result_float:float";
    info->in_param_spec = "value_in:float,limit_value:float,type:enum?max|min";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    float_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"value_in");
    float_in->set(0.0f);

    limit_value = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"limit_value");
    limit_value->set(1.0f);

    type = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"type");

    //--------------------------------------------------------------------------------------------------

    result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
    result_float->set(0.0f);
  }

  void run()
  {
    if (type->get() == 0)
    {
      if (float_in->get() > limit_value->get())
      {
        result_float->set(limit_value->get());
      } else
      {
        result_float->set(float_in->get());
      }
    } else
    {
      if (float_in->get() < limit_value->get())
      {
        result_float->set(limit_value->get());
      }
      else
      {
        result_float->set(float_in->get());
      }
    }
  }
};
