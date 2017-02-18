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


class module_float_smooth : public vsx_module
{
  // in
  vsx_module_param_float* float_in;
  vsx_module_param_float* speed;

  // out
  vsx_module_param_float* result_float;

  // internal
  double internal_value;

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "!maths;interpolation;float_interpolation"
      "||"
      "maths;interpolation;float_smoother"
    ;

    info->description =
      "Smoothens a value over time\n"
      " - same algorith as the knobs/sliders"
    ;

    info->out_param_spec =
      "result_float:float";

    info->in_param_spec =
      "value_in:float,speed:float";

    info->component_class =
      "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    internal_value = 0.0f;

    //--------------------------------------------------------------------------------------------------

    float_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"value_in");
    float_in->set(0.0f);

    speed = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"speed");
    speed->set(1.0f);

    //--------------------------------------------------------------------------------------------------

    result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
    result_float->set(0.0f);
  }

  void run()
  {
    double tt = engine_state->dtime*speed->get();
    if (tt > 1.0) tt = 1.0;

    double temp = (float)(internal_value*(1.0-tt)+(double)float_in->get() * tt);

    internal_value = temp;
    result_float->set((float)internal_value);
  }
};
