#include "_configuration.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include "main.h"
#include "vsx_math_3d.h"

class vsx_module_plugin_maths_oscillator : public vsx_module {
  // in
	vsx_module_param_int* osc_type; // square wave, triangle, sin
	vsx_module_param_float* amp;
	vsx_module_param_float* ofs;
	vsx_module_param_float* phase;
	vsx_module_param_float* freq;
	vsx_module_param_float* drive;
	vsx_module_param_int* time_source; // 0 or 1
	vsx_module_param_int* drive_type; // 0 or 1
	
	// out
	vsx_module_param_float* result1;

	// internal
  float time;
  float vtime, dtime;
  float rand_time_accum;

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;oscillators;oscillator";
    info->description = ""
      "This oscillator can iterate over\n"
      "time, random or by external source\n"
      "(float param). It can give noise\n"
      "(random) values, sin wave,saw or\n"
      "square.\n"
      "The resulting float value ranges from\n"
      "ofs-amp up to ofs+amp.";
    info->in_param_spec = 
      "osc:complex"
      "{"
      "  osc_type:enum?"
      "    noise|sin|saw|square|triangle|quadratic,"
      "  freq:float,"
      "  amp:float,"
      "  ofs:float,"
      "  phase:float?help=`Not often useful,\nbut it shifts time back and forth.`,"
      "}"
      ","
      "options:complex"
      "{"
      "  time_source:enum?operating_system|sequence,"
      "  drive_type:enum?time_internal_absolute|time_internal_relative|external,"
      "  drive:float"
      "}"
      ;
    info->out_param_spec = "float:float";
    
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    // TODO:
    loading_done = true;
  
  	osc_type = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "osc_type");
    // 1 = white noise (random)
    // 2 = brown noise
    // 3 = 
  	osc_type->set(1); // noise
  
  	amp = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"amp");
  	ofs = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"ofs");
  	phase = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"phase");
  	freq = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"freq");
  	drive = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"drive");
  	time_source = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"time_source");
  	drive_type = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"drive_type");
  	phase->set(0);
  	amp->set(0.5f);
  	ofs->set(0.5f);
  	freq->set(1); // one revolution per second
  	drive->set(1);
  	time_source->set(0);
  	drive_type->set(1);
  	time = 0;
  	// amplitude = 0.5
  	// phase 0.5
  	// frequency 1
  	rand_time_accum = 0.0f;
  	result1 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"float");
    result1->set(0);
  //--------------------------------------------------------------------------------------------------	
    // NO SPACES IN THIS STRING!!!
    //in_param_spec = "complex:surround_sound_data_in[blob:center,complex:rear_data[complex:balance[float:bal,float:gain],blob:Rleft,blob:Rright],complex:front_data[blob:Fleft,blob:Fright]],float:seed";
    
  
  //  out_info = "blob:center complex:rear_data[blob:Rleft blob:Rright] complex:front_data[blob:Fleft blob:Fright]";
  //  out_info = "float:result2";
    //complex[float,complex[float,float,float]]
    
  }

  void run() {
    if (time_source->get() == 0) {
      vtime = engine->real_vtime;
      dtime = engine->real_dtime;
    } else {
      vtime = engine->vtime;
      dtime = engine->dtime;
    }
    switch (drive_type->get()) {
      case 0:
        time = vtime*freq->get();
        break;
      case 1:
        if (dtime + vtime == 0) {
          time = 0;
        } else
  //      time += engine->dtime*freq->get();
        time += dtime*freq->get();
        //printf("osc: %f %f freq %f\n",time, engine->vtime,freq->get());
        break;
      case 2: time = drive->get();
    }  
  
    switch (osc_type->get()) {
      // noise
      case 0:
      	{
      		rand_time_accum += dtime;
      		if (rand_time_accum > freq->get()) {
      			rand_time_accum = 0.0f;
      			result1->set(((rand()/(float)RAND_MAX)-0.5f) * amp->get() + ofs->get());
      		}
      	}
    	break;
    	// sin
      case 1:
				result1->set((float)sin((time+phase->get())*pi_float) * amp->get() + ofs->get());
      	//result1->set((float)sin((time+phase->get())*pi) * amp->get() + ofs->get());
    	break;
    	// saw
      case 2:
      	result1->set(((float)fmod((time+phase->get()),1)-0.5f)* amp->get() + ofs->get());
    	break;
    	// square
      case 3:
      	result1->set(((float)round(fmod((time+phase->get()),1))-0.5f)* amp->get() + ofs->get());
    	break;
    	// triangle
      case 4:   //1-2*abs(1-2*f*t%2)
      	result1->set((1.0f-2.0f*(float)fabs(1-fmod(time+phase->get(),2.0f)))* amp->get() + ofs->get());
    	break;
      // quadratic 
      case 5:
        result1->set(
              (float)pow( 
                fmod(
                  time + phase->get(),
                  pow(amp->get(),0.5f)
                )
                ,2.0
              ) 
              * 
              amp->get() 
              + 
              ofs->get());
      break;
    }
  }
};


class vsx_module_plugin_maths_oscillators_float_sequencer : public vsx_module {
  // in
  vsx_module_param_sequence* float_sequence;
  vsx_module_param_float* length;
  vsx_module_param_float* trigger;
  vsx_module_param_float* trigger_reverse;
  vsx_module_param_float* drive;
  vsx_module_param_int* time_source; // 0 or 1
  vsx_module_param_int* drive_type; // 0 or 1
  vsx_module_param_int* behaviour;
  
  // out
  vsx_module_param_float* result1;

  // internal
  float time;
  float vtime, dtime;
  float prev_trig_val; 
  float prev_trig_val_reverse; 
  int trigger_state;
  vsx_sequence seq_int;
  float sequence_cache[8192];
public:
  
  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;oscillators;float_sequencer";
    info->description = "";
    info->in_param_spec = 
      "float_sequence:sequence,"
      "length:float,"
      "options:complex"
      "{"
      "  behaviour:enum?oscillating|trigger|trigger_pingpong,"
      "  time_source:enum?operating_system|sequence,"
      "  trigger:float,"
      "  trigger_reverse:float,"
      "  drive_type:enum?time_internal_relative|external,"
      "  drive:float"
      "}"
      ;
    info->out_param_spec = "float:float";
    
    info->component_class = "parameters";
  }


  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    // TODO:
    loading_done = true;
    prev_trig_val = 0.0f;
    trigger_state = 0;
    float_sequence = (vsx_module_param_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_SEQUENCE,"float_sequence"); 
    float_sequence->set(seq_int);
    behaviour = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"behaviour");
    trigger = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"trigger");
    trigger_reverse = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"trigger_reverse");
    length = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "length");
    length->set(1.0f);
    drive = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"drive");
    time_source = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"time_source");
    drive_type = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"drive_type");
    
    result1 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"float");
    result1->set(0);
  }

  void calc_cache() {
    if (float_sequence->updates)
    {
      seq_int = float_sequence->get();
      float_sequence->updates = 0;
      seq_int.reset();
      for (int i = 0; i < 8192; ++i) {
        sequence_cache[i] = seq_int.execute(1.0f/8192.0f);
      }
    }
  }
  
  float delta_multiplier;
  
  void run() {
    if (time_source->get() == 0) {
      vtime = engine->real_vtime;
      dtime = engine->real_dtime;
    } else {
      vtime = engine->vtime;
      dtime = engine->dtime;
    }
    switch (drive_type->get()) {
      case 0:
        if (dtime + vtime == 0) {
          time = 0;
        } else
        time +=  dtime;
        // time += engine->dtime*freq->get();
        //printf("osc: %f %f freq %f\n",time, engine->vtime,freq->get());
        break;
      case 1: time = drive->get();
    }
    if (time < 0.0f) time = 0.0f;
    float i_time = 0.0f;
    switch (behaviour->get()) {
      case 0:
        i_time = time = (float)fmod(time, length->get() );
        break;
      case 1:
        i_time = time;
        if (i_time > length->get()) i_time = length->get();
        if (prev_trig_val <= 0.0f && trigger->get() > 0.0f)
        {
          time = 0.0f;
        }
        break;
      case 2:
          if (time > length->get()) {
            time = length->get();
            if (trigger_state == 1) trigger_state = 2;
            if (trigger_state == 3) trigger_state = 0;
          }
          if (prev_trig_val <= 0.0f && trigger->get() > 0.0f)
          {
            time = 0.0f;
            if (trigger_state == 0) trigger_state = 1;
            else
            if (trigger_state == 2) trigger_state = 3;
            else
            if (trigger_state == 3) trigger_state = 0;
          }
        if (trigger_state == 1 || trigger_state == 2)
        {
          i_time = time;
        }
        if (trigger_state == 3)
        {
          i_time = length->get() - time;
        }
        prev_trig_val = trigger->get();
        break;
    }
    //printf("%d %f\n", trigger_state,time);
    //if (time > length->get()) time = length->get();
    float tt = i_time / length->get();
    calc_cache();
    result1->set(sequence_cache[(int)round(8191.0f*tt)]);    
  }
};

#if BUILDING_DLL
vsx_module* create_new_module(unsigned long module) {
  switch (module) {
    case 0: return (vsx_module*)new vsx_module_plugin_maths_oscillator;
    case 1: return (vsx_module*)new vsx_module_plugin_maths_oscillators_float_sequencer;
  }
  return 0;
}

void destroy_module(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (vsx_module_plugin_maths_oscillator*)m; break;
    case 1: delete (vsx_module_plugin_maths_oscillators_float_sequencer*)m; break;
  }
}

unsigned long get_num_modules() {
  // we have only one module. it's id is 0
  return 2;
}  
#endif
