class module_string_dummy : public vsx_module
{
  // in
  vsx_module_param_string* string_in;

  // out
  vsx_module_param_string* string_out;

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "string;string_dummy||dummies;string_dummy";

    info->in_param_spec =
      "string_in:string";

    info->out_param_spec =
      "string_out:string";

    info->component_class =
      "system";

    info->description =
      "Holds a string value";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    string_in = (vsx_module_param_string*)in_parameters.create(VSX_MODULE_PARAM_ID_STRING,"string_in");
    string_in->set("");
    string_out = (vsx_module_param_string*)out_parameters.create(VSX_MODULE_PARAM_ID_STRING,"string_out");
    string_out->set("");
    loading_done = true;
  }

  void run()
  {
    string_out->set(string_in->get());
  }
};


