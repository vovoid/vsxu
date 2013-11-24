
#include "vsx_tm.h"
class module_system_tm_m : public vsx_module
{
  // in
  vsx_module_param_string* handle_name;
  vsx_module_param_render* render_in;
  // out
  vsx_module_param_render* render_result;
  // internal
  vsx_string sname;
  char* iname;
public:
  void module_info(vsx_module_info* info)
  {
    info->identifier = "system;tm_measure";
    info->description = "";
    info->in_param_spec = "render_in:render,handle_name:string";
    info->out_param_spec = "render_out:render";
    info->component_class = "system";
    info->tunnel = true; // always run this
  }


  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    handle_name = (vsx_module_param_string*)in_parameters.create(VSX_MODULE_PARAM_ID_STRING,"handle_name");
    handle_name->set("default");

    render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
    render_in->set(0);
    render_in->run_activate_offscreen = true;
    render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_result->set(0);
  }

  void param_set_notify(const vsx_string& name)
  {
    #ifdef VSXU_TM
    if (name == "handle_name")
    {
      sname = handle_name->get();
      iname = (char*)sname.c_str();
    }
    #endif
  }

  bool activate_offscreen()
  {
    #ifdef VSXU_TM
    if (!engine->tm) return true;
    ((vsx_tm*)engine->tm)->e( iname );
    #endif
    return true;

  }

  void deactivate_offscreen()
  {
    #ifdef VSXU_TM
    if (!engine->tm) return;
    ((vsx_tm*)engine->tm)->l();
    #endif
  }

};


