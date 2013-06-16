class module_float3to3float : public vsx_module
{
  // in
  vsx_module_param_float3* float3_in;

  // out
  vsx_module_param_float* a;
  vsx_module_param_float* b;
  vsx_module_param_float* c;

  // internal

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;converters;float3to3float";
    info->description = "This is only to gather connections - inside a macro for instance if you have many components which need to read the same value but you don't want 10 aliases going out.\n[floatin] is copied to [result_float].";
    info->out_param_spec = "a:float,b:float,c:float";
    info->in_param_spec = "float3_in:float3";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    float3_in = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"float3_in");

    //--------------------------------------------------------------------------------------------------

    a = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"a");
    a->set(0.0f);
    b = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"b");
    b->set(0.0f);
    c = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"c");
    c->set(0.0f);
  }

  void run()
  {
    a->set(float3_in->get(0));
    b->set(float3_in->get(1));
    c->set(float3_in->get(2));
  }
};
