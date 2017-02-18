#include <vsx_gl_global.h>

class module_system_gl_vendor : public vsx_module
{
  // in

  // out
  vsx_module_param_float* intel;
  vsx_module_param_float* nvidia;
  vsx_module_param_float* amd;
  vsx_module_param_float* other;


  // internal
  bool detection_complete;

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "system;gl_vendor";

    info->description =
      "Detecte GL Vendors"
    ;

    info->in_param_spec =
      ""
    ;

    info->out_param_spec =
      "intel:float,"
      "nvidia:float,"
      "amd:float,"
      "other:float"
    ;

    info->component_class =
      "system";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    VSX_UNUSED(in_parameters);
    loading_done = true;
    detection_complete = false;

    intel = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "intel");
    intel->set(0.0f);

    nvidia = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "nvidia");
    nvidia->set(0.0f);

    amd = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "amd");
    amd->set(0.0f);

    other = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "other");
    other->set(0.0f);
  }

  bool activate_offscreen()
  {
    return true;
  }

  void run()
  {
    if (detection_complete)
      return;

    detection_complete = true;

    char *str = (char*)glGetString(GL_VENDOR);

    if (
        vsx_string<>(str).find("Intel") >= 0 ||
        vsx_string<>(str).find("intel") >= 0
    )
    {
      intel->set(1.0f);
      return;
    }

    if (
        vsx_string<>(str).find("NVIDIA") >= 0 ||
        vsx_string<>(str).find("nvidia") >= 0 ||
        vsx_string<>(str).find("Nvidia") >= 0 ||
        vsx_string<>(str).find("noveau") >= 0
    )
    {
      nvidia->set(1.0f);
      return;
    }

    if (
        vsx_string<>(str).find("AMD") >= 0 ||
        vsx_string<>(str).find("amd") >= 0 ||
        vsx_string<>(str).find("Amd") >= 0 ||
        vsx_string<>(str).find("Advanced") >= 0
    )
    {
      intel->set(1.0f);
      return;
    }

    other->set(1.0f);
  }

};

