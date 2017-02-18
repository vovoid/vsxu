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


class module_float_clamp : public vsx_module
{
  // in
  vsx_module_param_float* float_in;
  vsx_module_param_float* clamp_value_low;
  vsx_module_param_float* clamp_value_high;

  vsx_module_param_int* type;

  // out
  vsx_module_param_float* result_float;

  // internal

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "maths;limiters;float_clamp";

    info->description =
      "If [value] is larger than\n"
      "  [high], set to [high]\n"
      "or if value is lower than\n"
      "  [low], set to [low]\n"
    ;

    info->out_param_spec =
      "result_float:float";

    info->in_param_spec =
      "value:float,"
      "low:float,"
      "high:float,"
    ;

    info->component_class =
      "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    float_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"value");
    float_in->set(0.0f);

    clamp_value_low = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"low");
    clamp_value_low->set(0.0f);

    clamp_value_high = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"high");
    clamp_value_high->set(0.0f);

    type = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"type");

    //--------------------------------------------------------------------------------------------------

    result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
    result_float->set(0.0f);
  }

  void run()
  {
      if (float_in->get() < clamp_value_low->get())
      {
        result_float->set(clamp_value_low->get());
        return;
      }

      if (float_in->get() > clamp_value_high->get())
      {
        result_float->set(clamp_value_high->get());
        return;
      }
      result_float->set( float_in->get() );
  }
};
