class module_system_trig_sequencer : public vsx_module 
{
  // in
  vsx_module_param_float* trig_play;
  vsx_module_param_float* trig_stop;
  vsx_module_param_float* trig_rewind;
  vsx_module_param_float* trig_set_time;
  // internal
  float prev_play;
  float prev_stop;
  float prev_rewind;
public:

void module_info(vsx_module_info* info)
{
  info->output = 1;
  info->identifier = "system;system_sequencer_control";
  info->description = "";
  info->out_param_spec = "";
  info->in_param_spec = "trig_play:float,"
                        "trig_stop:float,"
                        "trig_rewind:float,"
                        "trig_set_time:float"
                        ;
  info->component_class = "system";
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  VSX_UNUSED(out_parameters);
  loading_done = true;
  trig_play = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"trig_play");
  trig_stop = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"trig_stop");
  trig_rewind = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"trig_rewind");
  trig_set_time = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"trig_set_time");
  trig_set_time->set(-0.01f);
  //--------------------------------------------------------------------------------------------------  
}

void run() {
  // set engine play state
  if (prev_play == 0.0f && trig_play->get() > 0.0f) {
    engine->request_play = 1;
  }
  prev_play = trig_play->get();
  
  // set engine stop state
  if (prev_stop == 0.0f && trig_stop->get() > 0.0f) {
    engine->request_stop = 1;
  }
  prev_stop = trig_stop->get();

  // set engine rewind state
  if (prev_rewind == 0.0f && trig_rewind->get() > 0.0f) {
    engine->request_rewind = 1;
  }
  prev_rewind = trig_rewind->get();
  // set engine vtime
  engine->request_set_time = trig_set_time->get();
}

};


