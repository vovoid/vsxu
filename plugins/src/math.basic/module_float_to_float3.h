class module_float_to_float3 : public vsx_module
{
  // in
  vsx_module_param_float* param1;

  // out
  vsx_module_param_float3* result3;

  // internal
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;converters;float_to_float3";
    info->description = "float to float3 (vector) conversion\n"
                        "param1 is copied into all 3 of the\n"
                        "values in the float3 parameter\n"
                        "\n"
                        "useful for generating a uniform\n"
                        "scaling vector for instance";
    info->in_param_spec = "param1:float";
    info->out_param_spec = "result_float3:float3";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    param1 = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param1");

    //--------------------------------------------------------------------------------------------------

    result3 = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"result_float3");
    result3->set(0,0);
    result3->set(0,1);
    result3->set(0,2);
  }

  void run()
  {
    result3->set(param1->get(),0);
    result3->set(param1->get(),1);
    result3->set(param1->get(),2);
  }
};
