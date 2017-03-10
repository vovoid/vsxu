#ifndef MODULERENDERSTATE_H
#define MODULERENDERSTATE_H

#include "vsx_param.h"
#include <module/vsx_module.h>
#include "vsx_engine.h"
#include "vsx_engine_helper.h"
#include "vsx_data_path.h"

class module_render_state : vsx_module
{
  vsx_engine_helper* helper;
  vsx_module_param_render* render_result;
  vsx_module_param_resource* filename_in;
  vsx_string<>current_filename;

public:
  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "renderers;state_loader"
      "||"
      "system;state_loader";

    info->in_param_spec =
      "filename:resource";

    info->out_param_spec =
      "render_out:render";

    info->component_class =
      "system";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_result->set(0);
    filename_in = (vsx_module_param_resource*)in_parameters.create(VSX_MODULE_PARAM_ID_RESOURCE,"filename");
    filename_in->set("");
    current_filename = "";
    helper = 0;
  }

  void run()
  {
    if(filename_in->get() != current_filename)
    {
      if (!vsx_string_helper::verify_filesuffix(filename_in->get(),"vsx"))
      {
        user_message = "module||ERROR!! File not .vsx!";
        return;
      }
      current_filename = filename_in->get();
      if(helper)
      {
        delete helper;
      }
      helper = new vsx_engine_helper( vsx_data_path::get_instance()->data_path_get() + current_filename, (vsx_module_list_abs*) engine_state->module_list);
    }
  }

  void output(vsx_module_param_abs* param)
  {
    VSX_UNUSED(param);
    if(helper)
    {
       helper->render();
       render_result->set(1);
    }
  }

  void on_delete()
  {
    if(helper)
    {
      delete helper;
    }
  }
private:

};

#endif // MODULERENDERSTATE_H
