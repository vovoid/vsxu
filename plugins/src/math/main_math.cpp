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



#include "_configuration.h"
#include "vsx_param.h"
#include <module/vsx_module.h>
#include "math_binary_ops.h"
#include <math/quaternion/vsx_quaternion.h>




//TODO: make an enumeration toggler

#ifndef MIN_MAX_STATIC
  #define MIN_MAX_STATIC
float vsx_max (float x, float a)
{
   x -= a;
   x += (float)fabs (x);
   x *= 0.5f;
   x += a;
   return (x);
}

float vsx_min (float x, float b)
{
   x = b - x;
   x += (float)fabs (x);
   x *= 0.5f;
   x = b - x;
   return (x);
}

#endif

float clip (float x, float a, float b)
{
   float x1 = (float)fabs (x-a);
   float x2 = (float)fabs (x-b);
   x = x1 + (a+b);
   x -= x2;
   x *= 0.5f;
   return (x);
}


#include "math_binary_ops.h"
#include "module_3float_to_float3.h"
#include "module_4f_hsv_to_rgb_f4.h"
#include "module_4float_to_float4.h"
#include "module_axis_angle_to_quaternion.h"
#include "module_bool_and.h"
#include "module_bool_nand.h"
#include "module_bool_nor.h"
#include "module_bool_not.h"
#include "module_bool_or.h"
#include "module_bool_xor.h"
#include "module_bspline_matrix.h"
#include "module_f4_hsl_to_rgb_f4.h"
#include "module_float3_accumulator.h"
#include "module_float3_dummy.h"
#include "module_float3_interpolate.h"
#include "module_float3to3float.h"
#include "module_float4_accumulator.h"
#include "module_float4_add.h"
#include "module_float4_dummy.h"
#include "module_float4_interpolate.h"
#include "module_float4_mul_float.h"
#include "module_float_abs.h"
#include "module_float_accumulator.h"
#include "module_float_accumulator_limits.h"
#include "module_float_acos.h"
#include "module_float_array_average.h"
#include "module_float_array_pick.h"
#include "module_float_array_memory_buffer.h"
#include "module_float_compare.h"
#include "module_float_cos.h"
#include "module_float_clamp.h"
#include "module_float_dummy.h"
#include "module_float_interpolate.h"
#include "module_float_limit.h"
#include "module_float_sin.h"
#include "module_float_smooth.h"
#include "module_float_to_float3.h"
#include "module_matrix_to_quaternion.h"
#include "module_quaternion_dummy.h"
#include "module_quaternion_mul.h"
#include "module_quaternion_rotation_accumulator_2d.h"
#include "module_quaternion_slerp_2.h"
#include "module_quaternion_slerp_3.h"
#include "module_quaternion_to_axis_angle.h"
#include "module_vector_4float_to_quaternion.h"
#include "module_vector_add_float.h"
#include "module_vector_add.h"
#include "module_vector_cross_product.h"
#include "module_vector_dot_product.h"
#include "module_vector_float4_to_4float.h"
#include "module_vector_from_points.h"
#include "module_vector_mul_float.h"
#include "module_vector_normalize.h"
#include "module_vector_quaternion_to_4float.h"


//******************************************************************************
//*** F A C T O R Y ************************************************************
//******************************************************************************

#ifndef _WIN32
#define __declspec(a)
#endif

extern "C" {
__declspec(dllexport) vsx_module* create_new_module(unsigned long module, void* args);
__declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
__declspec(dllexport) unsigned long get_num_modules(vsx_module_engine_environment* environment);
}

#ifndef MOD_CM
#define MOD_CM vsx_module_math_cm
#define MOD_DM vsx_module_math_dm
#define MOD_NM vsx_module_math_nm
#endif



vsx_module* MOD_CM(unsigned long module, void* args)
{
  VSX_UNUSED(args);
  switch (module)
  {
    case 0:  return (vsx_module*)(new module_3float_to_float3);
    case 1:  return (vsx_module*)(new module_4float_to_float4);
    case 2:  return (vsx_module*)(new module_4f_hsv_to_rgb_f4);
    case 3:  return (vsx_module*)(new module_float_dummy);
    case 4:  return (vsx_module*)(new module_float3_dummy);
    case 5:  return (vsx_module*)(new module_float_array_pick);
    case 6:  return (vsx_module*)(new module_arith_add);
    case 7:  return (vsx_module*)(new module_arith_sub);
    case 8:  return (vsx_module*)(new module_arith_mult);
    case 9:  return (vsx_module*)(new module_arith_div);
    case 10: return (vsx_module*)(new module_arith_min);
    case 11: return (vsx_module*)(new module_arith_max);
    case 12: return (vsx_module*)(new module_arith_pow);
    case 13: return (vsx_module*)(new module_arith_round);
    case 14: return (vsx_module*)(new module_arith_floor);
    case 15: return (vsx_module*)(new module_arith_ceil);
    case 16: return (vsx_module*)(new module_float_accumulator);
    case 17: return (vsx_module*)(new module_float3_accumulator);
    case 18: return (vsx_module*)(new module_float4_accumulator);
    case 19: return (vsx_module*)(new module_vector_add);
    case 20: return (vsx_module*)(new module_vector_add_float);
    case 21: return (vsx_module*)(new module_vector_mul_float);
    case 22: return (vsx_module*)(new module_float_to_float3);
    case 23: return (vsx_module*)(new module_float_abs);
    case 24: return (vsx_module*)(new module_float_sin);
    case 25: return (vsx_module*)(new module_float_cos);
    case 26: return (vsx_module*)(new module_bool_and);
    case 27: return (vsx_module*)(new module_bool_or);
    case 28: return (vsx_module*)(new module_bool_nor);
    case 29: return (vsx_module*)(new module_bool_xor);
    case 30: return (vsx_module*)(new module_bool_not);
    case 31: return (vsx_module*)(new module_float4_mul_float);
    case 32: return (vsx_module*)(new module_bool_nand);
    case 33: return (vsx_module*)(new module_float4_add);
    case 34: return (vsx_module*)(new module_float_array_average);
    case 35: return (vsx_module*)(new module_arith_mod);
    case 36: return (vsx_module*)(new module_f4_hsl_to_rgb_f4);
    case 37: return (vsx_module*)(new module_float3to3float);
    case 38: return (vsx_module*)(new module_float_limit);
    case 39: return (vsx_module*)(new module_vector_4float_to_quaternion);
    case 40: return (vsx_module*)(new module_float_smooth);
    case 41: return (vsx_module*)(new module_quaternion_rotation_accumulator_2d);
    case 42: return (vsx_module*)(new module_vector_normalize);
    case 43: return (vsx_module*)(new module_vector_cross_product);
    case 44: return (vsx_module*)(new module_vector_dot_product);
    case 45: return (vsx_module*)(new module_vector_from_points);
    case 46: return (vsx_module*)(new module_quaternion_slerp_2);
    case 47: return (vsx_module*)(new module_quaternion_mul);
    case 48: return (vsx_module*)(new module_float_accumulator_limits);
    case 49: return (vsx_module*)(new module_quaternion_slerp_3);
    case 50: return (vsx_module*)(new module_float4_interpolate);
    case 51: return (vsx_module*)(new module_float_interpolate);
    case 52: return (vsx_module*)(new module_float_acos);
    case 53: return (vsx_module*)(new module_quaternion_dummy);
    case 54: return (vsx_module*)(new module_vector_quaternion_to_4float);
    case 55: return (vsx_module*)(new module_float3_interpolate);
    case 56: return (vsx_module*)(new module_float4_dummy);
    case 57: return (vsx_module*)(new module_axis_angle_to_quaternion);
    case 58: return (vsx_module*)(new module_quaternion_to_axis_angle);
    case 59: return (vsx_module*)(new module_bspline_matrix);
    case 60: return (vsx_module*)(new module_float_array_memory_buffer);
    case 61: return (vsx_module*)(new module_matrix_to_quaternion);
    case 62: return (vsx_module*)(new module_float_compare);
    case 63: return (vsx_module*)(new module_float_clamp);
  }

  return 0;
}

void MOD_DM(vsx_module* m,unsigned long module)
{
  switch(module) {
    case 0: delete (module_3float_to_float3*)m; break;
    case 1: delete (module_4float_to_float4*)m; break;
    case 2: delete (module_4f_hsv_to_rgb_f4*)m; break;
    case 3: delete (module_float_dummy*)m; break;
    case 4: delete (module_float3_dummy*)m; break;
    case 5: delete (module_float_array_pick*)m; break;
    case 6: delete (module_arith_add*)m; break;
    case 7: delete (module_arith_sub*)m; break;
    case 8: delete (module_arith_mult*)m; break;
    case 9: delete (module_arith_div*)m; break;
    case 10: delete (module_arith_min*)m; break;
    case 11: delete (module_arith_max*)m; break;
    case 12: delete (module_arith_pow*)m; break;
    case 13: delete (module_arith_round*)m; break;
    case 14: delete (module_arith_floor*)m; break;
    case 15: delete (module_arith_ceil*)m; break;
    case 16: delete (module_float_accumulator*)m; break;
    case 17: delete (module_float3_accumulator*)m; break;
    case 18: delete (module_float4_accumulator*)m; break;
    case 19: delete (module_vector_add*)m; break;
    case 20: delete (module_vector_add_float*)m; break;
    case 21: delete (module_vector_mul_float*)m; break;
    case 22: delete (module_float_to_float3*)m; break;
    case 23: delete (module_float_abs*)m; break;
    case 24: delete (module_float_sin*)m; break;
    case 25: delete (module_float_cos*)m; break;
    case 26: delete (module_bool_and*)m; break;
    case 27: delete (module_bool_or*)m; break;
    case 28: delete (module_bool_nor*)m; break;
    case 29: delete (module_bool_xor*)m; break;
    case 30: delete (module_bool_not*)m; break;
    case 31: delete (module_float4_mul_float*)m; break;
    case 32: delete (module_bool_nand*)m; break;
    case 33: delete (module_float4_add*)m; break;
    case 34: delete (module_float_array_average*)m; break;
    case 35: delete (module_arith_mod*)m; break;
    case 36: delete (module_f4_hsl_to_rgb_f4*)m; break;
    case 37: delete (module_float3to3float*)m; break;
    case 38: delete (module_float_limit*)m; break;
    case 39: delete (module_vector_4float_to_quaternion*)m; break;
    case 40: delete (module_float_smooth*)m; break;
    case 41: delete (module_quaternion_rotation_accumulator_2d*)m; break;
    case 42: delete (module_vector_normalize*)m; break;
    case 43: delete (module_vector_cross_product*)m; break;
    case 44: delete (module_vector_dot_product*)m; break;
    case 45: delete (module_vector_from_points*)m; break;
    case 46: delete (module_quaternion_slerp_2*)m; break;
    case 47: delete (module_quaternion_mul*)m; break;
    case 48: delete (module_float_accumulator_limits*)m; break;
    case 49: delete (module_quaternion_slerp_3*)m; break;
    case 50: delete (module_float4_interpolate*)m; break;
    case 51: delete (module_float_interpolate*)m; break;
    case 52: delete (module_float_acos*)m; break;
    case 53: delete (module_quaternion_dummy*)m; break;
    case 54: delete (module_vector_quaternion_to_4float*)m; break;
    case 55: delete (module_float3_interpolate*)m; break;
    case 56: delete (module_float4_dummy*)m; break;
    case 57: delete (module_axis_angle_to_quaternion*)m; break;
    case 58: delete (module_quaternion_to_axis_angle*)m; break;
    case 59: delete (module_bspline_matrix*)m; break;
    case 60: delete (module_float_array_memory_buffer*)m; break;
    case 61: delete (module_matrix_to_quaternion*)m; break;
    case 62: delete (module_float_compare*)m; break;
    case 63: delete (module_float_clamp*)m; break;
  }

}

unsigned long MOD_NM(vsx_module_engine_environment* environment)
{
  VSX_UNUSED(environment);
  return 64;
}
