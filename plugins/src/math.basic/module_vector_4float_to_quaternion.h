class module_vector_4float_to_quaternion : public vsx_module
{
  // in
  vsx_module_param_float* param1;
  vsx_module_param_float* param2;
  vsx_module_param_float* param3;
  vsx_module_param_float* param4;

  // out
  vsx_module_param_quaternion* result_quat;

  // internal

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;converters;4float_to_quaternion";
    info->description = "takes 4 float values and forms a\n"
                        "quaternion\n";

    info->out_param_spec = "result_quat:quaternion";
    info->in_param_spec = "param1:float,"
                          "param2:float,"
                          "param3:float,"
                          "param4:float";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    param1 = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param1");
    param2 = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param2");
    param3 = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param3");
    param4 = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param4");

    //--------------------------------------------------------------------------------------------------

    result_quat = (vsx_module_param_quaternion*)out_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"result_quat");
    result_quat->set(0.0f,0);
    result_quat->set(0.0f,1);
    result_quat->set(0.0f,2);
    result_quat->set(1.0f,3);
  }

  void run()
  {
    result_quat->set(param1->get(),0);
    result_quat->set(param2->get(),1);
    result_quat->set(param3->get(),2);
    result_quat->set(param4->get(),3);
  }

};
