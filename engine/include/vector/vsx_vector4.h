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


#ifndef VSX_VECTOR4_H
#define VSX_VECTOR4_H

#include <inttypes.h>
#include <vsx_math.h>
#include <vector/vsx_vector3.h>

template<typename T = float>
class vsx_vector4
{
public:
  T x;
  T y;
  T z;
  T w;

  vsx_vector4()
    :
      x(0.0),
      y(0.0),
      z(0.0),
      w(0.0)
  {}

  static size_t arity()
  {
    return 4;
  }

};

typedef vsx_vector4<float> vsx_vector4f;
typedef vsx_vector4<double> vsx_vector4d;


#endif
