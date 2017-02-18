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


#ifndef VSX_VECTOR_H
#define VSX_VECTOR_H

#include <vsx_platform.h>
#include <inttypes.h>
#include <math/vsx_math.h>

VSX_PACK_BEGIN
template<typename T = float>
class vsx_vector3
{
public:
  T x = (T)0.0;
  T y = (T)0.0;
  T z = (T)0.0;

  template<typename D>
  inline vsx_vector3<D> convert()
  {
    return vsx_vector3<D>((D)x, (D)y, (D)z);
  }

  static size_t arity()
  {
    return 3;
  }

  inline vsx_vector3 operator +(const vsx_vector3 &t)
  {
    vsx_vector3 temp;
    temp.x = x+t.x;
    temp.y = y+t.y;
    temp.z = z+t.z;
    return temp;
  }

  inline vsx_vector3 operator +=(const vsx_vector3 &t)
  {
    x+=t.x;
    y+=t.y;
    z+=t.z;
    return *this;
  }

  inline vsx_vector3 operator *=(const T &t)
  {
    x*=t;
    y*=t;
    z*=t;
    return *this;
  }

  inline vsx_vector3 operator -=(const vsx_vector3 &t)
  {
    x-=t.x;
    y-=t.y;
    z-=t.z;
    return *this;
  }

  inline vsx_vector3 operator -()
  {
    vsx_vector3 temp;
    temp.x = -x;
    temp.y = -y;
    temp.z = -z;
    return temp;
  }

  inline vsx_vector3 operator -(const vsx_vector3 &t)
  {
    vsx_vector3 temp;
    temp.x = x-t.x;
    temp.y = y-t.y;
    temp.z = z-t.z;
    return temp;
  }

  inline vsx_vector3 operator *(const vsx_vector3 &t)
  {
    vsx_vector3 temp;
    temp.x = x*t.x;
    temp.y = y*t.y;
    temp.z = z*t.z;
    return temp;
  }

  inline vsx_vector3 operator *(const T &t)
  {
    vsx_vector3 temp;
    temp.x = x*t;
    temp.y = y*t;
    temp.z = z*t;
    return temp;
  }

  inline vsx_vector3 operator /(const vsx_vector3 &t)
  {
    vsx_vector3 temp;
    temp.x = x/t.x;
    temp.y = y/t.y;
    temp.z = z/t.z;
    return temp;
  }

  inline vsx_vector3 operator /(int t)
  {
    vsx_vector3 temp;
    temp.x = x/(T)t;
    temp.y = y/(T)t;
    temp.z = z/(T)t;
    return temp;
  }

  inline bool operator ==(const vsx_vector3 &t)
  {
    return (x==t.x&&y==t.y&&z==t.z);
  }

  inline bool operator !=(const vsx_vector3 &t)
  {
    return (x!=t.x||y!=t.y||z!=t.z);
  }

  // calculates the cross product of the two other vectors and stores it here
  inline void cross(const vsx_vector3 &u, const vsx_vector3 &vv) {
    x = (u.y*vv.z) - (u.z*vv.y);
    y = (u.z*vv.x) - (u.x*vv.z);
    z = (u.x*vv.y) - (u.y*vv.x);
  }

  inline T dot_product(const vsx_vector3* ov)
  {
    return x * ov->x   +   y * ov->y   +   z * ov->z;
  }

  inline T length()
  {
    return norm();
  }

  inline T norm()
  {
    return (T)sqrt(x*x + y*y + z*z);
  }

  inline void normalize()
  {
    T a = 1.0f/(T)sqrt(x*x + y*y + z*z);
    x*=a;
    y*=a;
    z*=a;
  }

  inline void normalize(T len)
  {
    T a = 1.0f/(T)sqrt(x*x + y*y + z*z)*len;
    x*=a;
    y*=a;
    z*=a;
  }

  inline void midpoint(const vsx_vector3& a, const vsx_vector3& b)
  {
    x = (a.x + b.x) * (T)0.5;
    y = (a.y + b.y) * (T)0.5;
    z = (a.z + b.z) * (T)0.5;
  }


  inline void assign_face_normal(vsx_vector3 *a, vsx_vector3* bb, vsx_vector3*c)
  {
    T relx1 = bb->x - a->x,
    rely1 = bb->y - a->y,
    relz1 = bb->z - a->z,
    relx2 = c->x - a->x,
    rely2 = c->y - a->y,
    relz2 = c->z - a->z;
    x = (rely1 * relz2 - relz1 * rely2);
    y = (relz1 * relx2 - relx1 * relz2);
    z = (relx1 * rely2 - rely1 * relx2);
  }

  inline void multiply_matrix(const T *m)
  {
    T nx, ny, nz;
    nx = m[0] * x + m[1] * y + m[2]  * z + m[3];
    ny = m[4] * x + m[5] * y + m[6]  * z + m[7];
    nz = m[8] * x + m[9] * y + m[10] * z + m[11];
    x = nx;
    y = ny;
    z = nz;
  }

  inline void multiply_matrix_other_vec(const T *m,const vsx_vector3& b)
  {
    x = m[0] * b.x + m[1] * b.y + m[2]  * b.z + m[3];
    y = m[4] * b.x + m[5] * b.y + m[6]  * b.z + m[7];
    z = m[8] * b.x + m[9] * b.y + m[10] * b.z + m[11];
  }

  inline double distance(const vsx_vector3 &otherpoint)
  {
    double dx = otherpoint.x - x;
    double dy = otherpoint.y - y;
    double dz = otherpoint.z - z;
    return sqrt(dx*dx + dy*dy + dz*dz);
  }


  void set(const T ix=0, const T iy=0, const T iz=0)
  {
    x=ix;
    y=iy;
    z=iz;
  }

  bool nan()
  {
    if (isnan(x))
      return true;
    if (isnan(y))
      return true;
    if (isnan(z))
      return true;
    return false;
  }


  vsx_vector3()
    :
      x(0.0f),
      y(0.0f),
      z(0.0f)
  {}

  vsx_vector3(const T n[3])
  {
    x = n[0];
    y = n[1];
    z = n[2];
  }

  vsx_vector3(T a)
  {
    x = a;
  }

  vsx_vector3(T a, T b)
  {
    x = a;
    y = b;
  }

  vsx_vector3(T a, T b, T c)
  {
    x = a;
    y = b;
    z = c;
  }
}
VSX_PACK_END

typedef vsx_vector3<float> vsx_vector3f;
typedef vsx_vector3<double> vsx_vector3d;


#endif
