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


class module_4float_to_float4 : public vsx_module
{
  // in
  vsx_module_param_float* floata;
  vsx_module_param_float* floatb;
  vsx_module_param_float* floatc;
  vsx_module_param_float* floatd;

  // out
  vsx_module_param_float4* result4;

  // internal

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "maths;converters;4float_to_float4";

    info->description =
      "[floata, floatb, floatc, floatd] are copied and \n"
      "converted into \n"
      "[result_float4] which is a float3\n"
      "\n"
      "This is a simple conversion module.\n"
      "Whenever possible, try to set values\n"
      "with sliders, this is only good if you\n"
      "really need to convert"
    ;

    info->out_param_spec =
      "result_float4:float4";

    info->in_param_spec =
      "floata:float,"
      "floatb:float,"
      "floatc:float,"
      "floatd:float"
    ;

    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    floata = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"floata");
    floatb = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"floatb");
    floatc = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"floatc");
    floatd = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"floatd");
    floata->set(1);
    floatb->set(1);
    floatc->set(1);
    floatd->set(1);

    //--------------------------------------------------------------------------------------------------

    result4 = (vsx_module_param_float4*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"result_float4");
    result4->set(1,0);
    result4->set(1,1);
    result4->set(1,2);
    result4->set(1,3);
  }

  void run()
  {
    result4->set(floata->get(),0);
    result4->set(floatb->get(),1);
    result4->set(floatc->get(),2);
    result4->set(floatd->get(),3);
  }
};
