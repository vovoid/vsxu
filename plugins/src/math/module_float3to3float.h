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


class module_float3to3float : public vsx_module
{
  // in
  vsx_module_param_float3* float3_in;

  // out
  vsx_module_param_float* a;
  vsx_module_param_float* b;
  vsx_module_param_float* c;

  // internal

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "maths;converters;float3to3float";

    info->description =
      "Data type conversion\n"
      "  float3 to 3 x float values."
    ;

    info->in_param_spec =
      "float3_in:float3";

    info->out_param_spec =
      "a:float,"
      "b:float,"
      "c:float"
    ;

    info->component_class =
      "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    float3_in = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"float3_in");

    //--------------------------------------------------------------------------------------------------

    a = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"a");
    a->set(0.0f);
    b = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"b");
    b->set(0.0f);
    c = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"c");
    c->set(0.0f);
  }

  void run()
  {
    a->set(float3_in->get(0));
    b->set(float3_in->get(1));
    c->set(float3_in->get(2));
  }
};
