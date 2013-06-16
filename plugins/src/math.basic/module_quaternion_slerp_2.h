class module_quaternion_slerp_2 : public vsx_module
{
  // in
  vsx_module_param_quaternion* quat_a;
  vsx_module_param_quaternion* quat_b;
  vsx_module_param_float* pos;

  // out
  vsx_module_param_quaternion* result_quat;

  // internal
  vsx_quaternion q1,q2,q_out;

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;interpolation;quat_slerp_2p";
    info->description = "Rotate seamlessly between 2 angles/rotations";
    info->out_param_spec = "result_quat:quaternion";
    info->in_param_spec = "quat_a:quaternion,"
                          "quat_b:quaternion,"
                          "pos:float";
    info->component_class = "parameters";
  }


  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    q_out.x = q_out.y = q_out.z = 0.0f;
    q_out.w = 1.0f;

    //--------------------------------------------------------------------------------------------------

    quat_a = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION, "quat_a");
    quat_a->set(0.0f, 0);
    quat_a->set(0.0f, 1);
    quat_a->set(0.0f, 2);
    quat_a->set(1.0f, 3);

    quat_b = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION, "quat_b");
    quat_b->set(0.0f, 0);
    quat_b->set(0.0f, 1);
    quat_b->set(0.0f, 2);
    quat_b->set(1.0f, 3);

    pos = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "pos");

    //--------------------------------------------------------------------------------------------------

    result_quat = (vsx_module_param_quaternion*)out_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"result_quat");
    result_quat->set(0.0f,0);
    result_quat->set(0.0f,1);
    result_quat->set(0.0f,2);
    result_quat->set(1.0f,3);
  }

  void run()
  {
    q1.x = quat_a->get(0);
    q1.y = quat_a->get(1);
    q1.z = quat_a->get(2);
    q1.w = quat_a->get(3);
    q1.normalize();
    q2.x = quat_b->get(0);
    q2.y = quat_b->get(1);
    q2.z = quat_b->get(2);
    q2.w = quat_b->get(3);
    q2.normalize();
    float p = pos->get();
    if (p < 0.0f) p = 0.0f;
    if (p > 1.0f) p = 1.0f;
    q_out.slerp(q1, q2, p);

    result_quat->set(q_out.x,0);
    result_quat->set(q_out.y,1);
    result_quat->set(q_out.z,2);
    result_quat->set(q_out.w,3);
  }

};
