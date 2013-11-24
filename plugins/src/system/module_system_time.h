class module_system_time : public vsx_module 
{
  // in
	// out
	vsx_module_param_float* time_out;
	vsx_module_param_float* dtime_out;
	vsx_module_param_float* r_time_out;
	vsx_module_param_float* r_dtime_out;
	// internal

void module_info(vsx_module_info* info)
{
  info->identifier = "system;time";
//#ifndef VSX_NO_CLIENT
  info->description = "The outputs of this module need some\n\
explanation. The normal time is bound\n\
to the timeline used when creating a\n\
presentation. The 'real' is pure\n\
operating system time, that is -\n\
will be updated even if the sequenced\n\
time is stopped. In most cases you\n\
|||\n\
want to use the normal time unless you're\n\
creating a presentation and know that\n\
you need real time. Time is total\n\
time elapsed, dtime is the time that\n\
passed since the last frame.";
  info->out_param_spec = "normal:complex{time:float,dtime:float},real:complex{r_time:float,r_dtime:float}";
  info->component_class = "system";
//#endif
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  VSX_UNUSED(in_parameters);
  loading_done = true;
	time_out = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"time");
	time_out->set(0.0f);
	dtime_out = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"dtime");
	dtime_out->set(0.0f);
	r_time_out = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"r_time");
	r_time_out->set(0.0f);
	r_dtime_out = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"r_dtime");
	r_dtime_out->set(0.0f);
}

void run() {
  time_out->set(engine->vtime);
  dtime_out->set(engine->dtime);
  r_time_out->set(engine->real_vtime);
  r_dtime_out->set(engine->real_dtime);
}

};

