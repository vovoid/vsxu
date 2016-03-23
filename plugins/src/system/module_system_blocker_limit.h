class module_system_blocker_limit : public vsx_module
{
  // in
  vsx_module_param_float* block;
  vsx_module_param_float* limit;
  vsx_module_param_render* render_in;

  // out
  vsx_module_param_render* render_result;
  vsx_module_param_float* passthru;


  // internal

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "system;blocker_limit";

    info->description =
      "Blocks a rendering chain."
      "If the control value is less"
      "than 0.5 the chain is blocked,"
      "if higher or equal it's run."
    ;

    info->in_param_spec =
      "render_in:render,"
        "block:float,"
        "limit:float"
      ;

    info->out_param_spec =
      "render_out:render,"
      "passthru:float"
    ;

    info->component_class =
      "system";

    info->tunnel = true;
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    block = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"block");
    block->set(1.0f);

    limit = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"limit");
    limit->set(0.5f);


    render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
    render_in->set(0);
    render_in->run_activate_offscreen = true;
    render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_result->set(0);

    passthru = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"passthru");
    passthru->set(1.0f);

  }

  bool activate_offscreen()
  {
    passthru->set( block->get() );

    if (engine_state->state == VSX_ENGINE_LOADING)
      return true;

    if (block->get() >= limit->get())
      return true;

    return false;
  }

};

