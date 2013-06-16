class module_vector_dot_product : public vsx_module
{
  // in
  vsx_module_param_float3* param1;
  vsx_module_param_float3* param2;

  // out
  vsx_module_param_float* result;

  // internal

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;arithmetics;vector;vector_dot_product";
    info->description = "Vector dot product - \n"
                        "'Multiply' one vector by another";
    info->in_param_spec = "param1:float3, param2:float3";
    info->out_param_spec = "result_float:float";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    param1 = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"param1");

    param2 = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"param2");

    //--------------------------------------------------------------------------------------------------

    result = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
    result->set(0.0f);
  }

  void run()
  {
    result->set(
        param1->get(0) * param2->get(0)
        +
        param1->get(1) * param2->get(1)
        +
        param1->get(2) * param2->get(2)
    );
  }

};
