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

#ifndef VSX_3D_FUNC
#define VSX_3D_FUNC

#include <stdio.h>
#include <math.h>
#include <cstring>
#include <stdint.h>

#define TWO_PI 3.1415926535897932384626433832795 * 2.0
#define PI 3.1415926535897932384626433832795
#define PI_FLOAT 3.1415926535897932384626433832795f
#define HALF_PI 1.5707963267948966192313216916398

#include <vsx_platform.h>

#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
#define VSX_MATH_3D_DLLIMPORT
#else
  #ifdef VSX_ENG_DLL
    #define VSX_MATH_3D_DLLIMPORT __declspec (dllexport) 
  #else 
    #define VSX_MATH_3D_DLLIMPORT __declspec (dllimport)
  #endif
#endif


#ifndef __gl_h_
typedef float GLfloat;
#endif

class vsx_vector {
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
    GLfloat jaw;
  };

  vsx_vector operator +(const vsx_vector &t) {
    vsx_vector temp;
    temp.x = x+t.x;
    temp.y = y+t.y;
    temp.z = z+t.z;
    return temp;
  }

  vsx_vector operator +=(const vsx_vector &t) {
    x+=t.x;
    y+=t.y;
    z+=t.z;
    return *this;
  }

  vsx_vector operator *=(const float &t) {
    x*=t;
    y*=t;
    z*=t;
    return *this;
  }

  vsx_vector operator -=(const vsx_vector &t) {
    x-=t.x;
    y-=t.y;
    z-=t.z;
    return *this;
  }

  vsx_vector operator -() {
  	vsx_vector temp;
    temp.x = -x;
    temp.y = -y;
    temp.z = -z;
    return temp;
  }

  vsx_vector operator -(const vsx_vector &t) {
    vsx_vector temp;
    temp.x = x-t.x;
    temp.y = y-t.y;
    temp.z = z-t.z;
    return temp;
  }

  vsx_vector operator *(const vsx_vector &t) {
    vsx_vector temp;
    temp.x = x*t.x;
    temp.y = y*t.y;
    temp.z = z*t.z;
    return temp;
  }

  vsx_vector operator *(const float &t) {
    vsx_vector temp;
    temp.x = x*t;
    temp.y = y*t;
    temp.z = z*t;
    return temp;
  }


  vsx_vector operator /(const vsx_vector &t) {
    vsx_vector temp;
    temp.x = x/t.x;
    temp.y = y/t.y;
    temp.z = z/t.z;
    return temp;
  }
  vsx_vector operator /(int t) {
    vsx_vector temp;
    temp.x = x/(GLfloat)t;
    temp.y = y/(GLfloat)t;
    temp.z = z/(GLfloat)t;
    return temp;
  }
  bool operator ==(const vsx_vector &t)
  {
    return (x==t.x&&y==t.y&&z==t.z);
  }
  bool operator !=(const vsx_vector &t)
  {
    return (x!=t.x||y!=t.y||z!=t.z);
  }

  // calculates the cross product of the two other vectors and stores it here
  void cross(const vsx_vector &u, const vsx_vector &vv) {
    x = (u.y*vv.z) - (u.z*vv.y);
    y = (u.z*vv.x) - (u.x*vv.z);
    z = (u.x*vv.y) - (u.y*vv.x);
  }

  float dot_product(vsx_vector* ov) {
    return x * ov->x   +   y * ov->y   +   z * ov->z;
  }

  float length() {
    return norm();
  }

  float norm() {
    return (float)sqrt(x*x + y*y + z*z);
  }

  void normalize() {
    float a = 1.0f/(float)sqrt(x*x + y*y + z*z);
    x*=a;
    y*=a;
    z*=a;
  }

  void normalize(float len) {
    float a = 1.0f/(float)sqrt(x*x + y*y + z*z)*len;
    x*=a;
    y*=a;
    z*=a;
  }


  void assign_face_normal(vsx_vector *a, vsx_vector*bb, vsx_vector*c) {
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
  };

  inline void multiply_matrix_other_vec(const float *m,const vsx_vector& b)
  {
    x = m[0] * b.x + m[1] * b.y + m[2]  * b.z + m[3];
    y = m[4] * b.x + m[5] * b.y + m[6]  * b.z + m[7];
    z = m[8] * b.x + m[9] * b.y + m[10] * b.z + m[11];
  };



  VSX_MATH_3D_DLLIMPORT double distance(vsx_vector otherpoint);

  /*void hsl2rgb(float h, float s, float l) {
  //	SColor t;
	 if (s == 0.0f)
  	{
  		r = l;
  		g = l;
  		b = l;
  	} else {
  		float temp1, temp2, Rtemp3, Gtemp3, Btemp3;
  		if (l < 0.5)
  		{
  			temp2 = l*(1.0f+s);
  		} else {
  			temp2 = l+s-l*s;
  		}
  		temp1 = 2.0f *l - temp2;

  		Rtemp3 = h+1.0f/3.0f;
  		if (Rtemp3 < 0.0f) Rtemp3 += 1.0f;
  		if (Rtemp3 > 1.0f) Rtemp3 -= 1.0f;
  		Gtemp3 = h;
  		if (Gtemp3 < 0.0f) Gtemp3 += 1.0f;
  		if (Gtemp3 > 1.0f) Gtemp3 -= 1.0f;
  		Btemp3 = h-1.0f/3.0f;
  		if (Btemp3 < 0.0f) Btemp3 += 1.0f;
  		if (Btemp3 > 1.0f) Btemp3 -= 1.0f;
  		r = Rtemp3;
  		g = Gtemp3;
  		b = Btemp3;
  	}
  }*/


#ifdef VSXFST_H
  void from_string(vsx_string val) {
    vsx_string deli = ",";
    vsx_avector<vsx_string> pp;
    explode(val,deli,pp);
    if (pp.size() >= 1)
    x = s2f(pp[0]);
    if (pp.size() >= 2)
    y = s2f(pp[1]);
    if (pp.size() == 3)
    z = s2f(pp[2]);
  }

  vsx_string to_string(int num_values = 3) {
    vsx_string local = f2s(x);//+','+f2s(y)+','+f2s(z);
    if (num_values > 1) local += vsx_string(",")+f2s(y);
    if (num_values > 2) local += vsx_string(",")+f2s(z);
    return local;
  }
#endif

  void set(const float ix=0, const float iy=0, const float iz=0) {x=ix;y=iy;z=iz;};
  #ifdef DEBUG
  void dump(char* label="n/a") {printf("Name: %s. x: %f y: %f z: %f\n",label,x,y,z);}
  #endif
  vsx_vector() : x(0.0f),y(0.0f),z(0.0f) {};
  vsx_vector(float a, float bb = 0.0f, float c = 0.0f) {
    x = a;
    y = bb;
    z = c;
  }
};

#ifndef NO_VSX_COLOR
class vsx_color : public vsx_vector {
public:
  float a;

  void hsv(float H, float S, float V)
  {
    if ( S == 0 )                       //HSV values = From 0 to 1
    {
      r = V;                      //RGB results = From 0 to 255
      g = V;
      b = V;
    }
    else
    {
      float var_h = H * 6;
      int var_i = (int)floor( var_h );             //Or ... var_i = floor( var_h )
      float var_1 = V * ( 1 - S );
      float var_2 = V * ( 1 - S * ( var_h - var_i ) );
      float var_3 = V * ( 1 - S * ( 1 - ( var_h - var_i ) ) );
      float var_r, var_g, var_b;
   if      ( var_i == 0 ) { var_r = V     ; var_g = var_3 ; var_b = var_1; }
   else if ( var_i == 1 ) { var_r = var_2 ; var_g = V     ; var_b = var_1; }
   else if ( var_i == 2 ) { var_r = var_1 ; var_g = V     ; var_b = var_3; }
   else if ( var_i == 3 ) { var_r = var_1 ; var_g = var_2 ; var_b = V;     }
   else if ( var_i == 4 ) { var_r = var_3 ; var_g = var_1 ; var_b = V;     }
   else                   { var_r = V     ; var_g = var_1 ; var_b = var_2; }

   r = var_r;                  //RGB results = From 0 to 255
   g = var_g;
   b = var_b;
   }
  }

  void get_hsv(vsx_color* result) {
    float H = 0.0f, S, V;
    float var_R = r;
    float var_G = g;
    float var_B = b;
    float var_Min, var_Max;
    var_Min = var_R;
    var_Max = var_R;
    /*if (var_R < var_G) {
      var_Min = var_R;
      if (var_B < var_R) var_Min = var_B;
    } else
    if (var_G < var_R) {
      var_Min = var_G;
      if (var_B < var_G) var_Min = var_B;
    } else
    if (var_B < var_G) {
      var_Min = var_G;
      if (var_B < var_R) var_Min = var_B;
    } else*/

    if (var_R <= var_G && var_R <= var_B) var_Min = var_R; else
    if (var_G <= var_B && var_G <= var_R) var_Min = var_G; else
    if (var_B <= var_R && var_B <= var_G) var_Min = var_B;

    if (var_R >= var_G && var_R >= var_B) var_Max = var_R; else
    if (var_G >= var_B && var_G >= var_R) var_Max = var_G; else
    if (var_B >= var_R && var_B >= var_G) var_Max = var_B;

    //float var_Min = min( var_R, var_G, var_B );    //Min. value of RGB
    //float var_Max = max( var_R, var_G, var_B );    //Max. value of RGB
    float del_Max = var_Max - var_Min;             //Delta RGB value
    //printf("del_Max: %f\n",del_Max);
    V = var_Max;

    if ( del_Max == 0 )                     //This is a gray, no chroma...
    {
      H = 0;                                //HSV results = From 0 to 1
      S = 0;
    }
    else                                    //Chromatic data...
    {
      S = del_Max / var_Max;

      float del_R = ( ( ( var_Max - var_R ) / 6.0f ) + ( del_Max / 2.0f ) ) / del_Max;
      float del_G = ( ( ( var_Max - var_G ) / 6.0f ) + ( del_Max / 2.0f ) ) / del_Max;
      float del_B = ( ( ( var_Max - var_B ) / 6.0f ) + ( del_Max / 2.0f ) ) / del_Max;

      if      ( var_R == var_Max ) H = del_B - del_G;
      else if ( var_G == var_Max ) H = ( 1.0f / 3.0f ) + del_R - del_B;
      else if ( var_B == var_Max ) H = ( 2.0f / 3.0f ) + del_G - del_R;

      //if ( H < 0 ){}; H += 1;
      //if ( H > 1 ){}; H -= 1;
    }
    result->h = H;
    result->s = S;
    result->v = V;
  }

  // alpha blend 2 other vectors and store result in us
  // assumed it's colors so r,g,b,a will be blended
  // b is value to blend into
  // p at 0 is no blend, 1 is full blend into the other value
  void alpha_blend(vsx_color *aa, vsx_color *bb,float p) {
    if (p > 1) p = 1;
    /*if (p == 0) {
      r = aa->r;
      r = aa->g;
      r = aa->r;
      r = aa->r;
    } else
    if (p == 1) {
    } */
    r = aa->r*(1-p)+bb->r*p;
    g = aa->g*(1-p)+bb->g*p;
    b = aa->b*(1-p)+bb->b*p;
    a = aa->a*(1-p)+bb->a*p;
  }

  void gl_color() {
    #ifdef __gl_h_
    #ifndef VSX_NO_GL
    glColor4f(r,g,b,a);
    #endif
    #endif
  }
  void gl(float m) {
    #ifdef __gl_h_
    #ifndef VSX_NO_GL
    glColor4f(m*r,m*g,m*b,a);
    #endif
    #endif
  }

  void set(float ix=0, float iy=0, float iz=0, float ia=0) {x=ix;y=iy;z=iz;a=ia;};
  vsx_color() : a(1) {};
  vsx_color(float rr, float gg = 0.0f, float bb = 0.0f, float aa = 0.0f) {
    r = rr;
    g = gg;
    b = bb;
    a = aa;
  }

};
#endif

VSX_MATH_3D_DLLIMPORT vsx_color vsx_color__(float x, float y, float z, float a = 0);

class vsx_matrix {
public:
  float m[16];

  vsx_matrix() {
    load_identity();
  }

  void load_identity()
  {
    m[0] = 1;   m[1] = 0;   m[2] = 0;   m[3] = 0;
    m[4] = 0;   m[5] = 1;   m[6] = 0;   m[7] = 0;
    m[8] = 0;   m[9] = 0;   m[10] = 1;  m[11] = 0;
    m[12] = 0;  m[13] = 0;  m[14] = 0;  m[15] = 1;
  }

  void transpose() {
    m[0] = m[0];   m[1] = m[4];   m[2] = m[8];   m[3] = m[12];
    m[4] = m[1];   m[5] = m[5];   m[6] = m[9];   m[7] = m[13];
    m[8] = m[2];   m[9] = m[6];   m[10] = m[10];  m[11] = m[14];
    m[12] = m[3];  m[13] = m[7];  m[14] = m[11];  m[15] = m[15];
  }

  inline vsx_vector multiply_vector(vsx_vector a)
  {
    vsx_vector b;
    b.x = m[0] * a.x + m[1] * a.y + m[2]  * a.z + m[3];
    b.y = m[4] * a.x + m[5] * a.y + m[6]  * a.z + m[7];
    b.z = m[8] * a.x + m[9] * a.y + m[10] * a.z + m[11];
  	return b;
  };

  void assign_inverse(vsx_matrix *mm) {
    float d00, d01, d02, d03;
    float d10, d11, d12, d13;
    float d20, d21, d22, d23;
    float d30, d31, d32, d33;

    float m00, m01, m02, m03;
    float m10, m11, m12, m13;
    float m20, m21, m22, m23;
    float m30, m31, m32, m33;
    float D;

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

  void invert()  {
    int maxsize = 4;
    int actualsize = 4;
    if (actualsize <= 0) return;  // sanity check
    if (actualsize == 1) return;  // must be of dimension >= 2
    for (int i=1; i < actualsize; i++) m[i] /= m[0]; // normalize row 0
    for (int i=1; i < actualsize; i++)  {
      for (int j=i; j < actualsize; j++)  { // do a column of L
        float sum = 0.0;
        for (int k = 0; k < i; k++)
            sum += m[j*maxsize+k] * m[k*maxsize+i];
        m[j*maxsize+i] -= sum;
        }
      if (i == 3) continue;
      for (int j=i+1; j < 4; j++)  {  // do a row of U
        float sum = 0.0;
        for (int k = 0; k < i; k++)
            sum += m[i*maxsize+k]*m[k*maxsize+j];
        m[i*maxsize+j] =
           (m[i*maxsize+j]-sum) / m[i*maxsize+i];
        }
      }
    for ( int i = 0; i < actualsize; i++ )  // invert L
      for ( int j = i; j < actualsize; j++ )  {
        float x = 1.0;
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
        float sum = 0.0;
        for ( int k = i; k < j; k++ )
            sum += m[k*maxsize+j]*( (i==k) ? 1.0 : m[i*maxsize+k] );
        m[i*maxsize+j] = -sum;
        }
    for ( int i = 0; i < actualsize; i++ )   // final inversion
      for ( int j = 0; j < actualsize; j++ )  {
        float sum = 0.0;
        for ( int k = ((i>j)?i:j); k < actualsize; k++ )
          sum += ((j==k)?1.0:m[j*maxsize+k])*m[k*maxsize+i];
        m[j*maxsize+i] = sum;
        }
    }

  void multiply(vsx_matrix *a, vsx_matrix *b) {
    int i, j;
    float mm[16];
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
    memcpy(&m,&mm,sizeof(float)*16);
  }

  void rotation_from_vectors(vsx_vector* dir, vsx_vector* upv)
  {
  	vsx_vector x, y, z;
  	z = *dir;
  	z.normalize();
  	y = vsx_vector(-z.y*z.x,1-z.y*z.y,-z.y*z.z);
  	y.normalize();

  	x.cross(y,z);
    x.normalize();

  	m[0] = x.x; m[4] = x.y; m[8] = x.z;
  	m[1] = y.x; m[5] = y.y; m[9] = y.z;
  	m[2] = z.x; m[6] = z.y; m[10] = z.z;
  }


  #ifdef DEBUG
  void dump(char* label="n/a") {printf("Name: %s. 0: %f 1: %f 2: %f 3: %f\n4: %f 5: %f 6: %f 7: %f\n8: %f 9: %f 10: %f 11: %f\n12: %f 13: %f 14: %f 15: %f\n",label,m[0],m[1],m[2],m[3],m[4],m[5],m[6],m[7],m[8],m[9],m[10],m[11],m[12],m[13],m[14],m[15]);}
  #endif

};


class vsx_rand {
  void* state;
public:
  VSX_MATH_3D_DLLIMPORT vsx_rand();
  VSX_MATH_3D_DLLIMPORT ~vsx_rand();
  VSX_MATH_3D_DLLIMPORT void srand(uint32_t seed);
  VSX_MATH_3D_DLLIMPORT uint32_t rand();
  VSX_MATH_3D_DLLIMPORT float frand();

};


VSX_MATH_3D_DLLIMPORT float getmax(float foo,float bar);
VSX_MATH_3D_DLLIMPORT float getmin(float foo,float bar);
VSX_MATH_3D_DLLIMPORT float getrange(float value,float min,float max);
VSX_MATH_3D_DLLIMPORT int z_round(float val);
VSX_MATH_3D_DLLIMPORT double f_round(float val, int p);
VSX_MATH_3D_DLLIMPORT void v_norm(float *v);
VSX_MATH_3D_DLLIMPORT void mat_vec_mult(float *m, float *a, float *b);
VSX_MATH_3D_DLLIMPORT void mat_vec_mult3x3(float *m, float *a, float *b);
VSX_MATH_3D_DLLIMPORT void inv_mat( float *m, float *r);

VSX_MATH_3D_DLLIMPORT void Calc_Face_Normal(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float &nx, float &ny, float &nz);






#endif


