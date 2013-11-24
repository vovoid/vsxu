class module_string_resource_to_string : public vsx_module
{
  // in
	vsx_module_param_resource* resource_in;
	// out
	vsx_module_param_string* string_out;
public:
  void module_info(vsx_module_info* info)
  {
    info->identifier = "string;converters;res_to_str||string;res_to_str";
    info->in_param_spec = "resource_in:resource";
    info->out_param_spec = "string_out:string";
    info->component_class = "system";
    info->description = "converts a resource string to a normal string";
  }
  
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    resource_in = (vsx_module_param_resource*)in_parameters.create(VSX_MODULE_PARAM_ID_RESOURCE,"resource_in");
    resource_in->set("");
    string_out = (vsx_module_param_string*)out_parameters.create(VSX_MODULE_PARAM_ID_STRING,"string_out");
    string_out->set("");
    loading_done = true;
  }
  void run() {
  	string_out->set(resource_in->get());
  }
};


