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


class module_quaternion_dummy : public vsx_module {
  // in
  vsx_module_param_quaternion* quaternion_in;

  // out
  vsx_module_param_quaternion* result_quaternion;

  // internal

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "maths;dummies;quaternion_dummy"
      "||"
      "dummies;quaternion_dummy"
    ;

    info->description =
      "This is only to gather connections\n"
      " - inside a macro for instance if \n"
      "you have many components which need\n"
      "to read the same value but you don't\n"
      "want 10 aliases going out.\n[quatin]\n"
      "is copied to [result_quat]."
    ;

    info->out_param_spec =
      "out_quat:quaternion";

    info->in_param_spec =
      "quat_in:quaternion";

    info->component_class =
      "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    quaternion_in = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"quat_in");
    quaternion_in->set(0.0f,0);
    quaternion_in->set(0.0f,1);
    quaternion_in->set(0.0f,2);
    quaternion_in->set(1.0f,3);

    //--------------------------------------------------------------------------------------------------

    result_quaternion = (vsx_module_param_quaternion*)out_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"out_quat");
    result_quaternion->set(0.0f,0);
    result_quaternion->set(0.0f,1);
    result_quaternion->set(0.0f,2);
    result_quaternion->set(1.0f,3);
  }

  void run()
  {
    result_quaternion->set(quaternion_in->get(0),0);
    result_quaternion->set(quaternion_in->get(1),1);
    result_quaternion->set(quaternion_in->get(2),2);
    result_quaternion->set(quaternion_in->get(3),3);
  }
};
