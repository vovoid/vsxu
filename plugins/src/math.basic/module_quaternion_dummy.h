class module_quaternion_dummy : public vsx_module {
  // in
  vsx_module_param_quaternion* quaternion_in;

  // out
  vsx_module_param_quaternion* result_quaternion;

  // internal

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;dummies;quaternion_dummy";
    info->description = "This is only to gather connections\n"
        " - inside a macro for instance if \n"
        "you have many components which need\n"
        "to read the same value but you don't\n"
        "want 10 aliases going out.\n[quatin]\n"
        "is copied to [result_quat].";
    info->out_param_spec = "out_quat:quaternion";
    info->in_param_spec = "quat_in:quaternion";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    quaternion_in = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"quat_in");
    quaternion_in->set(0.0f,0);
    quaternion_in->set(0.0f,1);
    quaternion_in->set(0.0f,2);
    quaternion_in->set(1.0f,3);

    //--------------------------------------------------------------------------------------------------

    result_quaternion = (vsx_module_param_quaternion*)out_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"out_quat");
    result_quaternion->set(0.0f,0);
    result_quaternion->set(0.0f,1);
    result_quaternion->set(0.0f,2);
    result_quaternion->set(1.0f,3);
  }

  void run()
  {
    result_quaternion->set(quaternion_in->get(0),0);
    result_quaternion->set(quaternion_in->get(1),1);
    result_quaternion->set(quaternion_in->get(2),2);
    result_quaternion->set(quaternion_in->get(3),3);
  }
};
