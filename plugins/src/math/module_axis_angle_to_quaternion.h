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


class module_axis_angle_to_quaternion : public vsx_module
{
  // in
  vsx_module_param_float3* axis;
  vsx_module_param_float* angle;

  // out
  vsx_module_param_quaternion* result;

  // internal
  vsx_quaternion<> q;

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier = "maths;arithmetics;quaternion;axis_angle_to_quaternion";
    info->description =
      "Converts rotation around\n"
      "an axis with an angle\n"
      "into a quaternion."
    ;

    info->in_param_spec =
      "axis:float3,"
      "angle:float"
      ;
    info->out_param_spec = "result:quaternion";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    axis = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"axis");
    axis->set( 0.0f, 0 );
    axis->set( 0.0f, 1 );
    axis->set( 1.0f, 2 );

    angle = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"angle");
    angle->set( 0.0f );

    //--------------------------------------------------------------------------------------------------

    result = (vsx_module_param_quaternion*)out_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"result");
    result->set( 0.0, 0 );
    result->set( 0.0, 1 );
    result->set( 0.0, 2 );
    result->set( 1.0, 3 );
  }

  void run()
  {
    vsx_vector3<> i_axis(
          axis->get(0),
          axis->get(1),
          axis->get(2)
          );

    // make sure vector has length 1
    i_axis.normalize();

    float i_angle = angle->get();
    q.from_axis_angle( i_axis, i_angle);
    result->set(q.x, 0);
    result->set(q.y, 1);
    result->set(q.z, 2);
    result->set(q.w, 3);
  }

};
