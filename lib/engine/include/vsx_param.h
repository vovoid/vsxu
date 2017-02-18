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

#ifndef VSX_PARAM_H
#define VSX_PARAM_H

#include <vsx_platform.h>
#include "vsx_engine_dllimport.h"

#include <container/vsx_nw_vector.h>
#include <container/vsx_nw_vector_nd.h>
#include <string/vsx_string.h>

#ifndef VSX_NO_GL
  #include "vsx_gl_global.h"
#endif
#include <graphics/vsx_mesh.h>
#include <graphics/vsx_grid_mesh.h>
#include <texture/vsx_texture.h>
#include <bitmap/vsx_bitmap.h>
#include <particlesystem/vsx_particlesystem.h>
#include <math/quaternion/vsx_quaternion.h>
#include <math/vsx_float_array.h>
#include <math/vector/vsx_vector_array.h>
#include <math/quaternion/vsx_quaternion_array.h>
#include <math/vsx_bezier_calc.h>
#include <math/vsx_sequence.h>
#include <vsx_abstract.h>

//////////////////////////////////////////////////////////////////////

class vsx_module;
class vsx_module_param_abs;

//////////////////////////////////////////////////////////////////////
#define VSX_MODULE_PARAM_ID_INT                0
#define VSX_MODULE_PARAM_ID_RENDER             1
#define VSX_MODULE_PARAM_ID_FLOAT3             2
#define VSX_MODULE_PARAM_ID_DOUBLE             3
#define VSX_MODULE_PARAM_ID_STRING             4
#define VSX_MODULE_PARAM_ID_TEXTURE            5
#define VSX_MODULE_PARAM_ID_FLOAT              6
#define VSX_MODULE_PARAM_ID_FLOAT4             7
#define VSX_MODULE_PARAM_ID_MATRIX             8
#define VSX_MODULE_PARAM_ID_MESH               9
#define VSX_MODULE_PARAM_ID_BITMAP            10
#define VSX_MODULE_PARAM_ID_PARTICLESYSTEM    11
#define VSX_MODULE_PARAM_ID_FLOAT_ARRAY       12
#define VSX_MODULE_PARAM_ID_FLOAT_SEQUENCE    13
#define VSX_MODULE_PARAM_ID_STRING_SEQUENCE   14
#define VSX_MODULE_PARAM_ID_SEGMENT_MESH      15
#define VSX_MODULE_PARAM_ID_ABSTRACT          16
#define VSX_MODULE_PARAM_ID_QUATERNION        17
#define VSX_MODULE_PARAM_ID_RESOURCE          18
#define VSX_MODULE_PARAM_ID_FLOAT3_ARRAY      19
#define VSX_MODULE_PARAM_ID_QUATERNION_ARRAY  20



//////////////////////////////////////////////////////////////////////
class vsx_param_vsxl_abs;
class vsx_module_param_abs {
public:
  void* vsxl_modifier;

  int type; // parameter type: VSX_MODULE_PARAM_ID_INT, VSX_MODULE_PARAM_ID_RENDER etc.
  vsx_string<>name;
  unsigned long updates; // number of writes, incremented each time the parameter is written to
  bool connected;
  bool valid;
  bool value_from_module;
  bool critical; // is this critical for whatever purpouse it exists?
  bool all_required; // when multiple stuff connected to this, do all have to return true in order to run the module?
  bool run_activate_offscreen; // wether to run activate/deactivate offscreen methods for this parameter (default is true)

  const vsx_string<>& get_name() const {return name;}

  virtual void set_current_as_default() = 0;
  virtual void set_default() = 0;
  vsx_module_param_abs(const char* nname) :
    vsxl_modifier(0x0),
    type(0),
    name(nname),
    updates(0),
    connected(false),
    valid(false),
    value_from_module(false),
    critical(false),
    all_required(false),
    run_activate_offscreen(false)
  {}
  virtual ~vsx_module_param_abs() {}
};


//////////////////////////////////////////////////////////////////////

template<int id, typename T, int arity = 1, int clear_ram = 0>

class vsx_module_param : public vsx_module_param_abs {
public:
  // the data on module level
  T* param_data;
  // the value the GUI/other components assign this 
  T* param_data_suggestion;
  // default suggestion value
  T* param_data_default;
  
  bool default_set;

  vsx_module_param(const char* p_name) : 
    vsx_module_param_abs(p_name), 
    param_data(0),
    param_data_suggestion(0),
    param_data_default(0),
    default_set(false) {}


  void check_free()	{
    //printf("check free\n");
    if(!param_data) {
      param_data = new T[arity];
      param_data_default = new T[arity];
      param_data_suggestion = new T[arity];
      if (clear_ram)
      {
        memset((void*)param_data_default,0,sizeof(T)*arity);
        memset((void*)param_data,0,sizeof(T)*arity);
        memset((void*)param_data_suggestion,0,sizeof(T)*arity);
      }
    }
  }


  void set_current_as_default() {
    if (param_data) {
      for (int i = 0; i < arity; ++i) {
        param_data_default[i] = param_data[i];
      }
      default_set = true;
    }  
  }
  
  void set_default() {
    if (!default_set) return;
    if (param_data) {
      for (int i = 0; i < arity; ++i) {
        param_data_suggestion[i] = param_data_default[i];
        param_data[i] = param_data_default[i];
      }
    }  
  }  
  // -------------------------------------------------------------------------------------------
  // MODULE PARAMETER METHODS
  // -------------------------------------------------------------------------------------------
  // this method is only used by the modules 
  void set(T val, int index = 0)
  {
    check_free();
    param_data[index] = val;
    param_data_suggestion[index] = val;
    valid = true;
    value_from_module = true;
  }
  // this method is only used by the modules 
  void set_p(T& val, int index = 0)
  {
    check_free();
    param_data[index] = val;
    param_data_suggestion[index] = val;
    valid = true;
    value_from_module = true;
  }

  void set_inc_updates(T val, int index = 0)
  {
    set(val, index);
    updates++;
  }

  
  T* get_addr()
  {
    if (!valid)
      return 0;

    return param_data;
  }
  
  T& get(int index = 0)
  {
    return param_data[index];
  }

  bool has_value()
  {
    return valid;
  }
  
  // -------------------------------------------------------------------------------------------
  // ENGINE PARAMETER METHODS
  // -------------------------------------------------------------------------------------------
  // sets the suggestion value and the real value or if there's a vsxl modifier available
  // it runs that and lets it set the destination value instead.
  // this means for a float3 that the modifier will be run 3 times but that's all good if you
  // want to recalculate normals or whatever with it.
  void set_internal(T val, int index = 0) {
    check_free();
    param_data_suggestion[index] = val;
    if (!vsxl_modifier) {
      param_data[index] = val;
    }
  }

  void set_internal_from_param(vsx_module_param* val) {
    if (!val->valid) {
      valid = false;
      return;
    }
    check_free();
    for (int i = 0; i < arity; ++i) {
      param_data_suggestion[i] = val->param_data[i];
      if (!vsxl_modifier) {
        param_data[i] = val->param_data[i];
      }		
    }
    updates++;
    valid = true;
    return;
  }

  bool set_internal_from_param_with_value_check(vsx_module_param* val) {
    if (!val->valid) {
      valid = false;
      return false;
    }
    bool changed = false;
    check_free();
    for (int i = 0; i < arity; ++i) {
      param_data_suggestion[i] = val->param_data[i];
      if (!vsxl_modifier) {
        if (param_data[i] != val->param_data[i]) changed=true;

        param_data[i] = val->param_data[i];
      }
    }
    valid = true;
    return changed;
  }

  void set_raw(T val, int index = 0) {
    param_data[index] = val;
  }

  T get_internal(int index = 0) {
    return param_data_suggestion[index];
  }

  ~vsx_module_param() {
    if (param_data_suggestion)
    delete[] param_data_suggestion;
    if (param_data_default)
    delete[] param_data_default;
    if (param_data)
    delete[] param_data;
  }
};

//////////////////////////////////////////////////////////////////////

class vsx_module_param_list {
public:
  vsx_nw_vector<vsx_module_param_abs*> id_vec;
  ENGINE_DLLIMPORT vsx_module_param_abs* create(int type, const char* name, bool crit = false, bool all_required = false);
  ~vsx_module_param_list();
};

typedef vsx_module_param<0, int,                 1,1 > vsx_module_param_int; // use get() set()
typedef vsx_module_param<1, int,                 1,1 > vsx_module_param_render; // use get() set()
typedef vsx_module_param<0, float,               1,1 > vsx_module_param_float; // use get() set()
typedef vsx_module_param<0, float,               3,1> vsx_module_param_float3; // use get() set()
typedef vsx_module_param<0, double,              1,1 > vsx_module_param_double; // use get() set()
typedef vsx_module_param<0, vsx_string<>,          1,0 > vsx_module_param_string; // use get() set()
#ifdef VSX_NO_GL
  typedef vsx_module_param<0, void*,             1,1 > vsx_module_param_texture; 
#else
  #ifndef VSX_NO_TEXTURE
    typedef vsx_module_param<0, vsx_texture<vsx_texture_gl>*,    1,0 > vsx_module_param_texture; // use get() set()
  #else
    typedef vsx_module_param<0, void*,           1,1 > vsx_module_param_texture;
  #endif
#endif
typedef vsx_module_param<0, float,                 4,1 > vsx_module_param_float4; // use get() set()
typedef vsx_module_param<0, vsx_matrix<float>,     1,0 > vsx_module_param_matrix; // use get() set()
typedef vsx_module_param<0, vsx_mesh<>*,           1,0 > vsx_module_param_mesh; // use get() / set()
typedef vsx_module_param<0, vsx_bitmap*,            1,0 > vsx_module_param_bitmap; // use get_addr() / set_p()
typedef vsx_module_param<0, vsx_particlesystem<>,  1,0 > vsx_module_param_particlesystem; // use get_addr() / set_p()
typedef vsx_module_param<0, vsx_float_array,       1,0 > vsx_module_param_float_array; // use get_addr() set_p()
typedef vsx_module_param<0, vsx::sequence::channel<vsx::sequence::value_float>, 1,0 > vsx_module_param_float_sequence; // use get() set()
typedef vsx_module_param<0, vsx::sequence::channel<vsx::sequence::value_string>, 1,0 > vsx_module_param_string_sequence; // use get() set()
typedef vsx_module_param<0, vsx_2dgrid_mesh,       1,0 > vsx_module_param_segment_mesh; // use get_addr() / set_p()
typedef vsx_module_param<0, vsx_abstract,          1,0 > vsx_module_param_abstract; // use get_addr() / set_p()
typedef vsx_module_param<1, float,                 4,1 > vsx_module_param_quaternion; // use get_addr() / set_p()
typedef vsx_module_param<1, vsx_string<>,            1,0 > vsx_module_param_resource; // use get() / set()
typedef vsx_module_param<0, vsx_vector_array<>,    1,0 > vsx_module_param_float3_array; // use get_addr() set_p()
typedef vsx_module_param<0, vsx_quaternion_array<>,1,0 > vsx_module_param_quaternion_array; // use get_addr() set_p()


// Helper macros

#define VSX_PARAM_SET_FLOAT3_VECTOR3F(p, v) \
  p->set( v.x, 0); p->set( v.y, 1); p->set( v.z, 2);

#define VSX_PARAM_SET_FLOAT4_COLOR(p, v) \
  p->set( v.r, 0); p->set( v.g, 1); p->set( v.b, 2); p->set( v.a, 3);

#define VSX_PARAM_SET_QUATERNION_QUATERNION(p, v) \
  p->set( v.x, 0); p->set( v.y, 1); p->set( v.z, 2); p->set( v.w, 3);


#endif
