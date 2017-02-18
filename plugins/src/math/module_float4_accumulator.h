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


class module_float4_accumulator : public vsx_module
{
  // in
  vsx_module_param_float4* float4_in;
  vsx_module_param_int* reset;

  // out
  vsx_module_param_float4* result_float4;

  // internal
  float value[4];

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "maths;accumulators;float4_accumulator";

    info->description =
      "accumulator - float4\n"
      "result += in\n"
      "\n"
      "adds the value in float4_in\n"
      "to its current value once\n"
      "per frame"
    ;

    info->in_param_spec =
      "float4_in:float4,"
      "reset:enum?ok"
    ;

    info->out_param_spec =
      "result_float4:float4";

    info->component_class =
      "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    value[0] = 0;
    value[1] = 0;
    value[2] = 0;
    value[3] = 0;

    //--------------------------------------------------------------------------------------------------

    float4_in = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"float4_in");

    reset = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"reset");
    reset->set(-1);

    //--------------------------------------------------------------------------------------------------

    result_float4 = (vsx_module_param_float4*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"result_float4");
    result_float4->set(0,0);
    result_float4->set(0,1);
    result_float4->set(0,2);
    result_float4->set(0,3);
  }

  void run()
  {
    if (reset->get() == 0)
    {
      reset->set(-1);
      value[0] = 0;
      value[1] = 0;
      value[2] = 0;
      value[3] = 0;
    }

    value[0] += float4_in->get(0);
    value[1] += float4_in->get(1);
    value[2] += float4_in->get(2);
    value[3] += float4_in->get(3);

    result_float4->set(value[0],0);
    result_float4->set(value[1],1);
    result_float4->set(value[2],2);
    result_float4->set(value[3],3);
  }
};


