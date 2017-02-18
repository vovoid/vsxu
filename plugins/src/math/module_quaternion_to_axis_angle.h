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


class module_quaternion_to_axis_angle : public vsx_module
{
  // in
  vsx_module_param_quaternion* source_quaternion;

  // out
  vsx_module_param_float3* result_axis;
  vsx_module_param_float* result_angle;

  //internal

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier = "maths;arithmetics;quaternion;quaternion_to_axis_angle";
    info->description =
      "Converts a quaternion \n"
      "(rotation) to axis angle.\n"
    ;

    info->in_param_spec =
      "source_quaternion:quaternion"
    ;
    info->out_param_spec =
      "result_axis:float3,"
      "result_angle:float"
    ;
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    source_quaternion = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"source_quaternion");
    source_quaternion ->set( 0.0, 0 );
    source_quaternion ->set( 0.0, 1 );
    source_quaternion ->set( 0.0, 2 );
    source_quaternion ->set( 1.0, 3 );

    //--------------------------------------------------------------------------------------------------

    result_axis = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"result_axis");
    result_axis->set( 0.0f, 0 );
    result_axis->set( 0.0f, 1 );
    result_axis->set( 1.0f, 2 );

    result_angle = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_angle");
    result_angle->set( 0.0f );

  }

  void run()
  {
    vsx_quaternion<> i_quat(
          source_quaternion->get(0),
          source_quaternion->get(1),
          source_quaternion->get(2),
          source_quaternion->get(3)
          );
    // make sure source quaternion is normalized
    i_quat.normalize();

    float r_angle;
    vsx_vector3<> r_axis;
    i_quat.to_axis_angle( r_axis, r_angle);

    result_axis->set( r_axis.x, 0);
    result_axis->set( r_axis.y, 1);
    result_axis->set( r_axis.z, 2);

    result_angle->set( r_angle );
  }

};
