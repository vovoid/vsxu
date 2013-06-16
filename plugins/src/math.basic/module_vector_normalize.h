class module_vector_normalize : public vsx_module
{
  // in
  vsx_module_param_float3* param1;

  // out
  vsx_module_param_float3* result3;

  // internal

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;arithmetics;vector;vector_normalize";
    info->description = "Normalizes the vector (length = 1.0)\n";

    info->out_param_spec = "result_float3:float3";
    info->in_param_spec = "param1:float3";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    param1 = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"param1");

    //--------------------------------------------------------------------------------------------------

    result3 = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"result_float3");
    result3->set(0,0);
    result3->set(0,1);
    result3->set(0,2);
  }

  void run()
  {
    float v = (float)
      (
        1.0
        /
        sqrt(
          param1->get(0)*param1->get(0) +
          param1->get(1)*param1->get(1) +
          param1->get(2)*param1->get(2)
        )
      )
    ;

    result3->set(param1->get(0)*v,0);
    result3->set(param1->get(1)*v,1);
    result3->set(param1->get(2)*v,2);
  }

};
