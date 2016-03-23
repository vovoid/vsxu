/**
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Lesser General Public License (LGPL)
*
* VSXu Engine is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU Lesser General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


class module_plugin_maths_oscillator : public vsx_module
{
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

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "maths;oscillators;oscillator";

    info->description =
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
        "osc_type:enum?"
        "noise|sin|saw|square|triangle|quadratic,"
        "freq:float,"
        "amp:float,"
        "ofs:float,"
        "phase:float?help=`Not often useful,\nbut it shifts time back and forth.`,"
      "}"
      ","
      "options:complex"
      "{"
        "time_source:enum?operating_system|sequence,"
        "drive_type:enum?time_internal_absolute|time_internal_relative|external,"
        "drive:float"
      "}"
    ;

    info->out_param_spec =
      "float:float";

    info->component_class =
      "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    time = 0;
    rand_time_accum = 0.0f;
    loading_done = true;

    osc_type = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "osc_type");
    osc_type->set(1); // sin

    amp = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"amp");
    amp->set(0.5f);

    ofs = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"ofs");
    ofs->set(0.5f);

    phase = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"phase");
    phase->set(0);

    freq = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"freq");
    freq->set(1); // one revolution per second

    drive = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"drive");
    drive->set(1);

    time_source = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"time_source");
    time_source->set(0);

    drive_type = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"drive_type");
    drive_type->set(1);


    result1 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"float");
    result1->set(0);
  }

  void run()
  {
    if (time_source->get() == 0)
    {
      vtime = engine_state->real_vtime;
      dtime = engine_state->real_dtime;
    } else
    {
      vtime = engine_state->vtime;
      dtime = engine_state->dtime;
    }
    switch (drive_type->get())
    {
      case 0:
        time = vtime*freq->get();
        break;
      case 1:
        if (dtime + vtime == 0)
        {
          time = 0;
        } else
        {
          time += dtime*freq->get();
        }
        break;
      case 2:
        time = drive->get();
    }

    switch (osc_type->get())
    {
      // noise
      case 0:
        rand_time_accum += dtime;
        if (rand_time_accum > freq->get())
        {
          rand_time_accum = 0.0f;
          result1->set(((rand()/(float)RAND_MAX)-0.5f) * amp->get() + ofs->get());
        }
      break;

      // sin
      case 1:
        result1->set((float)sin((time+phase->get())*PI_FLOAT) * amp->get() + ofs->get());
        //result1->set((float)sin((time+phase->get())*PI) * amp->get() + ofs->get());
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
