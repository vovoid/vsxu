#ifndef VSX_QUATERNION_H
#define VSX_QUATERNION_H

#include "vsx_math_3d.h"

class vsx_quaternion {
public:
  float x;
  float y;
  float z;
  float w;
  
  vsx_quaternion() {};

  vsx_quaternion(float xx, float yy, float zz) {
    x = xx;
    y = yy;
    z = zz;
  }

  inline void cos_slerp(vsx_quaternion& from, vsx_quaternion& to, float t)
  {
    slerp(from,to,(float)sin(t*half_pi));
  }

  inline void slerp(vsx_quaternion& from, vsx_quaternion& to, float t)
  {
    float         to1[4];
    double        omega, cosom, sinom, scale0, scale1;
    // calc cosine
    cosom = from.x * to.x + from.y * to.y + from.z * to.z + from.w * to.w;
    // adjust signs (if necessary)
    if (cosom < 0.0)
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
    if ( (1.0 - cosom) > 0.00001f ) {
      // standard case (slerp)
      omega = acos(cosom);
      sinom = sin(omega);
      scale0 = sin((1.0 - t) * omega) / sinom;
      scale1 = sin(t * omega) / sinom;
    } else {        
      // "from" and "to" quaternions are very close 
      //  ... so we can do a linear interpolation
      scale0 = 1.0 - t;
      scale1 = t;
    }
  	// calculate final values
  	x = (float)(scale0 * from.x + scale1 * to1[0]);
  	y = (float)(scale0 * from.y + scale1 * to1[1]);
  	z = (float)(scale0 * from.z + scale1 * to1[2]);
  	w = (float)(scale0 * from.w + scale1 * to1[3]);
  }
  
  vsx_quaternion operator +=(const vsx_quaternion &t) {
    x+=t.x;
    y+=t.y;
    z+=t.z;
    w+=t.w;
    return *this;
  }

  vsx_quaternion operator -(const vsx_quaternion &t) {
    vsx_quaternion temp;
    temp.x = x-t.x;
    temp.y = y-t.y;
    temp.z = z-t.z;
    return temp;
  }

  vsx_quaternion operator -(const vsx_vector &t) {
    vsx_quaternion temp;
    temp.x = x-t.x;
    temp.y = y-t.y;
    temp.z = z-t.z;
    return temp;
  }


  vsx_quaternion operator *(const vsx_vector &t) {
    vsx_quaternion temp;
    temp.x = x*t.x;
    temp.y = y*t.y;
    temp.z = z*t.z;
    return temp;
  }


  inline float dot_product(vsx_vector* ov) {
    return x * ov->x   +   y * ov->y   +   z * ov->z;
  }


  // OPTIMIZATION PENALTY!!!
  // Since we want to be able to multiply with ourselves, q1 is not by reference.
  inline void mul(vsx_quaternion q1, vsx_quaternion &q2) {
    x =  q1.x * q2.w + q1.y * q2.z - q1.z * q2.y + q1.w * q2.x;
    y = -q1.x * q2.z + q1.y * q2.w + q1.z * q2.x + q1.w * q2.y;
    z =  q1.x * q2.y - q1.y * q2.x + q1.z * q2.w + q1.w * q2.z;
    w = -q1.x * q2.x - q1.y * q2.y - q1.z * q2.z + q1.w * q2.w;
}
  
  inline vsx_matrix matrix()
  {
    vsx_matrix mat;
    GLfloat n, s;
    GLfloat xs, ys, zs;
    GLfloat wx, wy, wz;
    GLfloat xx, xy, xz;
    GLfloat yy, yz, zz;

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
  
  // normalizes the quaternion ensuring it stays a unit-quaternion
  inline void normalize() {
    float len = (float)(1.0 / sqrt(
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

//#ifdef VSX_STRING_LIB_H
#ifdef VSXFST_H
 /*****************************************************************************/
/** Initializes the quaternion from a vsx_string
  *
  * 
  *****************************************************************************/
  void from_string(vsx_string& str) {
    vsx_avector<vsx_string> parts;
    vsx_string deli = ",";
    explode(str, deli, parts);
    if (parts.size() == 4) {
      x = s2f(parts[0]);
      y = s2f(parts[1]);
      z = s2f(parts[2]);
      w = s2f(parts[3]);
    }
  }
#endif

  
};



#endif
