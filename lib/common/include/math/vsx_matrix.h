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

#include <math/vector/vsx_vector2.h>
#include <math/vector/vsx_vector3.h>

template<typename T = float>
class vsx_matrix
{
public:
  T m[16];

  vsx_matrix() {
    load_identity();
  }

  inline void load_identity()
  {
    m[0] = 1;   m[1] = 0;   m[2] = 0;   m[3] = 0;
    m[4] = 0;   m[5] = 1;   m[6] = 0;   m[7] = 0;
    m[8] = 0;   m[9] = 0;   m[10] = 1;  m[11] = 0;
    m[12] = 0;  m[13] = 0;  m[14] = 0;  m[15] = 1;
  }

  inline void transpose() {
    m[0] = m[0];   m[1] = m[4];   m[2] = m[8];   m[3] = m[12];
    m[4] = m[1];   m[5] = m[5];   m[6] = m[9];   m[7] = m[13];
    m[8] = m[2];   m[9] = m[6];   m[10] = m[10];  m[11] = m[14];
    m[12] = m[3];  m[13] = m[7];  m[14] = m[11];  m[15] = m[15];
  }

  inline vsx_vector3d multiply_vector(const vsx_vector3d &a)
  {
    vsx_vector3d b;
    b.x = m[0] * a.x + m[1] * a.y + m[2]  * a.z + m[3];
    b.y = m[4] * a.x + m[5] * a.y + m[6]  * a.z + m[7];
    b.z = m[8] * a.x + m[9] * a.y + m[10] * a.z + m[11];
    return b;
  }

  template < typename Ti = float>
  inline vsx_vector2<Ti> multiply_vector(const vsx_vector2<Ti> &a)
  {
    vsx_vector2<Ti> b;
    b.x = m[0] * a.x + m[1] * a.y + m[3];
    b.y = m[4] * a.x + m[5] * a.y + m[7];
    return b;
  }

  template < typename Ti = float>
  inline vsx_vector3<Ti> multiply_vector(const vsx_vector3<Ti> &a)
  {
    vsx_vector3<Ti> b;
    b.x = m[0] * a.x + m[1] * a.y + m[2]  * a.z + m[3];
    b.y = m[4] * a.x + m[5] * a.y + m[6]  * a.z + m[7];
    b.z = m[8] * a.x + m[9] * a.y + m[10] * a.z + m[11];
    return b;
  }

  template < typename Ti = float>
  inline vsx_vector3<Ti> multiply_vector_alt(const vsx_vector3<Ti> &a)
  {
    vsx_vector3<Ti> b;
    b.x = m[0] * a.x + m[1] * a.y + m[2]  * a.z + m[12];
    b.y = m[4] * a.x + m[5] * a.y + m[6]  * a.z + m[13];
    b.z = m[8] * a.x + m[9] * a.y + m[10] * a.z + m[14];
    return b;
  }

  /*
   *
   * res.x = mat.row1.x*x + mat.row2.x*y + mat.row3.x*z + mat.row4.x*w;
    res.y = mat.row1.y*x + mat.row2.y*y + mat.row3.y*z + mat.row4.y*w;
    res.z = mat.row1.z*x + mat.row2.z*y + mat.row3.z*z + mat.row4.z*w;
*/


  inline void assign_inverse(vsx_matrix<T> *mm) {
    T d00, d01, d02, d03;
    T d10, d11, d12, d13;
    T d20, d21, d22, d23;
    T d30, d31, d32, d33;

    T m00, m01, m02, m03;
    T m10, m11, m12, m13;
    T m20, m21, m22, m23;
    T m30, m31, m32, m33;
    T D;

    m00 = mm->m[0];  m01 = mm->m[4];  m02 = mm->m[8];  m03 = mm->m[12];
    m10 = mm->m[1];  m11 = mm->m[5];  m12 = mm->m[9];  m13 = mm->m[13];
    m20 = mm->m[2];  m21 = mm->m[6];  m22 = mm->m[10]; m23 = mm->m[14];
    m30 = mm->m[3];  m31 = mm->m[7];  m32 = mm->m[11]; m33 = mm->m[15];

    d00 = m11*m22*m33 + m12*m23*m31 + m13*m21*m32 - m31*m22*m13 - m32*m23*m11 - m33*m21*m12;
    d01 = m10*m22*m33 + m12*m23*m30 + m13*m20*m32 - m30*m22*m13 - m32*m23*m10 - m33*m20*m12;
    d02 = m10*m21*m33 + m11*m23*m30 + m13*m20*m31 - m30*m21*m13 - m31*m23*m10 - m33*m20*m11;
    d03 = m10*m21*m32 + m11*m22*m30 + m12*m20*m31 - m30*m21*m12 - m31*m22*m10 - m32*m20*m11;

    d10 = m01*m22*m33 + m02*m23*m31 + m03*m21*m32 - m31*m22*m03 - m32*m23*m01 - m33*m21*m02;
    d11 = m00*m22*m33 + m02*m23*m30 + m03*m20*m32 - m30*m22*m03 - m32*m23*m00 - m33*m20*m02;
    d12 = m00*m21*m33 + m01*m23*m30 + m03*m20*m31 - m30*m21*m03 - m31*m23*m00 - m33*m20*m01;
    d13 = m00*m21*m32 + m01*m22*m30 + m02*m20*m31 - m30*m21*m02 - m31*m22*m00 - m32*m20*m01;

    d20 = m01*m12*m33 + m02*m13*m31 + m03*m11*m32 - m31*m12*m03 - m32*m13*m01 - m33*m11*m02;
    d21 = m00*m12*m33 + m02*m13*m30 + m03*m10*m32 - m30*m12*m03 - m32*m13*m00 - m33*m10*m02;
    d22 = m00*m11*m33 + m01*m13*m30 + m03*m10*m31 - m30*m11*m03 - m31*m13*m00 - m33*m10*m01;
    d23 = m00*m11*m32 + m01*m12*m30 + m02*m10*m31 - m30*m11*m02 - m31*m12*m00 - m32*m10*m01;

    d30 = m01*m12*m23 + m02*m13*m21 + m03*m11*m22 - m21*m12*m03 - m22*m13*m01 - m23*m11*m02;
    d31 = m00*m12*m23 + m02*m13*m20 + m03*m10*m22 - m20*m12*m03 - m22*m13*m00 - m23*m10*m02;
    d32 = m00*m11*m23 + m01*m13*m20 + m03*m10*m21 - m20*m11*m03 - m21*m13*m00 - m23*m10*m01;
    d33 = m00*m11*m22 + m01*m12*m20 + m02*m10*m21 - m20*m11*m02 - m21*m12*m00 - m22*m10*m01;

    D = 1.0f / (m00*d00 - m01*d01 + m02*d02 - m03*d03);


    m[0] =  d00*D; m[4] = -d10*D;  m[8] =  d20*D; m[12]= -d30*D;
    m[1] = -d01*D; m[5] =  d11*D;  m[9] = -d21*D; m[13]=  d31*D;
    m[2] =  d02*D; m[6] = -d12*D;  m[10]=  d22*D; m[14]= -d32*D;
    m[3] = -d03*D; m[7] =  d13*D;  m[11]= -d23*D; m[15]=  d33*D;
  };

  inline void invert()  {
    int maxsize = 4;
    int actualsize = 4;
    if (actualsize <= 0) return;  // sanity check
    if (actualsize == 1) return;  // must be of dimension >= 2
    for (int i=1; i < actualsize; i++) m[i] /= m[0]; // normalize row 0
    for (int i=1; i < actualsize; i++)  {
      for (int j=i; j < actualsize; j++)  { // do a column of L
        T sum = 0.0;
        for (int k = 0; k < i; k++)
            sum += m[j*maxsize+k] * m[k*maxsize+i];
        m[j*maxsize+i] -= sum;
        }
      if (i == 3) continue;
      for (int j=i+1; j < 4; j++)  {  // do a row of U
        T sum = 0.0;
        for (int k = 0; k < i; k++)
            sum += m[i*maxsize+k]*m[k*maxsize+j];
        m[i*maxsize+j] =
           (m[i*maxsize+j]-sum) / m[i*maxsize+i];
        }
      }
    for ( int i = 0; i < actualsize; i++ )  // invert L
      for ( int j = i; j < actualsize; j++ )  {
        T x = 1.0;
        if ( i != j ) {
          x = 0.0;
          for ( int k = i; k < j; k++ )
              x -= m[j*maxsize+k]*m[k*maxsize+i];
          }
        m[j*maxsize+i] = x / m[j*maxsize+j];
        }
    for ( int i = 0; i < actualsize; i++ )   // invert U
      for ( int j = i; j < actualsize; j++ )  {
        if ( i == j ) continue;
        T sum = 0.0;
        for ( int k = i; k < j; k++ )
            sum += m[k*maxsize+j]*( (i==k) ? 1.0f : m[i*maxsize+k] );
        m[i*maxsize+j] = -sum;
        }
    for ( int i = 0; i < actualsize; i++ )   // final inversion
      for ( int j = 0; j < actualsize; j++ )  {
        T sum = 0.0;
        for ( int k = ((i>j)?i:j); k < actualsize; k++ )
          sum += ((j==k)?1.0f:m[j*maxsize+k])*m[k*maxsize+i];
        m[j*maxsize+i] = sum;
        }
    }

  inline void multiply(vsx_matrix<T> *a, vsx_matrix<T> *b) {
    int i, j;
    // TODO: do this directly into m?
    T mm[16];
    for (i = 0; i < 4; i++)
    {
      int ii = i*4;
      for (j = 0; j < 4; j++)
      {
        mm[ii+j] = a->m[ii] * b->m[j] +
                  a->m[ii+1] * b->m[4+j] +
                  a->m[ii+2] * b->m[8+j] +
                  a->m[ii+3] * b->m[12+j];
      }
    }
    memcpy(&m,&mm,sizeof(T)*16);
  }


  inline void rotation_from_vectors(vsx_vector3<T>* dir)
  {
    vsx_vector3<T> x, y, z;
    z = *dir;
    z.normalize();
    y = vsx_vector3<T>(-z.y*z.x, 1-z.y*z.y, -z.y*z.z);
    y.normalize();

    x.cross(y,z);
    x.normalize();

    m[0] = x.x; m[4] = x.y; m[8] = x.z;
    m[1] = y.x; m[5] = y.y; m[9] = y.z;
    m[2] = z.x; m[6] = z.y; m[10] = z.z;
  }

  inline void rotation_from_vectors_n(vsx_vector2<T>* xv, vsx_vector3<T>* yv)
  {
    vsx_vector3<T> x, y, z;
    x.x = xv->x;
    x.y = xv->y;
    x.normalize();

    y = *yv;
    y.normalize();

    z.cross(x,y);
    z.normalize();

    m[0] = x.x; m[4] = x.y; m[8] = x.z;
    m[1] = y.x; m[5] = y.y; m[9] = y.z;
    m[2] = z.x; m[6] = z.y; m[10] = z.z;
  }


  inline void rotation_from_vectors_n(vsx_vector3<T>* xv, vsx_vector3<T>* yv)
  {
    vsx_vector3<T> x, y, z;
    x = *xv;
    x.normalize();
    y = *yv;
    y.normalize();

    z.cross(x,y);
    z.normalize();

    m[0] = x.x; m[4] = x.y; m[8] = x.z;
    m[1] = y.x; m[5] = y.y; m[9] = y.z;
    m[2] = z.x; m[6] = z.y; m[10] = z.z;
  }


  #ifdef DEBUG
  void dump(char* label="n/a")
  {
    printf(
      "Name: %s. 0: %f 1: %f 2: %f 3: %f\n"
      "4: %f 5: %f 6: %f 7: %f\n"
      "8: %f 9: %f 10: %f 11: %f\n"
      "12: %f 13: %f 14: %f 15: %f\n",
      label,
      m[0],m[1],m[2],m[3],
      m[4],m[5],m[6],m[7],
      m[8],m[9],m[10],m[11],
      m[12],m[13],m[14],m[15]
    );
  }
  #endif

};

