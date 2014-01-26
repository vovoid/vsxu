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

#ifndef VSX_BEZIER_CALC_H
#define VSX_BEZIER_CALC_H

#include <vsx_math.h>

//
// recursive two dimensional third-grade newton raphson numerical analysis solver
// and bezier curve

class vsx_bezier_calc
{
public:

  float a;
  float b;
  float c;
  float d;
  float e;
  float f;
  float g;
  float h;

  float x0; // first coordinate
  float y0;
  float x1; // first handle
  float y1;

  float x2; // second handle
  float y2;
  float x3; // second coordinate
  float y3;

  void init()
  {
    a = x3 - 3.0f*x2 + 3.0f*x1 - x0; 
    e = y3 - 3.0f*y2 + 3.0f*y1 - y0;
    b = 3.0f*x2 - 6.0f*x1 + 3.0f*x0;
    f = 3.0f*y2 - 6.0f*y1 + 3.0f*y0;
    c = 3.0f*x1 - 3.0f*x0;
    g = 3.0f*y1 - 3.0f*y0;
    d = x0;
    h = y0;
  }
  
   

  float t_from_x(float x_find, float t = 0.5f, int iter = 5)
  {
    if (iter < 0)
      return t;

    if (iter == 5)
    {
      t = x_find;
    }
    float current_slope = 1.0f/(3.0f*a*t*t + 2.0f*b*t + c);
    float cur_x = t*(t*(a*t + b) + c) + d;
    float nextguess = t + (x_find-cur_x)*(current_slope);

    return t_from_x(x_find,nextguess,iter-1);
  }
  
  inline float x_from_t(float t)
  {
    return t * ( t * ( a * t + b ) + c ) + d;
  }

  inline float y_from_t(float t)
  {
    return t * ( t * ( e * t + f ) + g ) + h;
  }

  inline float midpoint_x()
  {
    return (x3 - x0) * 0.5;
  }

  inline float midpoint_y()
  {
    return (y3 - y0) * 0.5;
  }

  inline float distance_0( const vsx_bezier_calc& o )
  {
    // our mid-point x
    #define MP_X (x0 + (x3-x0)*0.5)
    // our mid-point y
    #define MP_Y (y0 + (y3-y0)*0.5)

    // other's mid-point x
    #define MP_OX (o.x0 + (o.x3-o.x0)*0.5)
    // other's mid-point y
    #define MP_OY (o.y0 + (o.y3-o.y0)*0.5)

    return sqrt
    (
       pow((MP_OX - MP_X),2.0)
     + pow((MP_OY - MP_Y),2.0)
    );
    #undef MP_X
    #undef MP_Y
    #undef MP_OX
    #undef MP_OY
  }

  inline void mirror()
  {
    float ix0 = x3;
    float iy0 = y3;

    float ix1 = x2;
    float iy1 = y2;

    x2 = x1;
    y2 = y1;

    x3 = x0;
    y3 = y0;

    x0 = ix0;
    y0 = iy0;

    x1 = ix1;
    y1 = iy1;
  }

};

#endif
