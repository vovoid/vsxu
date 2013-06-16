class module_vector_float4_to_4float : public vsx_module {
  // in
  vsx_module_param_float4* in_float4;

  // out
  vsx_module_param_float* param1;
  vsx_module_param_float* param2;
  vsx_module_param_float* param3;
  vsx_module_param_float* param4;

  // internal

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;converters;float4_to_4float";
    info->description = "takes float4\n"
                        "outputs 4 floats\n";

    info->in_param_spec = "in_float4:float4";
    info->out_param_spec = "param1:float,"
                           "param2:float,"
                           "param3:float,"
                           "param4:float";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    in_float4 = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"in_float4");
    in_float4->set(0.0f,0);
    in_float4->set(0.0f,1);
    in_float4->set(0.0f,2);
    in_float4->set(1.0f,3);

    //--------------------------------------------------------------------------------------------------

    param1 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param1");
    param1->set(0.0f);

    param2 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param2");
    param2->set(0.0f);

    param3 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param3");
    param3->set(0.0f);

    param4 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param4");
    param4->set(0.0f);

  }

  void run()
  {
    param1->set(in_float4->get(0));
    param2->set(in_float4->get(1));
    param3->set(in_float4->get(2));
    param4->set(in_float4->get(3));
  }

};
