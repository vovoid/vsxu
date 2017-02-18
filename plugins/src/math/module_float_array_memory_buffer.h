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

const size_t sizes[] = {2,4,8,16,32,64,128,256,512,1024,2048,4096};

class module_float_array_memory_buffer: public vsx_module
{
  // in
  vsx_module_param_float* float_in;
  vsx_module_param_int* size;

  // out
  vsx_module_param_float_array* result_array;
  vsx_module_param_float* cur_index;

  // internal
  vsx_float_array my_array;

  vsx_ma_vector<float> data;
  int cur_size;
  size_t cur_size_elements;
  size_t index;

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "maths;array;float_array_memory_buffer";

    info->description =
      "Maintains an internal buffer with\n"
      "variable size.\n"
      "\n"
      "Adds one value to this buffer per frame.\n"
      "Can be used to display an oscilloscope-like\n"
      "log of values."
    ;

    info->in_param_spec =
      "float_in:float,"
      "size:enum?2|4|8|16|32|64|128|256|512|1024|2048|4096"
    ;

    info->out_param_spec =
      "result_array:float_array,"
      "cur_index:float"
    ;

    info->component_class =
      "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    index = 0;
    cur_size = 5;
    cur_size_elements = sizes[cur_size];

    // setup data
    my_array.data = &data;
    data.allocate( cur_size_elements );

    //--------------------------------------------------------------------------------------------------

    float_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"float_in");

    size= (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"size");
    size->set(cur_size);

    //--------------------------------------------------------------------------------------------------

    result_array = (vsx_module_param_float_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"result_array");
    result_array->set_p(my_array);

    cur_index = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"cur_index");
    cur_index->set(0);
  }

  void run()
  {
    if (cur_size != size->get())
    {
      cur_size = size->get();
      cur_size_elements = sizes[cur_size];
      // reallocate the buffer
      data.reset_used();
      data.allocate( cur_size_elements );
      data[cur_size_elements-1] = 0;
      data.memory_clear();
      index = 0;
    }

    // add the value to the array
    data[index] = float_in->get();

    // increase index
    index++;

    // handle end of buffer
    if (index >= cur_size_elements)
      index = 0;

    cur_index->set((float)index);

    result_array->set_p(my_array);
  }

};

