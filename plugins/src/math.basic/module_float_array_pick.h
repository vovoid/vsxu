class module_float_array_pick : public vsx_module
{
  // in
  vsx_module_param_float_array* float_in;
  vsx_module_param_float* which;

  // out
  vsx_module_param_float* result_float;

  // internal
  vsx_float_array* my_array;

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;array;float_array_pick";
    info->description = "picks the value from the array/list\n"
                        "and delivers it as out parameter";
    info->in_param_spec = "float_in:float_array,which:float";
    info->out_param_spec = "result_float:float";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    float_in = (vsx_module_param_float_array*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"float_in");

    which = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"which");
    which->set(0);

    //--------------------------------------------------------------------------------------------------

    result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
    result_float->set(0);
  }

  void run()
  {
    my_array = float_in->get_addr();
    if (!my_array) return;

    result_float->set((*(my_array->data))[(int)which->get()]);
  }

};
