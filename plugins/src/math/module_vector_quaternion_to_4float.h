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


class module_vector_quaternion_to_4float : public vsx_module
{
  // in
  vsx_module_param_quaternion* in_quat;

  // out
  vsx_module_param_float* param1;
  vsx_module_param_float* param2;
  vsx_module_param_float* param3;
  vsx_module_param_float* param4;

  // internal

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier = "maths;converters;quaternion_to_4float";

    info->description =
      "takes quaternion\n"
      "outputs 4 floats\n"
    ;

    info->in_param_spec =
      "in_quat:quaternion";

    info->out_param_spec =
      "param1:float,"
      "param2:float,"
      "param3:float,"
      "param4:float"
    ;

    info->component_class =
      "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    in_quat = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"in_quat");
    in_quat->set(0.0f,0);
    in_quat->set(0.0f,1);
    in_quat->set(0.0f,2);
    in_quat->set(1.0f,3);

    //--------------------------------------------------------------------------------------------------

    param1 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param1");
    param1->set(0.0f);

    param2 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param2");
    param2->set(0.0f);

    param3 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param3");
    param3->set(0.0f);

    param4 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param4");
    param4->set(1.0f);

  }

  void run()
  {
    param1->set(in_quat->get(0));
    param2->set(in_quat->get(1));
    param3->set(in_quat->get(2));
    param4->set(in_quat->get(3));
  }

};
