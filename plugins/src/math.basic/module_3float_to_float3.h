class module_3float_to_float3 : public vsx_module
{
  // in
  vsx_module_param_float* floata;
  vsx_module_param_float* floatb;
  vsx_module_param_float* floatc;

  // out
  vsx_module_param_float3* result3;

  // internal

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;converters;3float_to_float3";
    info->description = "[floata, floatb, floatc] are copied and \n"
                        "converted into \n"
                        "[result_float3] which is a float3\n"
                        "\n"
                        "This is a simple conversion module.\n"
                        "Whenever possible, try to set values\n"
                        "with sliders, this is only good if you\n"
                        "really need to convert";

    info->out_param_spec = "result_float3:float3";
    info->in_param_spec = "floata:float?help=`Set it to anything you like :)`,"
                          "floatb:float,"
                          "floatc:float";

    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    floata = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"floata");
    floatb = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"floatb");
    floatc = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"floatc");

    //--------------------------------------------------------------------------------------------------

    result3 = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"result_float3");
    result3->set(0.0f, 0);
    result3->set(0.0f, 1);
    result3->set(0.0f, 2);
  }

  void run()
  {
    result3->set(floata->get(),0);
    result3->set(floatb->get(),1);
    result3->set(floatc->get(),2);
  }

};
