/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Robert "CoR" Wenzel - Copyright (C) 2003-2011
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


#ifndef MATH_BINARY_OPS_H
#define MATH_BINARY_OPS_H

// code by cor

#define BINARY_OP(OP_TYPE, \
  PARAM_TYPE_IN1, PARAM_TYPE_IN2, PARAM_TYPE_OUT, \
  PARAM_NAME_IN1, PARAM_NAME_IN2, PARAM_NAME_OUT, \
  PARAM_ID_IN1,   PARAM_ID_IN2,   PARAM_ID_OUT, \
  PARAM_DEFAULTS, OP_CODE, DESCRIPTION) \
class vsx_arith_ ## OP_TYPE : public vsx_module {\
  /* in*/\
	vsx_module_param_ ## PARAM_TYPE_IN1* param1;\
	vsx_module_param_ ## PARAM_TYPE_IN2* param2;\
	/* out*/\
  vsx_module_param_ ## PARAM_TYPE_OUT* param_out;\
	/* internal*/\
public:\
  void module_info(vsx_module_info* info)\
  {\
    info->identifier = "maths;arithmetics;binary;" #OP_TYPE;\
    info->description = DESCRIPTION;\
    info->out_param_spec = PARAM_NAME_OUT ":" #PARAM_TYPE_OUT ;\
    info->in_param_spec =  PARAM_NAME_IN1 ":" #PARAM_TYPE_IN1 ","\
                           PARAM_NAME_IN2 ":" #PARAM_TYPE_IN2;\
  \
    info->component_class = "parameters";\
  }\
  \
	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)\
	{\
    loading_done = true;\
    param1 = (vsx_module_param_ ## PARAM_TYPE_IN1*)in_parameters.create(PARAM_ID_IN1,PARAM_NAME_IN1);\
    param2 = (vsx_module_param_ ## PARAM_TYPE_IN2*)in_parameters.create(PARAM_ID_IN2,PARAM_NAME_IN2);\
    param_out = (vsx_module_param_ ## PARAM_TYPE_OUT*)out_parameters.create(PARAM_ID_OUT,PARAM_NAME_OUT);\
    PARAM_DEFAULTS\
  }\
  \
	void run() {\
    OP_CODE\
  }\
};

BINARY_OP (
  mod, 
  float, float, float, 
  "param1", "param2", "sum", 
  VSX_MODULE_PARAM_ID_FLOAT, VSX_MODULE_PARAM_ID_FLOAT, VSX_MODULE_PARAM_ID_FLOAT,
  param1->set(0);\
  param2->set(0);\
  param_out->set(0);,
  param_out->set(fmodf(param1->get(),param2->get()));,
  "add:\n"
  "sum = param1 mod param2\n"
  "this module returns the remainder of the division"
  )


BINARY_OP (
  add, 
  float, float, float, 
  "param1", "param2", "sum", 
  VSX_MODULE_PARAM_ID_FLOAT, VSX_MODULE_PARAM_ID_FLOAT, VSX_MODULE_PARAM_ID_FLOAT,
  param1->set(0);\
  param2->set(0);\
  param_out->set(0);,
  param_out->set(param1->get() + param2->get());,
  "add:\n"
  "sum = param1 + param2\n"
  "this module adds 2 float-values"
  )

BINARY_OP (
  sub, 
  float, float, float, 
  "param1", "param2", "sum", 
  VSX_MODULE_PARAM_ID_FLOAT, VSX_MODULE_PARAM_ID_FLOAT, VSX_MODULE_PARAM_ID_FLOAT,
  param1->set(0);\
  param2->set(0);\
  param_out->set(0);,
  param_out->set(param1->get() - param2->get());,
  "sub:\n"
  "sum = param1 - param2\n"
  "this module subtracts 2 float-values"
  )

BINARY_OP (
  mult, 
  float, float, float, 
  "param1", "param2", "product", 
  VSX_MODULE_PARAM_ID_FLOAT, VSX_MODULE_PARAM_ID_FLOAT, VSX_MODULE_PARAM_ID_FLOAT,
  param1->set(0);\
  param2->set(0);\
  param_out->set(0);,
  param_out->set(param1->get() * param2->get());,
  "mult:\n"
  "product = param1 * param2\n"
  "this module multiplies 2 float-values"
  )

BINARY_OP (
  div, 
  float, float, float, 
  "param1", "param2", "quotient", 
  VSX_MODULE_PARAM_ID_FLOAT, VSX_MODULE_PARAM_ID_FLOAT, VSX_MODULE_PARAM_ID_FLOAT,
  
  param1->set(0);\
  param2->set(1);\
  param_out->set(0);,
  
  if(param2->get() != 0) \
    param_out->set(param1->get() / param2->get()); \
  else \
    param_out->set(0);,
  "div:\n"
  "quotient = param1 / param2\n"
  "this module divides 2 float-values"
  )

BINARY_OP (
  max, 
  float, float, float, 
  "param1", "param2", "result", 
  VSX_MODULE_PARAM_ID_FLOAT, VSX_MODULE_PARAM_ID_FLOAT, VSX_MODULE_PARAM_ID_FLOAT,
  
  param1->set(0);\
  param2->set(0);\
  param_out->set(0);,
  
  param_out->set(param1->get() > param2->get()?param1->get():param2->get());,
  "max:\n"
  "maximum = param1 > param2 ? param1 : param2\n"
  "returns the greater value of 2 float-values"
  )

BINARY_OP (
  min, 
  float, float, float, 
  "param1", "param2", "minimum", 
  VSX_MODULE_PARAM_ID_FLOAT, VSX_MODULE_PARAM_ID_FLOAT, VSX_MODULE_PARAM_ID_FLOAT,
  
  param1->set(0);\
  param2->set(0);\
  param_out->set(0);,
  
  param_out->set(param1->get() < param2->get()?param1->get():param2->get());,
  "min:\n"
  "minimum = param1 < param2 ? param1 : param2\n"
  "returns the smaller value of 2 float-values"
  )

BINARY_OP (
  pow, 
  float, float, float, 
  "param1", "param2", "power", 
  VSX_MODULE_PARAM_ID_FLOAT, VSX_MODULE_PARAM_ID_FLOAT, VSX_MODULE_PARAM_ID_FLOAT,
  
  param1->set(0);\
  param2->set(1);\
  param_out->set(0);,
  
  param_out->set((float)pow(param1->get(), param2->get()));,
  "pow:\n"
  "power = param1 ^ param2\n"
  "returns param1 raised to the power of param2"
  )

BINARY_OP (
  round, 
  float, float, float, 
  "param1", "step", "result", 
  VSX_MODULE_PARAM_ID_FLOAT, VSX_MODULE_PARAM_ID_FLOAT, VSX_MODULE_PARAM_ID_FLOAT,
  
  param1->set(0);\
  param2->set(1);\
  param_out->set(0);,
  
  if(param2->get() != 0) \
    param_out->set((float)round(param1->get() / param2->get()) * param2->get()); \
  else \
    param_out->set(param1->get());,
  "round:\n"
  "result = round(param1 / step) * step\n"
  "returns param1 rounded to the next multiple of step"
  )

BINARY_OP (
  floor, 
  float, float, float, 
  "param1", "step", "result", 
  VSX_MODULE_PARAM_ID_FLOAT, VSX_MODULE_PARAM_ID_FLOAT, VSX_MODULE_PARAM_ID_FLOAT,
  
  param1->set(0);\
  param2->set(1);\
  param_out->set(0);,
  
  if(param2->get() != 0) \
    param_out->set((float)floor(param1->get() / param2->get()) * param2->get()); \
  else \
    param_out->set(param1->get());,
  "floor:\n"
  "result = floor(param1 / step) * step\n"
  "returns param1 rounded to the next smaller multiple of step"
  )

BINARY_OP (
  ceil, 
  float, float, float, 
  "param1", "step", "result", 
  VSX_MODULE_PARAM_ID_FLOAT, VSX_MODULE_PARAM_ID_FLOAT, VSX_MODULE_PARAM_ID_FLOAT,
  
  param1->set(0);\
  param2->set(1);\
  param_out->set(0);,
  
  if(param2->get() != 0) \
    param_out->set((float)ceil(param1->get() / param2->get()) * param2->get()); \
  else \
    param_out->set(param1->get());,
  "ceil:\n"
  "result = ceil(param1 / step) * step\n"
  "returns param1 rounded to the next greater multiple of step"
  )
  
  
/*
class vsx_arith_add : public vsx_module {
  // in
	vsx_module_param_float* floata;
	vsx_module_param_float* floatb;
	// out
	vsx_module_param_float* floatc;
	// internal
public:
  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;arithmetics;binary;add";
    info->description = "add 2 floats";
    info->out_param_spec = "float_result:float";
    info->in_param_spec = "float1:float,"
                          "float2:float";
  
    info->component_class = "small:parameters";
  }
  
	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
	{
    loading_done = true;
    floata = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"float1");
    floatb = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"float2");
    floatc = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"float_result");
    floata->set(0);
    floatb->set(0);
    floatc->set(0);
  }
  
	void run() {
    floatc->set(floata->get() + floatb->get());
  }
};
*/

#endif
