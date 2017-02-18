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


#pragma once

#include <math/vector/vsx_vector3.h>
#include <math/vsx_matrix.h>

inline float vsx_math_3d_max(const float& x, const float& y)
{
  return ( x > y ) ? x : y;
}

inline double vsx_math_3d_max(const double& x, const double& y)
{
  return ( x > y ) ? x : y;
}

VSX_PACK_BEGIN
template<typename T = float>
class vsx_quaternion
{
public:
  T x = 0.0;
  T y = 0.0;
  T z = 0.0;
  T w = 1.0;
  
  vsx_quaternion()
  {
  }

  vsx_quaternion(T xx, T yy, T zz)
  {
    x = xx;
    y = yy;
    z = zz;
  }

  vsx_quaternion(T xx, T yy, T zz, T ww)
  {
    x = xx;
    y = yy;
    z = zz;
    w = ww;
  }

  template<typename D>
  inline vsx_quaternion<D> convert()
  {
    return vsx_quaternion<D>((D)x, (D)y, (D)z, (D)w);
  }


  static size_t arity()
  {
    return 4;
  }

  inline void cos_slerp(vsx_quaternion& from, vsx_quaternion& to, T t)
  {
    slerp(from,to,(T)sin(t*(T)HALF_PI));
  }

  inline void slerp(vsx_quaternion& from, vsx_quaternion& to, T t)
  {
    T         to1[4];
    T        omega, cosom, sinom, scale0, scale1;
    // calc cosine
    cosom = from.x * to.x + from.y * to.y + from.z * to.z + from.w * to.w;
    // adjust signs (if necessary)
    if (cosom < (T)0.0)
    { 
      cosom = -cosom; to1[0] = - to.x;
  		to1[1] = - to.y;
  		to1[2] = - to.z;
  		to1[3] = - to.w;
    } else {
  		to1[0] = to.x;
  		to1[1] = to.y;
  		to1[2] = to.z;
  		to1[3] = to.w;
    }
    // calculate coefficients
    if ( ((T)1.0 - cosom) > (T)0.00001 )
    {
      // standard case (slerp)
      omega = (T)acos(cosom);
      sinom = (T)sin(omega);
      scale0 = (T)sin(((T)1.0 - t) * omega) / sinom;
      scale1 = (T)sin(t * omega) / sinom;
    }
    else
    {
      // "from" and "to" quaternions are very close 
      //  ... so we can do a linear interpolation
      scale0 = (T)1.0 - t;
      scale1 = t;
    }
  	// calculate final values
    x = (T)(scale0 * from.x + scale1 * to1[0]);
    y = (T)(scale0 * from.y + scale1 * to1[1]);
    z = (T)(scale0 * from.z + scale1 * to1[2]);
    w = (T)(scale0 * from.w + scale1 * to1[3]);
  }
  
  vsx_quaternion operator +=(const vsx_quaternion &t)
  {
    x+=t.x;
    y+=t.y;
    z+=t.z;
    w+=t.w;
    return *this;
  }

  vsx_quaternion operator -(const vsx_quaternion &t) const
  {
    vsx_quaternion temp;
    temp.x = x-t.x;
    temp.y = y-t.y;
    temp.z = z-t.z;
    return temp;
  }

  vsx_quaternion operator -(const vsx_vector3<T> &t) const
  {
    vsx_quaternion temp;
    temp.x = x-t.x;
    temp.y = y-t.y;
    temp.z = z-t.z;
    return temp;
  }

  vsx_quaternion operator *(const vsx_quaternion<T> &t) const
  {
    vsx_quaternion temp;
    temp.x =  x * t.w + y * t.z - z * t.y + w * t.x;
    temp.y = -x * t.z + y * t.w + z * t.x + w * t.y;
    temp.z =  x * t.y - y * t.x + z * t.w + w * t.z;
    temp.w = -x * t.x - y * t.y - z * t.z + w * t.w;
    return temp;
  }

  vsx_quaternion operator *=(const vsx_quaternion<T> &t)
  {
    x =  x * t.w + y * t.z - z * t.y + w * t.x;
    y = -x * t.z + y * t.w + z * t.x + w * t.y;
    z =  x * t.y - y * t.x + z * t.w + w * t.z;
    w = -x * t.x - y * t.y - z * t.z + w * t.w;
    return *this;
  }


  vsx_quaternion operator *(const vsx_vector3<T> &t) const
  {
    vsx_quaternion temp;
    temp.x = x*t.x;
    temp.y = y*t.y;
    temp.z = z*t.z;
    return temp;
  }


  inline T dot_product(vsx_vector3<T>* ov) const
  {
    return x * ov->x   +   y * ov->y   +   z * ov->z;
  }


  inline void from_axis_angle( vsx_vector3<T> &source_axis, T &source_angle)
  {
    T f = (T)sin( source_angle * (T)0.5 );
    x = source_axis.x * f;
    y = source_axis.y * f;
    z = source_axis.z * f;
    w = (T)cos( source_angle * (T)0.5 );
  }

  inline void to_axis_angle( vsx_vector3<T> &result_axis, T &result_angle)
  {
    result_angle = 2 * acos(w);
    T sq = 1.0f / sqrt(1-w*w);
    result_axis.x = x * sq;
    result_axis.y = y * sq;
    result_axis.z = z * sq;
  }

  inline void mul(const vsx_quaternion &q1, const vsx_quaternion &q2)
  {
    x =  q1.x * q2.w + q1.y * q2.z - q1.z * q2.y + q1.w * q2.x;
    y = -q1.x * q2.z + q1.y * q2.w + q1.z * q2.x + q1.w * q2.y;
    z =  q1.x * q2.y - q1.y * q2.x + q1.z * q2.w + q1.w * q2.z;
    w = -q1.x * q2.x - q1.y * q2.y - q1.z * q2.z + q1.w * q2.w;
  }
  
  inline vsx_matrix<T> matrix()
  {
    vsx_matrix<T> mat;
    T n, s;
    T xs, ys, zs;
    T wx, wy, wz;
    T xx, xy, xz;
    T yy, yz, zz;

    n = (x * x) + (y * y) + (z * z) + (w * w);
    s = (n > 0.0f) ? (2.0f / n) : 0.0f;

    xs = x * s;  ys = y * s;  zs = z * s;
    wx = w * xs; wy = w * ys; wz = w * zs;
    xx = x * xs; xy = x * ys; xz = x * zs;
    yy = y * ys; yz = y * zs; zz = z * zs;

    mat.m[0]     = 1.0f - (yy + zz); mat.m[1]     =         xy - wz;  mat.m[2]     =         xz + wy;
    mat.m[4]     =         xy + wz;  mat.m[5]     = 1.0f - (xx + zz); mat.m[6]     =         yz - wx;
    mat.m[8]     =         xz - wy;  mat.m[9]     =         yz + wx;  mat.m[10]    = 1.0f - (xx + yy);
    return mat;
  }

  inline void from_matrix(vsx_matrix<T>* mm)
  {
    #define m00 mm->m[0]
    #define m01 mm->m[4]
    #define m02 mm->m[8]
    #define m03 mm->m[12]
    #define m10 mm->m[1]
    #define m11 mm->m[5]
    #define m12 mm->m[9]
    #define m13 mm->m[13]
    #define m20 mm->m[2]
    #define m21 mm->m[6]
    #define m22 mm->m[10]
    #define m23 mm->m[14]
    #define m30 mm->m[3]
    #define m31 mm->m[7]
    #define m32 mm->m[11]
    #define m33 mm->m[15]

    /*#define m00 mm->m[0]
    #define m01 mm->m[1]
    #define m02 mm->m[2]
    #define m03 mm->m[3]
    #define m10 mm->m[4]
    #define m11 mm->m[5]
    #define m12 mm->m[6]
    #define m13 mm->m[7]
    #define m20 mm->m[8]
    #define m21 mm->m[9]
    #define m22 mm->m[10]
    #define m23 mm->m[11]
    #define m30 mm->m[12]
    #define m31 mm->m[13]
    #define m32 mm->m[14]
    #define m33 mm->m[15]
*/
    w = (T)sqrt( vsx_math_3d_max( 0, 1 + m00 + m11 + m22 ) ) * (T)0.5;
    x = (T)sqrt( vsx_math_3d_max( 0, 1 + m00 - m11 - m22 ) ) * (T)0.5;
    y = (T)sqrt( vsx_math_3d_max( 0, 1 - m00 + m11 - m22 ) ) * (T)0.5;
    z = (T)sqrt( vsx_math_3d_max( 0, 1 - m00 - m11 + m22 ) ) * (T)0.5;

    x = copysign( x, m21 - m12 );
    y = copysign( y, m02 - m20 );
    z = copysign( z, m10 - m01 );
    
  }

  
  // normalizes the quaternion ensuring it stays a unit-quaternion
  inline void normalize()
  {
    T len = (T)(1.0 / sqrt(
      x*x +
      y*y +
      z*z +
      w*w
    ));
    x *= len;
    y *= len;
    z *= len;
    w *= len;
  }

  vsx_vector3<T> transform(const vsx_vector3<T> &p1)
  {
    vsx_vector3<T> p2;
    p2.x = w*w*p1.x + 2*y*w*p1.z - 2*z*w*p1.y + x*x*p1.x + 2*y*x*p1.y + 2*z*x*p1.z - z*z*p1.x - y*y*p1.x;
    p2.y = 2*x*y*p1.x + y*y*p1.y + 2*z*y*p1.z + 2*w*z*p1.x - z*z*p1.y + w*w*p1.y - 2*x*w*p1.z - x*x*p1.y;
    p2.z = 2*x*z*p1.x + 2*y*z*p1.y + z*z*p1.z - 2*w*y*p1.x - y*y*p1.z + 2*w*x*p1.y - x*x*p1.z + w*w*p1.z;
    return p2;
  }

  vsx_vector2<T> transform(const vsx_vector2<T> &p1)
  {
    vsx_vector2<T> p2;
    p2.x = w*w*p1.x - 2*z*w*p1.y + x*x*p1.x + 2*y*x*p1.y - z*z*p1.x - y*y*p1.x;
    p2.y = 2*x*y*p1.x + y*y*p1.y + 2*w*z*p1.x - z*z*p1.y + w*w*p1.y - x*x*p1.y;
    return p2;
  }
  }
VSX_PACK_END
