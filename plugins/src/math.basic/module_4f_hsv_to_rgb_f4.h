class module_4f_hsv_to_rgb_f4 : public vsx_module
{
  // in
  vsx_module_param_float* floata;
  vsx_module_param_float* floatb;
  vsx_module_param_float* floatc;
  vsx_module_param_float* floatd;

  // out
  vsx_module_param_float4* result4;

  // internal
  vsx_color color;
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;color;converters;4f_hsv_to_f4_rgb";

    info->description = "\
  This module converts color provided in\n\
  HSV colorspace into RGB along with alpha.\n\
  It takes 4 floats,\n\
    [H]ue (color - red/green etc.),\n\
    [S]aturation (vibrancy)\n\
    [V]alue (brightness)\n\
    [A]lpha\n\
  and converts to an RedGreenBlueAlpha float4";
    info->out_param_spec = "result_float4:float4";
    info->in_param_spec =
      "h:float,"
      "s:float,"
      "v:float,"
      "a:float"
    ;

    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    floata = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"h");
    floatb = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"s");
    floatc = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"v");
    floatd = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"a");
    floata->set(0);
    floatb->set(1);
    floatc->set(1);
    floatd->set(1);

    //--------------------------------------------------------------------------------------------------

    result4 = (vsx_module_param_float4*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"result_float4");
    result4->set(0,0);
    result4->set(0,1);
    result4->set(0,2);
    result4->set(0,3);
  }

  void run()
  {
    color.hsv((float)fmod(fabs(floata->get()),1.0),floatb->get(),floatc->get());
    result4->set(color.r,0);
    result4->set(color.g,1);
    result4->set(color.b,2);
    result4->set(floatd->get(),3);
  }
};
