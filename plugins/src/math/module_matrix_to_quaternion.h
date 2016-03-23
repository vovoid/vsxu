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


class module_matrix_to_quaternion: public vsx_module
{
  // in
  vsx_module_param_matrix* source_matrix;

  // out
  vsx_module_param_quaternion* result_quaternion;

  //internal
  vsx_matrix<> m;

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier = "maths;converters;matrix;matrix_to_quaternion";
    info->description =
      "Converts a matrix\n"
      "to quaternion.\n"
    ;

    info->in_param_spec =
      "source_matrix:matrix"
    ;
    info->out_param_spec =
      "result_quaternion:quaternion"
    ;
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    source_matrix = (vsx_module_param_matrix*)in_parameters.create(VSX_MODULE_PARAM_ID_MATRIX,"source_matrix");
    source_matrix->set(m);


    //--------------------------------------------------------------------------------------------------

    result_quaternion = (vsx_module_param_quaternion*)out_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"result_quaternion");
    result_quaternion ->set( 0.0, 0 );
    result_quaternion ->set( 0.0, 1 );
    result_quaternion ->set( 0.0, 2 );
    result_quaternion ->set( 1.0, 3 );

  }

  void run()
  {
    vsx_quaternion<> res_quat;

    m = source_matrix->get();
    res_quat.from_matrix(&m);
    result_quaternion->set(res_quat.x, 0);
    result_quaternion->set(res_quat.y, 1);
    result_quaternion->set(res_quat.z, 2);
    result_quaternion->set(res_quat.w, 3);

  }

};
