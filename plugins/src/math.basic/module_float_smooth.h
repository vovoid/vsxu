class module_float_smooth : public vsx_module
{
  // in
  vsx_module_param_float* float_in;
  vsx_module_param_float* speed;

  // out
  vsx_module_param_float* result_float;

  // internal
  double internal_value;

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "!maths;interpolation;float_interpolation||maths;interpolation;float_smoother";
    info->description = "Smoothens a value over time\n"
                        " - same algorith as the knobs/sliders";
    info->out_param_spec = "result_float:float";
    info->in_param_spec = "value_in:float,speed:float";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    internal_value = 0.0f;

    //--------------------------------------------------------------------------------------------------

    float_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"value_in");
    float_in->set(0.0f);

    speed = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"speed");
    speed->set(1.0f);

    //--------------------------------------------------------------------------------------------------

    result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
    result_float->set(0.0f);
  }

  void run()
  {
    double tt = engine->real_dtime*speed->get();
    if (tt > 1.0) tt = 1.0;

    double temp = (float)(internal_value*(1.0-tt)+(double)float_in->get() * tt);

    internal_value = temp;
    result_float->set((float)internal_value);
  }
};
