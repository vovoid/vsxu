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

#ifndef VSX_MATH_H
#define VSX_MATH_H

#include <stdio.h>
#include <math.h>
#include <cstring>
#include <stdint.h>

#define TWO_PI 3.1415926535897932384626433832795 * 2.0
#define PI 3.1415926535897932384626433832795
#define PI_FLOAT 3.1415926535897932384626433832795f
#define HALF_PI 1.5707963267948966192313216916398

#define SGN(N) (N >= 0 ? 1 : -1)
#define MAX(N, M) ((N) >= (M) ? (N) : (M))
#define MIN(N, M) ((N) <= (M) ? (N) : (M))
#define CLAMP(N, L, U) (MAX(MIN((N), (U)), (L)))


#ifndef __gl_h_
typedef float GLfloat;
#endif


inline void v_norm(float *v)
{
  GLfloat len = 1.0f / (GLfloat)sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  v[0] *= len;
  v[1] *= len;
  v[2] *= len;
}


inline void mat_vec_mult(float *m, float *a, float *b)
{
  b[0] = m[0] * a[0] + m[1] * a[1] + m[2]  * a[2] + m[3];
  b[1] = m[4] * a[0] + m[5] * a[1] + m[6]  * a[2] + m[7];
  b[2] = m[8] * a[0] + m[9] * a[1] + m[10] * a[2] + m[11];
  return;
}



inline void mat_vec_mult3x3(float *m, float *a, float *b)
{
  b[0] = m[0] * a[0] + m[3] * a[1] + m[6]  * a[2];
  b[1] = m[1] * a[0] + m[4] * a[1] + m[7]  * a[2];
  b[2] = m[2] * a[0] + m[5] * a[1] + m[8]  * a[2];
  return;
}





#endif
