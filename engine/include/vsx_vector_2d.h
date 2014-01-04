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


#ifndef VSX_VECTOR_2D_H
#define VSX_VECTOR_2D_H

#include <vsx_math.h>

class vsx_vector_2d
{
public:
    GLfloat x;
    GLfloat y;

  inline vsx_vector_2d operator +(const vsx_vector_2d &t)
  {
    vsx_vector_2d temp;
    temp.x = x+t.x;
    temp.y = y+t.y;
    return temp;
  }

  inline vsx_vector_2d operator +=(const vsx_vector_2d &t)
  {
    x+=t.x;
    y+=t.y;
    return *this;
  }

  inline vsx_vector_2d operator *=(const float &t)
  {
    x*=t;
    y*=t;
    return *this;
  }

  inline vsx_vector_2d operator -=(const vsx_vector_2d &t)
  {
    x-=t.x;
    y-=t.y;
    return *this;
  }

  inline vsx_vector_2d operator -()
  {
    vsx_vector_2d temp;
    temp.x = -x;
    temp.y = -y;
    return temp;
  }

  inline vsx_vector_2d operator -(const vsx_vector_2d &t)
  {
    vsx_vector_2d temp;
    temp.x = x-t.x;
    temp.y = y-t.y;
    return temp;
  }

  inline vsx_vector_2d operator *(const vsx_vector_2d &t)
  {
    vsx_vector_2d temp;
    temp.x = x*t.x;
    temp.y = y*t.y;
    return temp;
  }

  inline vsx_vector_2d operator *(const float &t)
  {
    vsx_vector_2d temp;
    temp.x = x*t;
    temp.y = y*t;
    return temp;
  }

  inline vsx_vector_2d operator /(const vsx_vector_2d &t)
  {
    vsx_vector_2d temp;
    temp.x = x/t.x;
    temp.y = y/t.y;
    return temp;
  }

  inline vsx_vector_2d operator /(int t)
  {
    vsx_vector_2d temp;
    temp.x = x/(GLfloat)t;
    temp.y = y/(GLfloat)t;
    return temp;
  }

  inline bool operator ==(const vsx_vector_2d &t)
  {
    return (x==t.x&&y==t.y);
  }

  inline bool operator !=(const vsx_vector_2d &t)
  {
    return (x != t.x || y != t.y);
  }

  inline float length()
  {
    return norm();
  }

  inline float norm()
  {
    return (float)sqrt(x*x + y*y);
  }

  inline void normalize()
  {
    float a = 1.0f/(float)sqrt(x*x + y*y);
    x*=a;
    y*=a;
  }

  inline void normalize(float len)
  {
    float a = 1.0f/(float)sqrt(x*x + y*y)*len;
    x*=a;
    y*=a;
  }


  inline void multiply_matrix(const float *m)
  {
    float nx, ny;
    nx = m[0] * x + m[1] * y  + m[3];
    ny = m[4] * x + m[5] * y  + m[7];
    x = nx;
    y = ny;
  }

  inline void multiply_matrix_other_vec(const float *m, const vsx_vector_2d& b)
  {
    x = m[0] * b.x + m[1] * b.y +  m[3];
    y = m[4] * b.x + m[5] * b.y +  m[7];
  }

  inline double distance(const vsx_vector_2d &otherpoint)
  {
    double dx = otherpoint.x - x;
    double dy = otherpoint.y - y;
    return sqrt(dx*dx+dy*dy);
  }


  void set(const float ix=0, const float iy=0)
  {
    x=ix;
    y=iy;
  }

  vsx_vector_2d()
    :
      x(0.0f),
      y(0.0f)
  {}

  vsx_vector_2d(float nx, float ny = 0.0f)
  {
    x = nx;
    y = ny;
  }
};


#endif
