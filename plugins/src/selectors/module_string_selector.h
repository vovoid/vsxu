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

class module_string_selector : public vsx_module
{
//---DATA-STORAGE---------------------------------------------------------------

  // in
  vsx_module_param_float* index;
  vsx_module_param_int* inputs;
  std::vector<vsx_module_param_string*> string_x;
  vsx_module_param_int* wrap;

  // out
  vsx_module_param_string* result;

  // internal
  int i_prev_inputs;
  int i_curr_inputs;  

  float i_index;
  int i_index_x0;
  vsx_string<>i_value_y0;
  int i_wrap;
 
  std::stringstream i_paramString;
  std::stringstream i_paramName;
  vsx_string<>i_in_param_string;

  bool i_am_ready;

//---INITIALISATION-------------------------------------------------------------

public:

  //Initialise Data
  module_string_selector() 
  {
    i_am_ready = false; //Don't do operations if the data isn't ready

    i_prev_inputs = 15; //"16" for loading
    i_curr_inputs = 2;  //"3" for default
    
    i_index = 0.0;      //Storage for calculating indexes
    i_index_x0 = 0;
    i_value_y0 = "";   //Resulting String Value
    i_wrap = 2;        //Wrap

    i_in_param_string = "";
  }
  
  //Initialise Module & GUI
  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "selectors;string_selector";

    info->description =
      "[result] is equal to the\n"
      "[string_x] chosen by [index]\n"
      "\n"
      "The number of inputs is\n"
      "controlled by [inputs]\n" 
      "\n";                       //How do you interpolate between text strings? XD

    info->out_param_spec =
      "result:string";

    info->in_param_spec =
      "index:float,"
      "inputs:enum?0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16&nc=1,"
      + i_in_param_string +
      "wrap:enum?None_Zero|None_Freeze|Wrap";

    info->component_class = "system";

    info->output = 1; // Always Running
  }

  //Build Interface
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true; //Allows main sequencer to start playing

    //Priority parameters - inputs must get loaded before string_x in the state file
    index = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "index");
    index->set(i_index);
    inputs = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "inputs");
    inputs->set((float)(i_curr_inputs + 1)); //converts 0-based index to 1-based index!
    
    string_x.clear();      //Create the array and the param_string for string_x
    i_paramString.str("");
    i_paramString << "string_x:complex{";

    for(int i = 0; i < 16; ++i)
    {
      if(i > 0) i_paramString << ",";
      i_paramName.str("");
      i_paramName << "string_" << i;
      i_paramString << i_paramName.str().c_str() << ":string";

      string_x.push_back((vsx_module_param_string*)in_parameters.create(VSX_MODULE_PARAM_ID_STRING, i_paramName.str().c_str()));
      string_x[i]->set("");
    }

    i_paramString << "},";
    i_in_param_string = i_paramString.str().c_str();
    
    wrap = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "wrap");
    wrap->set(i_wrap);
    
    //Outputs
    result = (vsx_module_param_string*)out_parameters.create(VSX_MODULE_PARAM_ID_STRING,"result");
    result->set(""); 
  }

  //Rebuild Inputs
  void redeclare_in_params(vsx_module_param_list& in_parameters)
  {
    loading_done = true;

    index = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "index");
    inputs = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "inputs");
    
    string_x.clear();
    i_paramString.str("");
    i_paramString << "string_x:complex{";

    for(int i = 0; i <= i_prev_inputs; ++i)
    {
      if(i > 0) i_paramString << ",";
      i_paramName.str("");
      i_paramName << "string_" << i;
      i_paramString << i_paramName.str().c_str() << ":string";

      string_x.push_back((vsx_module_param_string*)in_parameters.create(VSX_MODULE_PARAM_ID_STRING, i_paramName.str().c_str()));
      string_x[i]->set("");
    }

    i_paramString << "},";
    i_in_param_string = i_paramString.str().c_str();
    
    wrap = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "wrap");
    
    i_am_ready = true; //Data is ready, we can start running properly!
  }

//---DATA-SETUP-FUNCTIONS-------------------------------------------------------

  //See /vsxu/engine/include/vsx_math.h for definitions of:
    //FLOAT_MOD(V, M)
    //FLOAT_CLAMP(V, MN, MX)

  //Send the chosen string directly to the output
  void SelectString()
  {
    i_wrap = wrap->get();
    switch(i_wrap)
    {
      case 0: //Don't Wrap Index - Zero Ends
        i_index = FLOAT_CLAMP(index->get() + 0.5, -0.5, (float)(i_prev_inputs) + 1.5);
        i_index_x0 = FLOAT_CLAMP((int)i_index, 0, i_prev_inputs);
        i_value_y0 = ((i_index < -0.0) || (i_index > (float)(i_prev_inputs + 1)) ? 0.0
                   : string_x[i_index_x0]->get());
      break;
      case 1: //Don't Wrap Index - Freeze Ends
        i_index = FLOAT_CLAMP(index->get() + 0.5, 0.5, (float)(i_prev_inputs) + 0.5);
        i_index_x0 = FLOAT_CLAMP((int)i_index, 0, i_prev_inputs);
        i_value_y0 = string_x[i_index_x0]->get();
      break;
      case 2: //Wrap Index
        i_index = FLOAT_MOD(index->get(), (float)(i_prev_inputs + 1));
        i_index_x0 = (i_index > (float)i_prev_inputs)
                   ? ((i_index > ((float)i_prev_inputs + 0.5)) ? 0 : i_prev_inputs)
                   : (int)(i_index + 0.5);
        i_value_y0 = string_x[i_index_x0]->get();
      break;
    }
    result->set(i_value_y0);
  }

//---CORE-FUNCTIONS-------------------------------------------------------------

  //Update Number of Inputs
  void UpdateInputs()
  {
    i_curr_inputs = inputs->get() - 1;
    if(i_prev_inputs != i_curr_inputs)
    {
      i_am_ready = false;
      i_prev_inputs = i_curr_inputs;
      redeclare_in = true;
    }
  }

//---MAIN-PROGRAM-LOOP---------------------------------------------------------

  void run()
  {
    UpdateInputs();
  
    if(i_am_ready) SelectString();
  }
};
