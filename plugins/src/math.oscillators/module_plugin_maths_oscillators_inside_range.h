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


class module_plugin_maths_oscillators_inside_range : public vsx_module
{
public:
  // in
  vsx_module_param_float* sound_in;
  vsx_module_param_float* range_low;
  vsx_module_param_float* range_high;
  vsx_module_param_float* randomness;

  // out
  vsx_module_param_float* every_beat;
  vsx_module_param_float* random_beat;
  vsx_module_param_float* in_range;

  // internal
  bool is_in_range;



  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "maths;oscillators;inside_range";

    info->description =
      "outputs 1 when Sound levels\n"
      "are inside the min / max range \n"
      "useful for beat detection. \n"
      "randomness must be between 0 and 100\n";

    info->in_param_spec =
      "sound_in:float,"
      "range_low:float,"
      "range_high:float,"
      "randomness:float"
    ;

    info->out_param_spec =
      "every_beat:float,"
      "random_beat:float,"
      "in_range:float"
    ;

    info->component_class =
      "parameters";
  }


  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    is_in_range = false;

    sound_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"sound_in");
    sound_in->set(0);

    range_low = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"range_low");
    range_low->set(0.5f);

    range_high = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"range_high");
    range_high->set(1);

    randomness = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"randomness");
    randomness->set(0.5f);

    every_beat = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"every_beat");
    every_beat->set(0);

    random_beat = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"random_beat");
    random_beat->set(10);

    in_range = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"in_range");
    in_range->set(0);

  }



  void run()
  {
    float current_sound = sound_in->get();

    if (is_in_range)
    {
      //its already pulsed out, so don't pulse again
      every_beat->set(0);

      random_beat->set(0);

      if
      (
          (sound_in->get() <= range_low->get())
          ||
          (sound_in->get() > range_high->get())
      )
      {
        is_in_range = false;
      }
    }
    else
    if
    (
        current_sound > range_low->get()
        &&
        current_sound < range_high->get()
    )
    {
      is_in_range = true;
      every_beat->set(1.0f);
      float random_number = (float)(rand() % 100);
      if (random_number > randomness->get())
      {
        random_beat->set(0.0f);
      }
      else
      {
        random_beat->set(1.0f);
      }
    }

    if (is_in_range)
    {
      in_range->set(1.0f);
    }
    else
    {
      in_range->set(0.0f);
    }
  }
};
