class module_float3_dummy : public vsx_module
{
  // in
  vsx_module_param_float3* float3_in;

  // out
  vsx_module_param_float3* result_float3;

  // internal

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;dummies;float3_dummy";
    info->description = "This is only to gather connections\n"
                        " - inside a macro for instance if \n"
                        "you have many components which need\n"
                        "to read the same value but you don't\n"
                        "want 10 aliases going out.\n[floatin]\n"
                        "is copied to [result_float].";
    info->out_param_spec = "out_float3:float3";
    info->in_param_spec = "float3_in:float3";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    float3_in = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"float3_in");
    float3_in->set(0,0);
    float3_in->set(0,1);
    float3_in->set(0,2);

    //--------------------------------------------------------------------------------------------------

    result_float3 = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"out_float3");
    result_float3->set(0,0);
    result_float3->set(0,1);
    result_float3->set(0,2);
  }

  void run()
  {
    result_float3->set(float3_in->get(0),0);
    result_float3->set(float3_in->get(1),1);
    result_float3->set(float3_in->get(2),2);
  }
};
