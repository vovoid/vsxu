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


class module_quaternion_rotation_accumulator_2d : public vsx_module {
  // in
  vsx_module_param_float* param_x;
  vsx_module_param_float* param_y;

  // out
  vsx_module_param_quaternion* result_quat;

  // internal
  vsx_quaternion<> q1,q_out;

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "maths;accumulators;quat_rotation_accum_2d";

    info->description =
      "It's like the rotation sphere,\n"
      "you can rotate it step by step\n"
      "by x or y"
    ;

    info->out_param_spec =
      "result_quat:quaternion";

    info->in_param_spec =
        "param_x:float,"
        "param_y:float"
    ;

    info->component_class =
      "parameters";
  }


  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    param_x = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param_x");
    param_y = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param_y");
    q_out.x = q_out.y = q_out.z = 0.0f;
    q_out.w = 1.0f;

    //--------------------------------------------------------------------------------------------------

    result_quat = (vsx_module_param_quaternion*)out_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"result_quat");
    result_quat->set(0.0f,0);
    result_quat->set(0.0f,1);
    result_quat->set(0.0f,2);
    result_quat->set(1.0f,3);
  }

  void run()
  {
    q1.x = 0.0f;
    q1.w = 1.0f;
    q1.y = param_x->get();
    q1.z = 0.0f;
    q1.normalize();
    q_out.mul(q_out, q1);

    q1.x = 0.0f;
    q1.w = 1.0f;
    q1.y = 0.0f;
    q1.z = param_y->get();
    q1.normalize();
    q_out.mul(q_out, q1);

    result_quat->set(q_out.x,0);
    result_quat->set(q_out.y,1);
    result_quat->set(q_out.z,2);
    result_quat->set(q_out.w,3);
  }

};
