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

#include <math/vsx_sequence.h>

class module_plugin_maths_oscillators_string_sequencer : public vsx_module
{
public:
  // in
  vsx_module_param_string_sequence* string_sequence;
  vsx_module_param_float* length;
  vsx_module_param_float* trigger;
  vsx_module_param_float* trigger_reverse;
  vsx_module_param_float* drive;
  vsx_module_param_int* time_source; // 0 or 1
  vsx_module_param_int* drive_type; // 0 or 1
  vsx_module_param_int* behaviour;

  // out
  vsx_module_param_string* result1;

  // internal
  float time;
  float vtime, dtime;
  float prev_trig_val;
  float prev_trig_val_reverse;
  int trigger_state;
  vsx::sequence::channel< vsx::sequence::value_string > seq_int;
  float delta_multiplier;
  float prev_time;


  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "maths;oscillators;string_sequencer";

    info->description = "";

    info->in_param_spec =
      "string_sequence:string_sequence,"
      "length:float,"
      "options:complex"
      "{"
        "behaviour:enum?oscillating|trigger|trigger_pingpong|trigger_sync,"
        "time_source:enum?operating_system|sequence,"
        "trigger:float,"
        "trigger_reverse:float,"
        "drive_type:enum?time_internal_relative|external,"
        "drive:float"
      "}"
    ;

    info->out_param_spec =
      "result:string";

    info->component_class =
      "parameters";
  }


  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    prev_trig_val = 0.0f;
    trigger_state = 0;
    dtime = 0.0f;
    prev_time = 0.0f;

    string_sequence = (vsx_module_param_string_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_STRING_SEQUENCE,"string_sequence");
    string_sequence->set(seq_int);

    behaviour = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"behaviour");

    trigger = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"trigger");

    trigger_reverse = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"trigger_reverse");

    length = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "length");
    length->set(1.0f);

    drive = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"drive");

    time_source = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"time_source");

    drive_type = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"drive_type");

    result1 = (vsx_module_param_string*)out_parameters.create(VSX_MODULE_PARAM_ID_STRING,"result");
    result1->set("");
  }

  inline void calc_cache()
  {
    if (!string_sequence->updates)
      return;

    seq_int = string_sequence->get();
    seq_int.reset();

    string_sequence->updates = 0;

  }


  void run()
  {

    switch (time_source->get())
    {

      // operating_system
      case 0:
        vtime = engine_state->real_vtime;
        dtime = engine_state->real_dtime;
      break;

      // master sequencer
      case 1:
        vtime = engine_state->vtime;
        dtime = engine_state->dtime;
      break;

    }



    switch (drive_type->get())
    {

      // time_internal_relative
      case 0:
        if (dtime + vtime == 0)
        {
          time = 0;
        } else
        {
          time +=  dtime;
        }

        break;

      // external
      case 1:
        time = drive->get();
        if (prev_time > 0.0f)
          dtime = drive->get() - prev_time;
        prev_time = time;
    }



    if (time < 0.0f)
    {
      time = 0.0f;
    }
    float i_time = 0.0f;

    switch (behaviour->get())
    {

      // oscillating
      case 0:
        i_time = time = (float)fmod(time, length->get() );
        break;


      // trigger
      case 1:
        i_time = time;

        if (i_time > length->get())
          i_time = length->get();

        if (prev_trig_val <= 0.00001f && trigger->get() > 0.0f)
        {
          time = 0.0f;
          i_time = 0.0f;
        }

        prev_trig_val = trigger->get();

        break;


      // trigger_pingpong
      case 2:
        if (time > length->get())
        {
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

      // trigger_sync
      case 3:

        if (trigger->get() > 0.000001)
          i_time = time;

        if (trigger->get() < 0.000001)
        {
          time -= dtime;
          time -= dtime;
          i_time = time;
        }

        if (i_time > length->get())
        {

          time = i_time = length->get();
        }

        if (i_time < 0.0f)
        {
          time = i_time = 0.0f;
        }

      break;
    }

    calc_cache();
    float tt = fmod(i_time / length->get(), length->get());

    result1->set( seq_int.execute_absolute(tt).get_string() );
  }
};
