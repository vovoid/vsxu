class module_system_clock : public vsx_module 
{
	// out
	vsx_module_param_float* year;
	vsx_module_param_float* month;
	vsx_module_param_float* day_of_week;
	vsx_module_param_float* day;
	vsx_module_param_float* hour;
	vsx_module_param_float* hour12;
	vsx_module_param_float* minute;
	vsx_module_param_float* second;
	vsx_module_param_float* millisecond;
public:

void module_info(vsx_module_info* info)
{
  info->identifier = "system;clock";
//#ifndef VSX_NO_CLIENT
	info->in_param_spec ="";
  //info->description = "";
  info->out_param_spec = "clock:complex{year:float,\
month:float,\
dayofweek:float,\
day:float,\
hour:float,\
hour12:float,\
minute:float,\
second:float,\
millisecond:float}";
  info->component_class = "system";

//#endif
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  VSX_UNUSED(in_parameters);
  loading_done = true;
	year = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"year");
	year->set(0);
	month = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"month");
	month->set(0);
	day_of_week = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"dayofweek");
	day_of_week->set(0);
	day = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"day");
	day->set(0);
	hour = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"hour");
	hour->set(0);
	hour12 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"hour12");
	hour12->set(0);
	minute = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"minute");
	minute->set(0);
	second = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"second");
	second->set(0);
	millisecond = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"millisecond");
	millisecond->set(0);
}
#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
SYSTEMTIME time;
#endif

void run() {
#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
	float sec = (float)time.wSecond+(float)time.wMilliseconds*0.001;
	float min = (float)time.wMinute+sec/60.0f;
	float hr = (float)time.wHour + min/60.0f;
	GetLocalTime(&time);
	year->set(time.wYear);
	month->set(time.wMonth);
	day_of_week->set(time.wDayOfWeek);
	day->set(time.wDay);
	hour->set(hr);
	hour12->set(fmod(hr,12.0f));
	minute->set(min);
	second->set(sec);
	millisecond->set(time.wMilliseconds);
#endif
#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
	time_t result;
  result = time(NULL);
  tm* t = localtime(&result);
  struct timeval now;
  gettimeofday(&now, 0);
  float sec = (float)t->tm_sec + 0.000001*(double)now.tv_usec;
  float min = (float)t->tm_min + sec / 60.0f;
  float hr = (float)t->tm_hour + min / 60.0f;
  year->set(t->tm_year);
  month->set(t->tm_mon);
  day_of_week->set(t->tm_wday);
  day->set(t->tm_mday);
  hour->set(hr);
  hour12->set(fmod(hr,12.0f));
  minute->set(min);
  second->set(sec);
  millisecond->set(0.001 * (double)now.tv_usec);
  
  //float sec = (float)tm.tm_sec + fmod(engine->real_vtime, 1.0f);
  //float min = (float)tm.tm_sec + fmod(engine->real_vtime, 1.0f);
#endif
}

};


