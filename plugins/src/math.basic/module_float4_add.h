class module_float4_add : public vsx_module
{
  // in
  vsx_module_param_float4* param1;
  vsx_module_param_float4* param2;

  // out
  vsx_module_param_float4* result4;

  // internal

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "!maths;arithmetics;vector;float4_add|maths;arithmetics;float4;float4_add";
    info->description = "";

    info->out_param_spec = "result_float4:float4";
    info->in_param_spec = ""
      "param1:float4,"
      "param2:float4";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    param1 = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"param1");
    param2 = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"param2");

    //--------------------------------------------------------------------------------------------------

    result4 = (vsx_module_param_float4*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"result_float4");
    result4->set(0,0);
    result4->set(0,1);
    result4->set(0,2);
    result4->set(0,3);
  }

  void run()
  {
    result4->set(param1->get(0)+param2->get(0),0);
    result4->set(param1->get(1)+param2->get(1),1);
    result4->set(param1->get(2)+param2->get(2),2);
    result4->set(param1->get(3)+param2->get(3),3);
  }
};
