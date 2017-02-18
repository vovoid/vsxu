/**
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2014
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


class module_float_compare : public vsx_module
{
  // in
  vsx_module_param_float* float_a;
  vsx_module_param_float* float_b;
  vsx_module_param_int* float_operator;

  // out
  vsx_module_param_float* result;

  // internal

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "maths;arithmetics;binary;compare"
    ;

    info->description =
      "[float_a] is compared to [float_b]\n"
      "using logic operator [operator]\n"
      "\n"
      "Result is either 0.0 or 1.0.\n"
      "Equals is faked comparing the \n"
      "difference against 0.00001"
    ;

    info->out_param_spec = "result:float";

    info->in_param_spec =
      "float_a:float,"
      "float_b:float,"
      "operator:enum?equals|larger_than|smaller_than"
    ;

    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    float_a = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"float_a");
    float_a->set(0);
    float_b = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"float_b");
    float_b->set(0);
    float_operator = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "operator");
    float_operator->set(0);

    //--------------------------------------------------------------------------------------------------

    result = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result");
    result->set(0);
  }

  void run()
  {
    float t;
    switch (float_operator->get())
    {
      // equals
      case 0:
        t = fabs(float_b->get() - float_a->get());
        if ( t < 0.00001 )
          result->set(1.0);
        else
          result->set(0.0);
        break;
      // larger_than
      case 1:
        if ( float_b->get() > float_a->get() )
          result->set(1.0);
        else
          result->set(0.0);
        break;
      // smaller_than
      case 2:
        if ( float_b->get() < float_a->get() )
          result->set(1.0);
        else
          result->set(0.0);
        break;
    }
  }
};
