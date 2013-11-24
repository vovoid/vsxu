class module_system_blocker_loading : public vsx_module
{
  // in
  vsx_module_param_float* fadeout_time;
  vsx_module_param_render* render_in;
  // out
  vsx_module_param_render* render_result;
  vsx_module_param_float* fadeout_out;
  // internal
  public:
    void module_info(vsx_module_info* info)
    {
      info->identifier = "system;blocker_loading";
      //#ifndef VSX_NO_CLIENT
      info->description = "Runs a rendering chain\
      Only when the engine is loading and time\
      seconds after that. Useful for \"loading\" overlays.";
      info->in_param_spec = "render_in:render,fadeout_time:float";
      info->out_param_spec = "render_out:render,fadeout_out:float";
      info->component_class = "system";
      //#endif
      info->tunnel = true; // always run this
    }
    
    void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
    {
      loading_done = true;
      fadeout_time = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"fadeout_time");
      fadeout_time->set(1.0f);
      
      render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
      render_in->set(0);
      render_in->run_activate_offscreen = true;
      render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
      render_result->set(0);
      fadeout_out = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"fadeout_out");
      fadeout_out->set(1.0f);
    }
    
    bool activate_offscreen() {
      if (engine->state == VSX_ENGINE_LOADING) return true;
      if (engine->state == VSX_ENGINE_PLAYING)
      {
        if (engine->vtime < fadeout_time->get())
        {
          fadeout_out->set(1.0f - engine->vtime/fadeout_time->get());
          return true;
        }
      }
      return false;
    }
    
};

