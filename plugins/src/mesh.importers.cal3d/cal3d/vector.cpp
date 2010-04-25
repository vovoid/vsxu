//****************************************************************************//
// vector.cpp                                                                 //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//

#include "cal3d/vector.h"
#include "cal3d/matrix.h"
#include "cal3d/quaternion.h"

 /*****************************************************************************/
/** Constructs the vector instance.
  *
  * This function is the default constructor of the vector instance.
  *****************************************************************************/
/*
CalVector::CalVector()
  : x(0.0f), y(0.0f), z(0.0f)
{
}
*/
 /*****************************************************************************/
/** Constructs the vector instance.
  *
  * This function is a constructor of the vector instance.
  *
  * @param v The vector to construct this vector instance from.
  *****************************************************************************/
/*
CalVector::CalVector(const CalVector& v)
  : x(v.x), y(v.y), z(v.z)
{
}
*/
 /*****************************************************************************/
/** Constructs the vector instance.
  *
  * This function is a constructor of the vector instance.
  *
  * @param vx The x component.
  * @param vy The y component.
  * @param vz The z component.
  *****************************************************************************/
/*
CalVector::CalVector(float vx, float vy, float vz)
  : x(vx), y(vy), z(vz)
{
}
*/
 /*****************************************************************************/
/** Destructs the vector instance.
  *
  * This function is the destructor of the vector instance.
  *****************************************************************************/
/*
CalVector::~CalVector()
{
}
*/
 /*****************************************************************************/
/** Provides access to the components of the vector instance.
  *
  * This function provides read and write access to the three components of the
  * vector instance.
  *
  * @param i The index to the specific component.
  *
  * @return A reference to the specific component.
  *****************************************************************************/
/*
float& CalVector::operator[](unsigned int i)
{
  return (&x)[i];
}
*/
 /*****************************************************************************/
/** Provides access to the components of the vector instance.
  *
  * This function provides read access to the three components of the vector
  * instance.
  *
  * @param i The index to the specific component.
  *
  * @return A constant reference to the specific component.
  *****************************************************************************/
/*
const float& CalVector::operator[](unsigned int i) const
{
  return (&x)[i];
}
*/
 /*****************************************************************************/
/** Equates the vector instance with another vector.
  *
  * This operator equates the vector instance with another vector.
  *
  * @param v The vector to equate the vector instance with.
  *****************************************************************************/
/*
void CalVector::operator=(const CalVector& v)
{
  x = v.x;
  y = v.y;
  z = v.z;
}
*/
 /*****************************************************************************/
/** Adds another vector to the vector instance.
  *
  * This operator adds another vector to the vector instance.
  *
  * @param v The vector to be added.
  *****************************************************************************/
/*
void CalVector::operator+=(const CalVector& v)
{
  x += v.x;
  y += v.y;
  z += v.z;
}
*/
 /*****************************************************************************/
/** Subtracts another vector from the vector instance.
  *
  * This operator subtracts another vector from the vector instance.
  *
  * @param v The vector to be subtracted.
  *****************************************************************************/
/*
void CalVector::operator-=(const CalVector& v)
{
  x -= v.x;
  y -= v.y;
  z -= v.z;
}
*/
 /*****************************************************************************/
/** Scales the vector instance.
  *
  * This operator scales the vector instance by multiplying its components by
  * a specific factor.
  *
  * @param d The factor to multiply the vector components by.
  *****************************************************************************/
/*
void CalVector::operator*=(const float d)
{
  x *= d;
  y *= d;
  z *= d;
}
*/
 /*****************************************************************************/
/** Transforms the vector instance by a quaternion.
  *
  * This function transforms the vector instance by a given quaternion.
  *
  * @param q The quaternion to be used for the transformation.
  *****************************************************************************/

void CalVector::operator*=(const CalQuaternion& q)
{
  CalQuaternion temp(-q.x, -q.y, -q.z, q.w);
  temp *= *this;
  temp *= q;

  x = temp.x;
  y = temp.y;
  z = temp.z;
}

 /*****************************************************************************/
/** Transforms the vector instance by a matrix.
  *
  * This function transforms the vector instance by a given matrix.
  *
  * @param m The matrix to be used for the transformation.
  *****************************************************************************/
/*
void CalVector::operator*=(const CalMatrix& m)
{
  float ox = x;
  float oy = y;
  float oz = z;
  x = m.dxdx*ox + m.dxdy*oy + m.dxdz*oz;
  y = m.dydx*ox + m.dydy*oy + m.dydz*oz;
  z = m.dzdx*ox + m.dzdy*oy + m.dzdz*oz;
}
*/
 /*****************************************************************************/
/** Scales the vector instance.
  *
  * This operator scales the vector instance by dividing its components by a
  * specific factor.
  *
  * @param d The factor to divide the vector components by.
  *****************************************************************************/
/*
void CalVector::operator/=(const float d)
{
  x /= d;
  y /= d;
  z /= d;
}
*/
 /*****************************************************************************/
/** Tests the equality of 2 vectors
  *
  * This operator checks to see if 2 vectors are equal
  *
  * @param v The vector to be tested against.
  *****************************************************************************/
/*
bool CalVector::operator==(const CalVector& v)
{
  return ((x == v.x) && (y == v.y) && (z == v.z));
}
*/
 /*****************************************************************************/
/** Calculates the sum of two vectors.
  *
  * This operator calculates the sum of two vectors.
  *
  * @param v The first vector to be added.
  * @param u The second vector to be added.
  *
  * @return The sum of the two vectors.
  *****************************************************************************/
/*
CalVector operator+(const CalVector& v, const CalVector& u)
{
  return CalVector(v.x + u.x, v.y + u.y, v.z + u.z);
}
*/
 /*****************************************************************************/
/** Calculates the difference of two vectors.
  *
  * This operator calculates the difference of two vectors.
  *
  * @param v The first vector to be added.
  * @param u The second vector to be subtracted.
  *
  * @return The difference of the two vectors.
  *****************************************************************************/
/*
CalVector operator-(const CalVector& v, const CalVector& u)
{
  return CalVector(v.x - u.x, v.y - u.y, v.z - u.z);
}
*/
 /*****************************************************************************/
/** Calculates a scaled vector.
  *
  * This operator calculates the vector multiplied by a factor.
  *
  * @param v The vector to be scaled.
  * @param d The factor to multiply the vector with.
  *
  * @return The scaled vector.
  *****************************************************************************/
/*
CalVector operator*(const CalVector& v, float d)
{
  return CalVector(v.x * d, v.y * d, v.z * d);
}
*/
 /*****************************************************************************/
/** Calculates a scaled vector.
  *
  * This operator calculates the vector multiplied by a factor.
  *
  * @param d The factor to multiply the vector with.
  * @param v The vector to be scaled.
  *
  * @return The scaled vector.
  *****************************************************************************/
/*
CalVector operator*(float d, const CalVector& v)
{
  return CalVector(v.x * d, v.y * d, v.z * d);
}
*/
 /*****************************************************************************/
/** Calculates a scaled vector.
  *
  * This operator calculates the vector divided by a factor.
  *
  * @param v The vector to be scaled.
  * @param d The factor to divide the vector with.
  *
  * @return The scaled vector.
  *****************************************************************************/
/*
CalVector operator/(const CalVector& v, float d)
{
  return CalVector(v.x / d, v.y / d, v.z / d);
}
*/
 /*****************************************************************************/
/** Calculates the dot product of two vectors.
  *
  * This operator calculates the dot product of two vectors.
  *
  * @param v The first vector.
  * @param u The second vector.
  *
  * @return The dot product of the two vectors.
  *****************************************************************************/
/*
float operator*(const CalVector& v, const CalVector& u)
{
  return v.x * u.x + v.y * u.y + v.z * u.z;
}
*/
 /*****************************************************************************/
/** Calculates the vector product of two vectors.
  *
  * This operator calculates the vector product of two vectors.
  *
  * @param v The first vector.
  * @param u The second vector.
  *
  * @return The vector product of the two vectors.
  *****************************************************************************/
/*
CalVector operator%(const CalVector& v, const CalVector& u)
{
  return CalVector(v.y * u.z - v.z * u.y, v.z * u.x - v.x * u.z, v.x * u.y - v.y * u.x);
}
*/
 /*****************************************************************************/
/** Interpolates the vector instance to another vector.
  *
  * This function interpolates the vector instance to another vector by a given
  * factor.
  *
  * @param d The blending factor in the range [0.0, 1.0].
  * @param v The vector to be interpolated to.
  *****************************************************************************/
/*
void CalVector::blend(float d, const CalVector& v)
{
  x += d * (v.x - x);
  y += d * (v.y - y);
  z += d * (v.z - z);
}
*/
 /*****************************************************************************/
/** Clears the vector instance.
  *
  * This function clears the vector instance.
  *****************************************************************************/
/*
void CalVector::clear()
{
  x = 0.0f;
  y = 0.0f;
  z = 0.0f;
}
*/
 /*****************************************************************************/
/** Returns the length of the vector instance.
  *
  * This function returns the length of the vector instance.
  *
  * @return The length of the vector instance.
  *****************************************************************************/
/*
float CalVector::length()
{
  return (float)sqrt(x * x + y * y + z * z);
}
*/
 /*****************************************************************************/
/** Normalizes the vector instance.
  *
  * This function normalizes the vector instance and returns its former length.
  *
  * @return The length of the vector instance before normalizing.
  *****************************************************************************/
/*
float CalVector::normalize()
{
  // calculate the length of the vector
  float length;
  length = (float) sqrt(x * x + y * y + z * z);

  // normalize the vector
  x /= length;
  y /= length;
  z /= length;

  return length;
}
*/
 /*****************************************************************************/
/** Sets new values.
  *
  * This function sets new values in the vector instance.
  *
  * @param x The x component.
  * @param y The y component.
  * @param z The z component.
  *****************************************************************************/
/*
void CalVector::set(float vx, float vy, float vz)
{
  x = vx;
  y = vy;
  z = vz;
}
*/
 /*****************************************************************************
  *
  *   Functions for the plane class, they don't have real sense outside
  *   bounding box calculation,
  *   
  *****************************************************************************/
  
  
float CalPlane::eval(CalVector &p)
{
   return p.x*a+p.y*b+p.z*c+d;
}

void CalPlane::setPosition(CalVector &p)
{
   d=-p.x*a-p.y*b-p.z*c;
}

void CalPlane::setNormal(CalVector &p)
{
     a=p.x;
     b=p.y;
     c=p.z;
     d=-1e32f;
};

float CalPlane::dist(CalVector &p)
{
  return fabs( (p.x*a+p.y*b+p.z*c+d)/sqrt(a*a+b*b+c*c)) ;
};


 /*****************************************************************************/
/** Computes points of a bounding box.
  *
  * This function computes the 8 points of a bounding box.
  *
  * @param p A pointer to CalVector[8], the 8 points of the bounding box
  *****************************************************************************/


void CalBoundingBox::computePoints(CalVector *p)
{
    CalMatrix m;
     
    int i,j,k;
        
    for(i=0;i<2;i++)
       for(j=2;j<4;j++)
           for(k=4;k<6;k++)
        {
           float x,y,z;
           
           m.dxdx=plane[i].a;m.dxdy=plane[i].b;m.dxdz=plane[i].c;        
           m.dydx=plane[j].a;m.dydy=plane[j].b;m.dydz=plane[j].c;        
           m.dzdx=plane[k].a;m.dzdy=plane[k].b;m.dzdz=plane[k].c;
           
           float det = m.det();
           
           if(det!=0)
           {
              m.dxdx=-plane[i].d;m.dxdy=plane[i].b;m.dxdz=plane[i].c;        
              m.dydx=-plane[j].d;m.dydy=plane[j].b;m.dydz=plane[j].c;        
              m.dzdx=-plane[k].d;m.dzdy=plane[k].b;m.dzdz=plane[k].c;
              
              x=m.det()/det;

              m.dxdx=plane[i].a;m.dxdy=-plane[i].d;m.dxdz=plane[i].c;        
              m.dydx=plane[j].a;m.dydy=-plane[j].d;m.dydz=plane[j].c;        
              m.dzdx=plane[k].a;m.dzdy=-plane[k].d;m.dzdz=plane[k].c;
              
              y=m.det()/det;

              m.dxdx=plane[i].a;m.dxdy=plane[i].b;m.dxdz=-plane[i].d;        
              m.dydx=plane[j].a;m.dydy=plane[j].b;m.dydz=-plane[j].d;        
              m.dzdx=plane[k].a;m.dzdy=plane[k].b;m.dzdz=-plane[k].d;
  
              z=m.det()/det;
              
              p->x=x;p->y=y;p->z=z;
           }
           else {p->x=0.0f;p->y=0.0f;p->z=0.0f;}
           
           p++;           
        }           
 }
   



//****************************************************************************//
