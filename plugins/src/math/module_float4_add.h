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


class module_float4_add : public vsx_module
{
  // in
  vsx_module_param_float4* param1;
  vsx_module_param_float4* param2;

  // out
  vsx_module_param_float4* result4;

  // internal

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "!maths;arithmetics;vector;float4_add"
      "||"
      "maths;arithmetics;float4;float4_add"
    ;

    info->description =
      "Adds 2 float4 values together.";

    info->out_param_spec =
      "result_float4:float4";

    info->in_param_spec = ""
      "param1:float4,"
      "param2:float4";

    info->component_class =
      "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    param1 = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"param1");
    param2 = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"param2");

    //--------------------------------------------------------------------------------------------------

    result4 = (vsx_module_param_float4*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"result_float4");
    result4->set(0,0);
    result4->set(0,1);
    result4->set(0,2);
    result4->set(0,3);
  }

  void run()
  {
    result4->set(param1->get(0)+param2->get(0),0);
    result4->set(param1->get(1)+param2->get(1),1);
    result4->set(param1->get(2)+param2->get(2),2);
    result4->set(param1->get(3)+param2->get(3),3);
  }
};
