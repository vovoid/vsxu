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


class module_plugin_maths_oscillators_pulse_oscillator : public vsx_module
{
public:
  // in
  vsx_module_param_float* amp;
  vsx_module_param_float* fade_speed;
  vsx_module_param_float* trigger;

  // out
  vsx_module_param_float* result1;

  // internal
  float current_time;
  bool is_pulsing;
  bool pause;
  float current_phase;
  float current_pos;

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "maths;oscillators;pulse_oscillator";

    info->description =
      "Like a normal oscillator,/n"
      "but it pulses to 50% and /n "
      "pauses until the next pulse /n"
    ;

    info->in_param_spec =
      "osc:complex"
      "{"
        "trigger:float,"
        "fade_speed:float,"
        "amp:float,"
      "}"
    ;

    info->out_param_spec =
      "result1:float";

    info->component_class =
      "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {

    loading_done = true;
  trigger = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"trigger");
    amp = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"amp");
    fade_speed = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"fade_speed");
    amp->set(0.5f);
    fade_speed->set(1); // one revolution per second
    current_time = 0;
  is_pulsing = false;
  pause = true;
  current_pos= 0;
  current_phase = 1;

    // amplitude = 0.5
    // phase 0.5
    // frequency 1
    result1 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result1");
    result1->set(0);
  }

  void run() {
  if(pause && (trigger->get() == 0)) {
  result1->set(current_pos);
  } else {

  current_time += fade_speed->get() / 60;
  float frequencyChunk = fade_speed->get() / 60;
  //current_pos = (float)sin((current_time)*PI_FLOAT) * amp->get();
  current_pos = (float)sin((current_time)*PI_FLOAT) * amp->get() + amp->get();

  if ((current_pos > 1- frequencyChunk) && (current_phase == 1))
  {
  current_pos = 1;
  current_phase = 2;
  pause = true;
  result1->set(0.5);

  }
  else if ((current_pos < frequencyChunk ) &&(current_phase == 2) )
  {
  current_pos = 0;
  current_phase = 1;
  pause = true;
  result1->set(0);
  }
  else
  {
    pause = false;
    result1->set(current_pos);
  }
  }
  }
};
