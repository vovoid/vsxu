class module_4float_to_float4 : public vsx_module
{
  // in
  vsx_module_param_float* floata;
  vsx_module_param_float* floatb;
  vsx_module_param_float* floatc;
  vsx_module_param_float* floatd;

  // out
  vsx_module_param_float4* result4;

  // internal

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;converters;4float_to_float4";
    info->description = ""
                        "[floata, floatb, floatc, floatd] are copied and \n"
                        "converted into \n"
                        "[result_float4] which is a float3\n"
                        "\n"
                        "This is a simple conversion module.\n"
                        "Whenever possible, try to set values\n"
                        "with sliders, this is only good if you\n"
                        "really need to convert";

    info->out_param_spec = "result_float4:float4";
    info->in_param_spec = "\
  floata:float,\
  floatb:float,\
  floatc:float,\
  floatd:float";

    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    floata = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"floata");
    floatb = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"floatb");
    floatc = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"floatc");
    floatd = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"floatd");
    floata->set(1);
    floatb->set(1);
    floatc->set(1);
    floatd->set(1);

    //--------------------------------------------------------------------------------------------------

    result4 = (vsx_module_param_float4*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"result_float4");
    result4->set(1,0);
    result4->set(1,1);
    result4->set(1,2);
    result4->set(1,3);
  }

  void run()
  {
    result4->set(floata->get(),0);
    result4->set(floatb->get(),1);
    result4->set(floatc->get(),2);
    result4->set(floatd->get(),3);
  }
};
