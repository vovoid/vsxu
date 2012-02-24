/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies Copyright (C) 2003-2011
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include <math.h>
#ifndef VSX_NO_GL
#include "vsx_gl_global.h"
#endif
#include "vsx_math_3d.h"
#include <mtwist.h>

double vsx_vector::distance(vsx_vector otherpoint) {
  double dx = otherpoint.x - x;
  double dy = otherpoint.y - y;
  double dz = otherpoint.z - z;
  //printf("xyz %f %f %f\n",dx,dy,dz);
  return sqrt(dx*dx+dy*dy+dz*dz);
}  

//-------------------------------------------------------------------------------------------------------

vsx_color vsx_color__(float x, float y, float z, float a) {
  vsx_color foo;
  foo.x = x;
  foo.y = y;
  foo.z = z;
  foo.a = a;
  return foo;
}




float getmax(float foo,float bar)
{
  if (foo>=bar) return foo;
  return bar;
}

float getmin(float foo,float bar)
{
  if (foo<=bar) return foo;
  return bar;
}

float getrange(float value,float min,float max)
{
  return getmin(getmax(value,min),max);
}

int z_round(float val)
{
  return (int)(trunc(val+(val>=0.0?0.5:-0.5)));  
}

double f_round(float val, int p)
{
  /*double i = pow(0.1,(float)p);
  double t = fmod(val,i);
  if (t < i*0.5)
  return val - t;
  else return val + t;*/
  double t=pow(10.0,p);
  return (double)z_round((float)((double)val*t))/t;
}

void v_norm(float *v) {
	GLfloat len = 1.0f / (GLfloat)sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] *= len;
	v[1] *= len;
	v[2] *= len;
}

void inv_mat( float *m, float *r) 
{
  float d00, d01, d02, d03;
  float d10, d11, d12, d13;
  float d20, d21, d22, d23;
  float d30, d31, d32, d33;

  float m00, m01, m02, m03;
  float m10, m11, m12, m13;
  float m20, m21, m22, m23;
  float m30, m31, m32, m33;
  float D;

  m00 = m[0];  m01 = m[4];  m02 = m[8];  m03 = m[12];
  m10 = m[1];  m11 = m[5];  m12 = m[9];  m13 = m[13];
  m20 = m[2];  m21 = m[6];  m22 = m[10]; m23 = m[14];
  m30 = m[3];  m31 = m[7];  m32 = m[11]; m33 = m[15];

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

  D = m00*d00 - m01*d01 + m02*d02 - m03*d03;

  r[0] =  d00/D; r[4] = -d10/D;  r[8] =  d20/D; r[12]= -d30/D;
  r[1] = -d01/D; r[5] =  d11/D;  r[9] = -d21/D; r[13]=  d31/D;
  r[2] =  d02/D; r[6] = -d12/D;  r[10]=  d22/D; r[14]= -d32/D;
  r[3] = -d03/D; r[7] =  d13/D;  r[11]= -d23/D; r[15]=  d33/D;
}

void mat_vec_mult(GLfloat *m, GLfloat *a, GLfloat *b)
{
  b[0] = m[0] * a[0] + m[1] * a[1] + m[2]  * a[2] + m[3];
  b[1] = m[4] * a[0] + m[5] * a[1] + m[6]  * a[2] + m[7];
  b[2] = m[8] * a[0] + m[9] * a[1] + m[10] * a[2] + m[11];
	return;
}

void mat_vec_mult3x3(float *m, float *a, float *b) {
  b[0] = m[0] * a[0] + m[3] * a[1] + m[6]  * a[2];
  b[1] = m[1] * a[0] + m[4] * a[1] + m[7]  * a[2];
  b[2] = m[2] * a[0] + m[5] * a[1] + m[8]  * a[2];
	return;
}  

void Calc_Face_Normal(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float &nx, float &ny, float &nz)
{
	float relx1 = x2 - x1,
	rely1 = y2 - y1,
	relz1 = z2 - z1,
	relx2 = x3 - x1,
	rely2 = y3 - y1,
	relz2 = z3 - z1;
	nx = (rely1 * relz2 - relz1 * rely2);
	ny = (relz1 * relx2 - relx1 * relz2);
	nz = (relx1 * rely2 - rely1 * relx2);
}



vsx_rand::vsx_rand()
{
  state = (void*)new mt_state;
  srand(1);
}

vsx_rand::~vsx_rand()
{
  delete (mt_state*)state;
}

void vsx_rand::srand(uint32_t seed)
{
  mts_seed32((mt_state*)state,seed);
}
uint32_t vsx_rand::rand()
{
  return mts_lrand((mt_state*)state);
}

float vsx_rand::frand()
{
  return (float)mts_drand((mt_state*)state);
}




