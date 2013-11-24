/**
* Project: VSXu Engine: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu Engine.
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

#ifndef VSX_NO_CLIENT
#include <list>
#include "vsx_param.h"
#include "vsx_module.h"
#include "vsx_command.h"
#include "vsx_command_list.h"
#include "vsx_param_interpolation.h"
#include "vsx_quaternion.h"

// --- FLOAT3
bool vsx_module_param_interpolation_float3::set(vsx_module_param_abs* value, int arity, float interpolation_time) {
  VSX_UNUSED(arity);
  dest_interp = interpolation_time;
  for(int i = 0; i < 3; ++i)
  destination_value[i] = ((vsx_module_param_float*)value)->get_internal(i);
  return true;
}

bool vsx_module_param_interpolation_float3::set(vsx_string value, int arity, float interpolation_time) {
  dest_interp = interpolation_time;
  destination_value[arity] = s2f(value);
  return true;
}

bool vsx_module_param_interpolation_float3::interpolate(float dtime) {
  float tt = dtime*dest_interp;
  if (tt > 1.0f) tt = 1.0f;
  int done = 0;
  for(int i = 0; i < 3; ++i) {
    temp = ((vsx_module_param_float3*)target_param)->get_internal(i)*(1-tt)+destination_value[i]*tt;
    ((vsx_module_param_float3*)target_param)->set_internal(temp,i);
    // know where to stop :)
    if (fabs(destination_value[i] - temp) < 0.000001) ++done;
  }
  if (done == 3) return false;
  else
  return true;
}

// --- FLOAT4
bool vsx_module_param_interpolation_float4::set(vsx_module_param_abs* value, int arity, float interpolation_time) {
  VSX_UNUSED(arity);
  dest_interp = interpolation_time;
  for(int i = 0; i < 4; ++i)
  destination_value[i] = ((vsx_module_param_float*)value)->get_internal(i);
  return true;
}

bool vsx_module_param_interpolation_float4::set(vsx_string value, int arity, float interpolation_time) {
  dest_interp = interpolation_time;
  destination_value[arity] = s2f(value);
  return true;
}

bool vsx_module_param_interpolation_float4::interpolate(float dtime) {
  float tt = dtime*dest_interp;
  if (tt > 1) tt = 1;
  int done = 0;
  for(int i = 0; i < 4; ++i) {
    temp = ((vsx_module_param_float3*)target_param)->get_internal(i)*(1-tt)+destination_value[i]*tt;
    ((vsx_module_param_float3*)target_param)->set_internal(temp,i);
    // know where to stop :)
    if (fabs(destination_value[i] - temp) < 0.000001) ++done;
  }
  if (done == 4) return false;
  else
  return true;
}


// --- QUATERNION
bool vsx_module_param_interpolation_quaternion::set(vsx_module_param_abs* value, int arity, float interpolation_time) {
  VSX_UNUSED(arity);
  dest_interp = interpolation_time;
  for(int i = 0; i < 4; ++i)
  destination_value[i] = ((vsx_module_param_float*)value)->get_internal(i);
  return true;
}

bool vsx_module_param_interpolation_quaternion::set(vsx_string value, int arity, float interpolation_time) {
  VSX_UNUSED(arity);
  dest_interp = interpolation_time;
  destination_value[arity] = s2f(value);
  return true;
}

bool vsx_module_param_interpolation_quaternion::interpolate(float dtime) {
  //printf("running quat-interpolation\n");
  float tt = dtime * dest_interp;
  if (tt > 1) tt = 1;
  int done = 0;
  vsx_quaternion from_quat;
  from_quat.x = ((vsx_module_param_quaternion*)target_param)->get_internal(0);
  from_quat.y = ((vsx_module_param_quaternion*)target_param)->get_internal(1);
  from_quat.z = ((vsx_module_param_quaternion*)target_param)->get_internal(2);
  from_quat.w = ((vsx_module_param_quaternion*)target_param)->get_internal(3);
  vsx_quaternion dest_quat;
  //printf("destination value 0: %f\n",destination_value[0]);
  dest_quat.x = destination_value[0];
  dest_quat.y = destination_value[1];
  dest_quat.z = destination_value[2];
  dest_quat.w = destination_value[3];
  vsx_quaternion result_quat;
  result_quat.slerp(from_quat,dest_quat,tt);
  
  float len = 1.0 / (float)sqrt(
    result_quat.x*result_quat.x +
    result_quat.y*result_quat.y +
    result_quat.z*result_quat.z +
    result_quat.w*result_quat.w
  );
  result_quat.x *= len;
  result_quat.y *= len;
  result_quat.z *= len;
  result_quat.w *= len;

  
  ((vsx_module_param_quaternion*)target_param)->set_internal(result_quat.x,0);
  if (fabs(result_quat.x - from_quat.x) < 0.000001) ++done;
  ((vsx_module_param_quaternion*)target_param)->set_internal(result_quat.y,1);
  if (fabs(result_quat.y - from_quat.y) < 0.000001) ++done;
  ((vsx_module_param_quaternion*)target_param)->set_internal(result_quat.z,2);
  if (fabs(result_quat.z - from_quat.z) < 0.000001) ++done;
  ((vsx_module_param_quaternion*)target_param)->set_internal(result_quat.w,3);
  if (fabs(result_quat.w - from_quat.w) < 0.000001) ++done;
  

  //for(int i = 0; i < 4; ++i) {
    //// know where to stop :)
    //if (fabs(destination_value[i])-temp < 0.000001) ++done;
  //}
  if (done == 4) return false;
  else
  return true;
}


// --- FLOAT
bool vsx_module_param_interpolation_float::set(vsx_module_param_abs* value, int arity, float interpolation_time) {
  VSX_UNUSED(arity);
  iterations = 0;
  dest_interp = interpolation_time;
  destination_value = ((vsx_module_param_float*)value)->get_internal();
  return true;
}

bool vsx_module_param_interpolation_float::set(vsx_string value, int arity, float interpolation_time) {
  VSX_UNUSED(arity);
  iterations = 0;
  dest_interp = interpolation_time;
  destination_value = s2f(value);
  return true;
}

bool vsx_module_param_interpolation_float::interpolate(float dtime) {
  //float tt = dtime*16;
  double tt = dtime*dest_interp;
  if (tt > 1.0) tt = 1.0;
  temp = ((vsx_module_param_float*)target_param)->get_internal()*(1.0-tt)+(double)destination_value*tt;
  //printf("temp: %f\n",temp);
  ((vsx_module_param_float*)target_param)->set_internal(temp);
  //printf("dv:   %f       t:   %f\n",destination_value,temp);
  if (++iterations > 5000) return false;
  if (fabs(destination_value - temp) < 0.00001f) return false; 
  else
  return true;
}

//------------------------
vsx_module_param_interpolation_abs* vsx_module_param_interpolation_list::add(vsx_engine_param* parameter) {
  vsx_module_param_interpolation_abs* t = 0;

  //float type
  if (parameter->module_param->type == VSX_MODULE_PARAM_ID_FLOAT) {
    if (listeners_map.find(parameter) == listeners_map.end()) {
      t = new vsx_module_param_interpolation_float(parameter->module_param);
      t->t_engine_param = parameter;
      t->module = parameter->module;
      listeners_map[parameter] = t;
      listeners.push_back(t);
      return t;
    } else {
      return listeners_map[parameter];
    }  
  }
  else
  //float3 type
  if (parameter->module_param->type == VSX_MODULE_PARAM_ID_FLOAT3) {
    if (listeners_map.find(parameter) == listeners_map.end()) {
      t = new vsx_module_param_interpolation_float3(parameter->module_param);
      t->t_engine_param = parameter;
      t->module = parameter->module;
      listeners_map[parameter] = t;      
      listeners.push_back(t);
      return t;
    } else {
      return listeners_map[parameter];
    }  
  }
  else
  //float4 type
  if (parameter->module_param->type == VSX_MODULE_PARAM_ID_FLOAT4) {
    if (listeners_map.find(parameter) == listeners_map.end()) {
      t = new vsx_module_param_interpolation_float4(parameter->module_param);
      t->t_engine_param = parameter;
      t->module = parameter->module;
      listeners_map[parameter] = t;
      listeners.push_back(t);
      return t;
    } else {
      return listeners_map[parameter];
    }  
  }
  else
  //quaternion type
  if (parameter->module_param->type == VSX_MODULE_PARAM_ID_QUATERNION) {
    if (listeners_map.find(parameter) == listeners_map.end()) {
      t = new vsx_module_param_interpolation_quaternion(parameter->module_param);
      t->t_engine_param = parameter;
      t->module = parameter->module;
      listeners_map[parameter] = t;
      listeners.push_back(t);
      return t;
    } else {
      return listeners_map[parameter];
    }  
  }

  return 0;
};

bool vsx_module_param_interpolation_list::remove(vsx_engine_param* parameter) {
  if (listeners_map.find(parameter) != listeners_map.end()) {
//    printf("removing parameter\n");
    listeners.remove(listeners_map[parameter]);
    listeners_map.erase(parameter);
  }
  return true;
}

bool vsx_module_param_interpolation_list::schedule_remove(vsx_engine_param* parameter) {
  if (listeners_map.find(parameter) != listeners_map.end()) {
    remove_list.push_back(parameter);
    //listeners.remove(listeners_map[parameter]);
    //listeners_map.erase(parameter);
  }
  return true;
}

/*void vsx_module_param_interpolation_list::set_target_value(vsx_engine_param* target, vsx_engine_param* value, int arity,float interpolation_time) {
  if (listeners_map.find(target) != listeners_map.end()) {
    listeners_map[target]->set(value->module_param,arity,interpolation_time);
  } else {
    vsx_module_param_interpolation_abs* new_interpolation = add(target);
    if (new_interpolation)
    new_interpolation->set(value,arity,interpolation_time);
  }
}*/

void vsx_module_param_interpolation_list::set_target_value(vsx_engine_param* target, vsx_string value, int arity,float interpolation_time) {
  if (listeners_map.find(target) != listeners_map.end()) {
    listeners_map[target]->set(value,arity,interpolation_time);
  } else {
    vsx_module_param_interpolation_abs* new_interpolation = add(target);
    if (new_interpolation)
    new_interpolation->set(value,arity,interpolation_time);
  }
}
//
void vsx_module_param_interpolation_list::run(float d_time) {
  //printf("run\n");
  for (std::list<vsx_engine_param*>::iterator it_r = remove_list.begin(); it_r != remove_list.end(); it_r++) {
    //printf("removing interpolator\n");
    remove(*it_r);
  }
  remove_list.clear();
  for (std::list<vsx_module_param_interpolation_abs*>::iterator it = listeners.begin(); it != listeners.end(); it++) {
    //printf("running interpolator\n");
    if ((*it)->interpolate(d_time)) {
      ++(*it)->module->param_updates;
      ++(*it)->target_param->updates;
    } else {
      remove_list.push_back((*it)->t_engine_param);
      //delete *it;
    }  
  }
}

#endif
