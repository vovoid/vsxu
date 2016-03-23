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


class module_bool_and : public vsx_module {
  // in
  vsx_module_param_float* float_a;
  vsx_module_param_float* float_b;

  // out
  vsx_module_param_float* result_float;

  // internal

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "maths;arithmetics;boolean;and";

    info->description =
      "boolean and:\n"
      "result = a and b\n"
      "the logic operation and:\n"
      "1 and 0 = 0\n"
      "0 and 1 = 0\n"
      "0 and 0 = 0\n"
      "1 and 1 = 1\n"
    ;

    info->in_param_spec =
      "a:float,"
      "b:float"
    ;

    info->out_param_spec =
      "result_float:float";

    info->component_class =
      "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    float_a = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"a");
    float_b = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"b");

    //--------------------------------------------------------------------------------------------------

    result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
    result_float->set(0);
  }

  void run()
  {
    result_float->set(
          (float)(round(float_a->get()) && round(float_b->get()))
          );
  }
};
