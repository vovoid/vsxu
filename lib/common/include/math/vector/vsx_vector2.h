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

#include <math/vsx_math.h>
#include <math/vector/vsx_vector3.h>

VSX_PACK_BEGIN
template<typename T = float>
class vsx_vector2
{
public:
    T x = (T)0.0;
    T y = (T)0.0;

  static size_t arity()
  {
    return 2;
  }

  template<typename D>
  inline vsx_vector2<D> convert()
  {
    return vsx_vector2<D>((D)x, (D)y);
  }

  inline vsx_vector2 operator +(const vsx_vector2 &t)
  {
    vsx_vector2 temp;
    temp.x = x+t.x;
    temp.y = y+t.y;
    return temp;
  }

  inline vsx_vector2 operator +=(const vsx_vector2 &t)
  {
    x+=t.x;
    y+=t.y;
    return *this;
  }

  inline vsx_vector2 operator *=(const T &t)
  {
    x*=t;
    y*=t;
    return *this;
  }

  inline vsx_vector2 operator -=(const vsx_vector2 &t)
  {
    x-=t.x;
    y-=t.y;
    return *this;
  }

  inline vsx_vector2 operator -()
  {
    vsx_vector2 temp;
    temp.x = -x;
    temp.y = -y;
    return temp;
  }

  inline vsx_vector2 operator -(const vsx_vector2 &t)
  {
    vsx_vector2 temp;
    temp.x = x-t.x;
    temp.y = y-t.y;
    return temp;
  }

  inline vsx_vector2 operator *(const vsx_vector2 &t)
  {
    vsx_vector2 temp;
    temp.x = x*t.x;
    temp.y = y*t.y;
    return temp;
  }

  inline vsx_vector2 operator *(const T &t)
  {
    vsx_vector2 temp;
    temp.x = x*t;
    temp.y = y*t;
    return temp;
  }

  inline vsx_vector2 operator /(const vsx_vector2 &t)
  {
    vsx_vector2 temp;
    temp.x = x/t.x;
    temp.y = y/t.y;
    return temp;
  }

  inline vsx_vector2 operator /(int t)
  {
    vsx_vector2 temp;
    temp.x = x/(T)t;
    temp.y = y/(T)t;
    return temp;
  }

  inline bool operator ==(const vsx_vector2 &t)
  {
    return (fabs(x - t.x) < 0.00001 || fabs(y - t.y) < 0.00001);
  }

  inline bool operator !=(const vsx_vector2 &t)
  {
    return (fabs(x - t.x) > 0.00001 || fabs(y - t.y) > 0.00001);
  }

  inline T length()
  {
    return norm();
  }

  inline T norm()
  {
    return (T)sqrt(x*x + y*y);
  }

  inline void normalize()
  {
    T a = 1.0f/(T)sqrt(x*x + y*y);
    x*=a;
    y*=a;
  }

  inline void normalize(T len)
  {
    T a = 1.0f/(T)sqrt(x*x + y*y)*len;
    x*=a;
    y*=a;
  }

  inline T dot_product(const vsx_vector2* ov)
  {
    return x * ov->x + y * ov->y;
  }

  // calculates the cross product of the two other vectors and stores it here
  inline void cross(const vsx_vector2 &u, float z = 1.0f) {
    x =  (u.y * z);
    y = -(u.x * z);
  }



  inline void multiply_matrix(const T *m)
  {
    T nx, ny;
    nx = m[0] * x + m[1] * y  + m[3];
    ny = m[4] * x + m[5] * y  + m[7];
    x = nx;
    y = ny;
  }

  inline void multiply_matrix_other_vec(const T *m, const vsx_vector2& b)
  {
    x = m[0] * b.x + m[1] * b.y +  m[3];
    y = m[4] * b.x + m[5] * b.y +  m[7];
  }

  inline double distance(const vsx_vector2 &otherpoint)
  {
    double dx = otherpoint.x - x;
    double dy = otherpoint.y - y;
    return sqrt(dx*dx+dy*dy);
  }


  void set(const T ix=0, const T iy=0)
  {
    x=ix;
    y=iy;
  }

  bool nan()
  {
    if (isnan(x))
      return true;
    if (isnan(y))
      return true;
    return false;
  }

  vsx_vector2()
    :
      x(0.0f),
      y(0.0f)
  {}

  vsx_vector2(const T n[2])
  {
    x = n[0];
    y = n[1];
  }

  vsx_vector2(const vsx_vector3<T> &n)
  {
    x = n.x;
    y = n.y;
  }

  vsx_vector2(const T nx)
  {
    x = nx;
  }

  vsx_vector2(const T nx, const T ny)
  {
    x = nx;
    y = ny;
  }
}
VSX_PACK_END

typedef vsx_vector2<float> vsx_vector2f;
typedef vsx_vector2<double> vsx_vector2d;

#endif
