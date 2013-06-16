class module_float4_dummy : public vsx_module
{
  // in
  vsx_module_param_float4* float4_in;

  // out
  vsx_module_param_float4* result_float4;

  // internal

public:
  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;dummies;float4_dummy";
    info->description = "This is only to gather connections\n"
                        " - inside a macro for instance if \n"
                        "you have many components which need\n"
                        "to read the same value but you don't\n"
                        "want 10 aliases going out.\n[floatin]\n"
                        "is copied to [result_float].";
    info->out_param_spec = "out_float4:float4";
    info->in_param_spec = "float4_in:float4";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    float4_in = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"float4_in");
    float4_in->set(0,0);
    float4_in->set(0,1);
    float4_in->set(0,2);
    float4_in->set(0,3);

    //--------------------------------------------------------------------------------------------------

    result_float4 = (vsx_module_param_float4*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"out_float4");
    result_float4->set(0,0);
    result_float4->set(0,1);
    result_float4->set(0,2);
    result_float4->set(0,3);
  }

  void run()
  {
    result_float4->set(float4_in->get(0),0);
    result_float4->set(float4_in->get(1),1);
    result_float4->set(float4_in->get(2),2);
    result_float4->set(float4_in->get(3),3);
  }
};
