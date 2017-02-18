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


class module_float_array_average : public vsx_module
{
  // in
  vsx_module_param_float_array* float_in;
  vsx_module_param_float* start;
  vsx_module_param_float* end;

  // out
  vsx_module_param_float* result_float;

  // internal
  vsx_float_array* my_array;

public:



  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "maths;array;float_array_average";

    info->description =
      "Calculates the average of all\n"
      "values in the array"
    ;

    info->in_param_spec =
      "float_in:float_array,"
      "start:float,end:float"
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

    float_in = (vsx_module_param_float_array*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"float_in");

    start = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"start");
    start->set(0);

    end = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"end");
    end->set(0);

    //--------------------------------------------------------------------------------------------------

    result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
    result_float->set(0);
  }

  void run()
  {
    my_array = float_in->get_addr();
    if (!my_array) return;
    if ( ! ( end->get() > start->get())) return;

    float temp = 0;
    float divisor = 0;

    float x = (float)ceil(start->get());
    float x_e = (float) floor(end->get());
    float x_e_f = end->get();
    if (x_e > (*(my_array->data)).size()-1)
    {
      x_e = (float)((*(my_array->data)).size() - 1);
      x_e_f = x_e;
    }
    divisor += x - start->get();
    temp += (*(my_array->data))[(int)x-1]*(x - start->get());
    for (; x < x_e; ++x)
    {
      temp += (*(my_array->data))[(int)x];
    }
    divisor += x_e_f - x_e;
    temp += (*(my_array->data))[(int)x_e+1]*(x_e_f - x_e);
    temp /= divisor;
    result_float->set(temp);
  }
};
