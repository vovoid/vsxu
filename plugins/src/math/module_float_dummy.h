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


class module_float_dummy : public vsx_module
{
  // in
  vsx_module_param_float* float_in;

  // out
  vsx_module_param_float* result_float;

  // internal

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "maths;dummies;float_dummy"
      "||"
      "dummies;float_dummy"
    ;

    info->description =
      "[floatin] is copied without modification into\n"
      "[result_float]\n"
      "\n"
      "This module is good for distributing a value you want to\n"
      "set to many components. Or host to code\n"
      "a filter in VSXL. For instance if you\n"
      "have many components in a macro which need\n"
      "the same value but you don't want 10\n"
      "aliases going out, it's a good idea to\n"
      "keep one inside your macro.\n"
    ;

    info->out_param_spec = "result_float:float";

    info->in_param_spec = "float_in:float";

    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    float_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"float_in");
    float_in->set(1);

    //--------------------------------------------------------------------------------------------------

    result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
    result_float->set(1);
  }

  void run()
  {
    result_float->set(float_in->get());
  }

};
