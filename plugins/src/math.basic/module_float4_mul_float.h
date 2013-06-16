class module_float4_mul_float : public vsx_module {
  // in
  vsx_module_param_float4* param1;
  vsx_module_param_float* param2;

  // out
  vsx_module_param_float4* result4;

  // internal

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "!maths;arithmetics;vec4;float4_mul_float||maths;arithmetics;float4;float4_mul_float";
    info->description = "multiplies each of the values in\n"
                        "the float4 vector with the float\n"
                        "value in param1";
    info->in_param_spec = "param1:float4,param2:float";
    info->out_param_spec = "result_float4:float4";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    param1 = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"param1");
    param1->set(0,0);
    param1->set(0,1);
    param1->set(0,2);
    param1->set(0,3);

    param2 = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param2");

    //--------------------------------------------------------------------------------------------------

    result4 = (vsx_module_param_float4*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"result_float4");
    result4->set(0,0);
    result4->set(0,1);
    result4->set(0,2);
    result4->set(0,3);
  }

  void run()
  {
    result4->set(param1->get(0)*param2->get(),0);
    result4->set(param1->get(1)*param2->get(),1);
    result4->set(param1->get(2)*param2->get(),2);
    result4->set(param1->get(3)*param2->get(),3);
  }
};
