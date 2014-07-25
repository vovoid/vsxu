/**
* Written by Alastair Cota for:
* 
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

#include <string>
#include <sstream>

#define SEQ_RESOLUTION 8192

class module_float_selector : public vsx_module
{
  // in
  vsx_module_param_float* index;
  vsx_module_param_int* inputs;
  std::vector<vsx_module_param_float*> float_x;
  vsx_module_param_int* wrap;
  vsx_module_param_int* interpolation;
  vsx_module_param_sequence* sequence;
  vsx_module_param_int* reverse;
  vsx_module_param_int* reset_seq_to_default;

  // out
  vsx_module_param_float* result;

  // internal
  int i_prev_inputs;
  int i_curr_inputs;  

  float i_index;
  int i_index_x;
  bool i_underRange;
  bool i_overRange;

  int i_index_x0;
  int i_index_x1;
  float i_value_y0;
  float i_value_y1;
  
  int i_wrap;
  int i_interpolation;
  vsx_sequence i_sequence;
  vsx_sequence i_seq_default;
  vsx_array<float> i_sequence_data;
  long i_seq_index;
  int i_reverse;
  int i_reset_seq_to_default;
 
  std::stringstream i_paramString;
  std::stringstream i_paramName;
  vsx_string i_in_param_string;
  vsx_string i_out_param_string;
  bool i_am_ready;

public:
  
  bool init()
  {
    i_am_ready = false;

    i_prev_inputs = 2; // "3"
    i_curr_inputs = 2;
    
    i_index = 0.0;
    i_index_x = 0;
    i_underRange = false;
    i_overRange = false;

    i_index_x0 = 0;
    i_index_x1 = 1;
    i_value_y0 = 0.0;
    i_value_y1 = 0.0;
    
    i_wrap = 2; // Wrap
    i_interpolation = 0; //No Interpolation
    i_reverse = 2; //Autoreverse Normal
    i_reset_seq_to_default = -1;

    i_in_param_string = "";
    i_out_param_string = "";
    
    return true;
  }
  
  void module_info(vsx_module_info* info)
  {
    info->identifier =
      "selectors;float_selector";

    info->description =
      "[result] is equal to the\n"
      "[float_x] chosen by [index]\n"
      "\n"
      "The number of inputs is\n"
      "controlled by [inputs]\n"
      "\n"
      "Linear and Sequence\n"
      "Interpolation options\n"
      "are available in [options]\n"
      "\n";

    info->out_param_spec =
      "result:float";

    info->in_param_spec =
      "index:float,"
      "inputs:enum?1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16,"
      + i_in_param_string +
      "options:complex{"
        "wrap:enum?None_Zero|None_Freeze|Wrap,"
        "interpolation:enum?None|Linear|Sequence,"
        "sequence:sequence,"
        "reverse:enum?Off|On|Auto_Normal|Auto_Inverted,"
        "reset_seq_to_default:enum?ok}";

    info->component_class = "parameters";

    info->output = 1;
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    redeclare_in=true;
  }

  void redeclare_in_params(vsx_module_param_list& in_parameters)
  {
    loading_done = true;

    index = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "index");
    index->set(i_index);
    inputs = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "inputs");
    inputs->set(i_prev_inputs);
    
    float_x.clear();
    i_paramString.str("");
    i_paramString << "float_x:complex{";

    for(int i = 0; i <= i_prev_inputs; ++i)
    {
        if(i > 0) i_paramString << ",";
        i_paramName.str("");
        i_paramName << "float_" << i;
        i_paramString << i_paramName.str().c_str() << ":float";

        float_x.push_back((vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, i_paramName.str().c_str()));
        float_x[i]->set(0.0);
    }

    i_paramString << "},";
    i_in_param_string = i_paramString.str().c_str();
    
    wrap = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "wrap");
    wrap->set(i_wrap);
    interpolation = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "interpolation");
    interpolation->set(i_interpolation);
    sequence = (vsx_module_param_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_SEQUENCE, "sequence");
    sequence->set(i_sequence);
    reverse = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "reverse");
    reverse->set(i_reverse);
    reset_seq_to_default = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "reset_seq_to_default");
    reset_seq_to_default->set(i_reset_seq_to_default);
    
    redeclare_out=true;
  }

  void redeclare_out_params(vsx_module_param_list& out_parameters)
  {
    result = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result");
    result->set(0.0);    

    i_am_ready = true;
  }

  void run()
  {
    i_curr_inputs = inputs->get();
    if(i_prev_inputs != i_curr_inputs) //Update Number Of Inputs
    {
      i_am_ready = false;
      i_curr_inputs = CLAMP(i_curr_inputs, 0, 15);
      i_prev_inputs = i_curr_inputs;
      redeclare_in = true;
    }
    
    i_reset_seq_to_default = reset_seq_to_default->get();
    if(i_reset_seq_to_default == 0) //Reset Sequence To Default
    {
      i_sequence = i_seq_default;
      sequence->set(i_sequence);
      reset_seq_to_default->set(-1);
      i_reset_seq_to_default = -1;
    }

    if(i_am_ready)
    {
      i_wrap = wrap->get();
      i_interpolation = interpolation->get();
      if(i_interpolation > 0) //Interpolation
      {
        if(i_wrap == 2) //Wrap
        {
          i_index = FLOAT_MOD(index->get(), (float)(i_prev_inputs + 1));
          i_index_x = (int)i_index;
          i_index_x0 = i_index_x % (i_prev_inputs + 1);
          i_index_x1 = i_index_x0 + 1;
          i_value_y0 = float_x[i_index_x0]->get();
          i_value_y1 = (i_index_x1 > i_prev_inputs) ? float_x[0]->get()
                     : float_x[i_index_x1]->get();
        }
        else if(i_wrap == 1) //No Wrap - Freeze Ends
        {
          i_index = CLAMP(index->get(), 0.0, (float)i_prev_inputs);
          i_index_x = (int)i_index;
          i_index_x0 = CLAMP(i_index_x, 0, i_prev_inputs);
          i_index_x1 = CLAMP(i_index_x + 1, 0, i_prev_inputs);
          i_value_y0 = float_x[i_index_x0]->get();
          i_value_y1 = float_x[i_index_x1]->get();
        }
        else //No Wrap - Zero Ends
        {
          i_index = CLAMP(index->get(), -1.0, (float)(i_prev_inputs + 1));
          i_index_x = (int)i_index;
          i_underRange = i_index < 0.0;
          i_overRange = i_index > (float)i_prev_inputs;
          i_index_x0 = (i_underRange) ? -1
                     :((i_overRange) ? i_prev_inputs
                     :  CLAMP(i_index_x, -1, i_prev_inputs + 1));
          i_index_x1 = (i_underRange) ? 0
                     :((i_overRange) ? i_prev_inputs + 1
                     :  CLAMP(i_index_x + 1, 0, i_prev_inputs + 2));
          i_value_y0 = (i_underRange) ? 0.0
                     :((i_overRange) ? float_x[i_prev_inputs]->get()
                     :  float_x[i_index_x0]->get());
          i_value_y1 = (i_underRange) ? float_x[0]->get()
                     :((i_overRange) ? 0.0
                     :  float_x[i_index_x1]->get());
        }
        
        //Linear Interpolation
        if(i_interpolation == 1)
        {
          result->set(FLOAT_INTERPOLATE(i_value_y0,
                                        i_value_y1,
                                        i_index,
                                        (float)i_index_x0,
                                        (float)i_index_x1));
        }
        //Sequence Interpolation
        else if(i_interpolation == 2)
        {
          i_sequence = sequence->get();
          sequence->updates = 0;
          i_sequence.reset();

          for(int i = 0; i < SEQ_RESOLUTION; ++i) //Cache Sequence Data
          {
            i_sequence_data[i] = i_sequence.execute(1.0f / (float)(SEQ_RESOLUTION - 1));
          }
          
          i_seq_index = ((i_index - (float)i_index_x0) 
                      / (float)(i_index_x1 - i_index_x0))
                      * (float)SEQ_RESOLUTION;
          
          if(i_wrap == 0) //Bugfix for No Wrap - Zero Ends
          {
            i_value_y0 = (i_index_x == i_prev_inputs + 1) ? 0.0 : i_value_y0;
            i_value_y1 = (i_index_x == -1) ? 0.0 : i_value_y1;
          }
          
          i_reverse = reverse->get();
          switch(i_reverse)
          {
            case(0): //Reverse Off
              result->set(FLOAT_INTERPOLATE(i_value_y0,
                                            i_value_y1,
                                     (float)i_index_x0 + i_sequence_data[i_seq_index],
                                     (float)i_index_x0,
                                     (float)i_index_x1));
            break;
            case(1): //Reverse On
              result->set(FLOAT_INTERPOLATE(i_value_y1,
                                            i_value_y0,
                                     (float)i_index_x1 - i_sequence_data[i_seq_index],
                                     (float)i_index_x0,
                                     (float)i_index_x1));
            break;
            case(2): //Autoreverse - Normal
              result->set(FLOAT_INTERPOLATE((i_value_y0 < i_value_y1) ? i_value_y0 : i_value_y1,
                                            (i_value_y0 < i_value_y1) ? i_value_y1 : i_value_y0,
                                            (i_value_y0 < i_value_y1) 
                                    ? (float)i_index_x0 + i_sequence_data[i_seq_index]
                                    : (float)i_index_x1 - i_sequence_data[i_seq_index],
                                      (float)i_index_x0,
                                      (float)i_index_x1));
            break;
            case(3): //Autoreverse - Inverted
              result->set(FLOAT_INTERPOLATE((i_value_y0 < i_value_y1) ? i_value_y1 : i_value_y0,
                                            (i_value_y0 < i_value_y1) ? i_value_y0 : i_value_y1,
                                            (i_value_y0 < i_value_y1)
                                    ? (float)i_index_x1 - i_sequence_data[i_seq_index]
                                    : (float)i_index_x0 + i_sequence_data[i_seq_index],
                                      (float)i_index_x0,
                                      (float)i_index_x1));
            break;
          }
        }
      }
      else //No Interpolation
      {
        if(i_wrap == 2) //Wrap
        {
          i_index = FLOAT_MOD(index->get(), (float)(i_prev_inputs + 1));
          i_index_x0 = (i_index > (float)i_prev_inputs) 
                     ? ((i_index > ((float)i_prev_inputs + 0.5)) ? 0 : i_prev_inputs)
                     : (int)(i_index + 0.5);
          i_value_y0 = float_x[i_index_x0]->get();
        }
        else if(i_wrap == 1) //No Wrap - Freeze Ends
        {
          i_index = CLAMP(index->get() + 0.5, 0.5, (float)(i_prev_inputs) + 0.5);
          i_index_x0 = CLAMP((int)i_index, 0, i_prev_inputs);
          i_value_y0 = float_x[i_index_x0]->get();
        }
        else //No Wrap - Zero Ends
        {
          i_index = CLAMP(index->get() + 0.5, -0.5, (float)(i_prev_inputs) + 1.5);
          i_index_x0 = CLAMP((int)i_index, 0, i_prev_inputs);
          i_value_y0 = ((i_index < -0.0) || (i_index > (float)(i_prev_inputs + 1)) ? 0.0
                     : float_x[i_index_x0]->get());
        }
        result->set(i_value_y0);
      }
    }
  }
};
