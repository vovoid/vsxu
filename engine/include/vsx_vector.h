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

#include <vsx_math.h>

class vsx_vector
{
public:
  union
  {
    GLfloat x;
    GLfloat h;
    GLfloat r;
    GLfloat pitch;
  };
  union
  {
    GLfloat y;
    GLfloat s;
    GLfloat g;
    GLfloat roll;
  };
  union
  {
    GLfloat z;
    GLfloat v;
    GLfloat b;
    GLfloat w;
    GLfloat yaw;
  };

  inline vsx_vector operator +(const vsx_vector &t)
  {
    vsx_vector temp;
    temp.x = x+t.x;
    temp.y = y+t.y;
    temp.z = z+t.z;
    return temp;
  }

  inline vsx_vector operator +=(const vsx_vector &t)
  {
    x+=t.x;
    y+=t.y;
    z+=t.z;
    return *this;
  }

  inline vsx_vector operator *=(const float &t)
  {
    x*=t;
    y*=t;
    z*=t;
    return *this;
  }

  inline vsx_vector operator -=(const vsx_vector &t)
  {
    x-=t.x;
    y-=t.y;
    z-=t.z;
    return *this;
  }

  inline vsx_vector operator -()
  {
    vsx_vector temp;
    temp.x = -x;
    temp.y = -y;
    temp.z = -z;
    return temp;
  }

  inline vsx_vector operator -(const vsx_vector &t)
  {
    vsx_vector temp;
    temp.x = x-t.x;
    temp.y = y-t.y;
    temp.z = z-t.z;
    return temp;
  }

  inline vsx_vector operator *(const vsx_vector &t)
  {
    vsx_vector temp;
    temp.x = x*t.x;
    temp.y = y*t.y;
    temp.z = z*t.z;
    return temp;
  }

  inline vsx_vector operator *(const float &t)
  {
    vsx_vector temp;
    temp.x = x*t;
    temp.y = y*t;
    temp.z = z*t;
    return temp;
  }

  inline vsx_vector operator /(const vsx_vector &t)
  {
    vsx_vector temp;
    temp.x = x/t.x;
    temp.y = y/t.y;
    temp.z = z/t.z;
    return temp;
  }

  inline vsx_vector operator /(int t)
  {
    vsx_vector temp;
    temp.x = x/(GLfloat)t;
    temp.y = y/(GLfloat)t;
    temp.z = z/(GLfloat)t;
    return temp;
  }

  inline bool operator ==(const vsx_vector &t)
  {
    return (x==t.x&&y==t.y&&z==t.z);
  }

  inline bool operator !=(const vsx_vector &t)
  {
    return (x!=t.x||y!=t.y||z!=t.z);
  }

  // calculates the cross product of the two other vectors and stores it here
  inline void cross(const vsx_vector &u, const vsx_vector &vv) {
    x = (u.y*vv.z) - (u.z*vv.y);
    y = (u.z*vv.x) - (u.x*vv.z);
    z = (u.x*vv.y) - (u.y*vv.x);
  }

  inline float dot_product(const vsx_vector* ov)
  {
    return x * ov->x   +   y * ov->y   +   z * ov->z;
  }

  inline float length()
  {
    return norm();
  }

  inline float norm()
  {
    return (float)sqrt(x*x + y*y + z*z);
  }

  inline void normalize()
  {
    float a = 1.0f/(float)sqrt(x*x + y*y + z*z);
    x*=a;
    y*=a;
    z*=a;
  }

  inline void normalize(float len)
  {
    float a = 1.0f/(float)sqrt(x*x + y*y + z*z)*len;
    x*=a;
    y*=a;
    z*=a;
  }

  inline void midpoint(const vsx_vector& a, const vsx_vector& b)
  {
    x = (a.x + b.x) * 0.5;
    y = (a.y + b.y) * 0.5;
    z = (a.z + b.z) * 0.5;
  }


  inline void assign_face_normal(vsx_vector *a, vsx_vector*bb, vsx_vector*c)
  {
    float relx1 = bb->x - a->x,
    rely1 = bb->y - a->y,
    relz1 = bb->z - a->z,
    relx2 = c->x - a->x,
    rely2 = c->y - a->y,
    relz2 = c->z - a->z;
    x = (rely1 * relz2 - relz1 * rely2);
    y = (relz1 * relx2 - relx1 * relz2);
    z = (relx1 * rely2 - rely1 * relx2);
  }

  inline void multiply_matrix(const float *m)
  {
    float nx, ny, nz;
    nx = m[0] * x + m[1] * y + m[2]  * z + m[3];
    ny = m[4] * x + m[5] * y + m[6]  * z + m[7];
    nz = m[8] * x + m[9] * y + m[10] * z + m[11];
    x = nx;
    y = ny;
    z = nz;
  }

  inline void multiply_matrix_other_vec(const float *m,const vsx_vector& b)
  {
    x = m[0] * b.x + m[1] * b.y + m[2]  * b.z + m[3];
    y = m[4] * b.x + m[5] * b.y + m[6]  * b.z + m[7];
    z = m[8] * b.x + m[9] * b.y + m[10] * b.z + m[11];
  }

  inline double distance(const vsx_vector &otherpoint)
  {
    double dx = otherpoint.x - x;
    double dy = otherpoint.y - y;
    double dz = otherpoint.z - z;
    return sqrt(dx*dx+dy*dy+dz*dz);
  }


  void set(const float ix=0, const float iy=0, const float iz=0)
  {
    x=ix;
    y=iy;
    z=iz;
  }

  vsx_vector()
    :
      x(0.0f),
      y(0.0f),
      z(0.0f)
  {}

  vsx_vector(float a, float bb = 0.0f, float c = 0.0f)
  {
    x = a;
    y = bb;
    z = c;
  }
};


#endif
