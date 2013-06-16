class module_vector_add : public vsx_module
{
  // in
  vsx_module_param_float3* param1;
  vsx_module_param_float3* param2;

  // out
  vsx_module_param_float3* result3;

  // internal

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;arithmetics;vector;vector_add||maths;arithmetics;float3;float3_add_float3";
    info->description = "";

    info->out_param_spec = "result_float3:float3";
    info->in_param_spec = "\
  param1:float3,\
  param2:float3";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    param1 = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"param1");
    param2 = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"param2");

    //--------------------------------------------------------------------------------------------------

    result3 = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"result_float3");
    result3->set(0,0);
    result3->set(0,1);
    result3->set(0,2);
  }

  void run()
  {
    result3->set(param1->get(0)+param2->get(0),0);
    result3->set(param1->get(1)+param2->get(1),1);
    result3->set(param1->get(2)+param2->get(2),2);
  }

};
