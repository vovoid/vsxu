#ifndef VSX_PARAM_INTERPOLATION_H
#define VSX_PARAM_INTERPOLATION_H

#include "vsx_param_abstraction.h"
class vsx_module_param_interpolation_abs {
public:
  vsx_engine_param* t_engine_param;
  vsx_module_param_abs* target_param;
  vsx_module* module;
  
  // set new destination value
  virtual bool set(vsx_module_param_abs* value,int arity, float interpolation_time) {return false;};
  virtual bool set(vsx_string value,int arity, float interpolation_time) {return false;};
  // to be run each frame
  virtual bool interpolate(float dtime) {return false;};
  vsx_module_param_interpolation_abs() {};
  vsx_module_param_interpolation_abs(vsx_module_param_abs* param) {
    target_param = param;
  }  
  virtual ~vsx_module_param_interpolation_abs() {};
};  


class vsx_module_param_interpolation_float : public vsx_module_param_interpolation_abs {
  int iterations;
  double temp;
  float destination_value;
  float dest_interp;
public:
  bool set(vsx_module_param_abs* value,int arity, float interpolation_time);
  bool set(vsx_string value,int arity, float interpolation_time);
  bool interpolate(float dtime);
  vsx_module_param_interpolation_float(vsx_module_param_abs* param) {
    target_param = param;
  }
};

class vsx_module_param_interpolation_float3 : public vsx_module_param_interpolation_abs {
  float temp;
  float destination_value[3];
  float dest_interp;
public:
  bool set(vsx_module_param_abs* value,int arity, float interpolation_time);
  bool set(vsx_string value,int arity, float interpolation_time);
  bool interpolate(float dtime);
  vsx_module_param_interpolation_float3(vsx_module_param_abs* param) {
    target_param = param;
  }
};

class vsx_module_param_interpolation_float4 : public vsx_module_param_interpolation_abs {
  float temp;
  float destination_value[4];
  float dest_interp;
public:
  bool set(vsx_module_param_abs* value,int arity, float interpolation_time);
  bool set(vsx_string value,int arity, float interpolation_time);
  bool interpolate(float dtime);
  vsx_module_param_interpolation_float4(vsx_module_param_abs* param) {
    target_param = param;
  }
};


class vsx_module_param_interpolation_quaternion : public vsx_module_param_interpolation_abs {
  float temp;
  float destination_value[4];
  float dest_interp;
public:
  bool set(vsx_module_param_abs* value,int arity, float interpolation_time);
  bool set(vsx_string value,int arity, float interpolation_time);
  bool interpolate(float dtime);
  vsx_module_param_interpolation_quaternion(vsx_module_param_abs* param) {
    target_param = param;
  }
};



class vsx_module_param_interpolation_list {
  std::list<vsx_module_param_interpolation_abs*> listeners;
  std::map<vsx_engine_param*,vsx_module_param_interpolation_abs*> listeners_map;
  std::list<vsx_engine_param*> remove_list;
public:
  vsx_module_param_interpolation_abs* add(vsx_engine_param* parameter);


  bool remove(vsx_engine_param* parameter);
  bool schedule_remove(vsx_engine_param* parameter);

  //void set_target_value(vsx_engine_param* target, vsx_engine_param* value, int arity = 0,float interpolation_time = 16);
  void set_target_value(vsx_engine_param* target, vsx_string value, int arity = 0,float interpolation_time = 16);
  
  void run(float d_time);
}; 

#endif
