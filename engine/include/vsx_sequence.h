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


#ifndef VSX_SEQUENCE_H
#define VSX_SEQUENCE_H


#include <vsx_platform.h>
#include <vsx_string_helper.h>
#include <vector/vsx_vector3.h>

#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
#define VSX_SEQUENCE_DLLIMPORT
#else
  #ifdef VSX_ENG_DLL
    #define VSX_SEQUENCE_DLLIMPORT __declspec (dllexport) 
  #else
    #define VSX_SEQUENCE_DLLIMPORT __declspec (dllimport)
  #endif
#endif

class vsx_sequence_item
{
public:
  float value;
  float delay; // in seconds (float)
  int interpolation;
  vsx_vector3<> handle1;
  vsx_vector3<> handle2;

  vsx_string get_value() {
    if (interpolation == 4) {
      return vsx_string_helper::f2s(value)+":"+vsx_string_helper::f2s(handle1.x)+","+vsx_string_helper::f2s(handle1.y)+":"+vsx_string_helper::f2s(handle2.x)+","+vsx_string_helper::f2s(handle2.y);
    } else {
      return vsx_string_helper::f2s(value);
    }
  }

  vsx_sequence_item():value(1),delay(1),interpolation(0) {}
};

class vsx_sequence
{
  vsx_bezier_calc<float> bez_calc;
public:
  vsx_avector<vsx_sequence_item> items;
  float i_time;
  float cur_val;
  float cur_delay;
  int cur_interpolation;
  long line_cur;

  float to_val;
  float line_time;
  int i_cur;

  long timestamp;
  VSX_SEQUENCE_DLLIMPORT vsx_sequence();
  VSX_SEQUENCE_DLLIMPORT vsx_sequence(const vsx_sequence& seq);
  VSX_SEQUENCE_DLLIMPORT vsx_sequence(vsx_sequence& seq);
  VSX_SEQUENCE_DLLIMPORT virtual ~vsx_sequence();

  VSX_SEQUENCE_DLLIMPORT vsx_sequence& operator=(vsx_sequence& ss);

  VSX_SEQUENCE_DLLIMPORT void reset();

  void set_time(float time) {
    execute(time-i_time);
  }

  float execute_absolute(float time) {
  	return execute(time-i_time);
  }

  VSX_SEQUENCE_DLLIMPORT float execute(float t_incr);
  #ifndef VSX_NO_SEQUENCE
  VSX_SEQUENCE_DLLIMPORT vsx_string get_string();
  VSX_SEQUENCE_DLLIMPORT void set_string(vsx_string str);
  #else
  virtual vsx_string get_string() = 0;
  virtual void set_string(vsx_string str) = 0;
  #endif
};

#endif
