//****************************************************************************//
// matrix.cpp                                                                 //
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

#include "cal3d/matrix.h"
#include "cal3d/quaternion.h"
#include "cal3d/vector.h"

 /*****************************************************************************/
/** Constructs the matrix instance.
  *
  * This function is the default constructor of the matrix instance.
  *****************************************************************************/
/*
CalMatrix::CalMatrix()
  : dxdx(0.0), dxdy(0.0), dxdz(0.0),
    dydx(0.0), dydy(0.0), dydz(0.0),
    dzdx(0.0), dzdy(0.0), dzdz(0.0)
{
}
*/
 /*****************************************************************************/
/** Destructs the matrix instance.
  *
  * This function is the destructor of the matrix instance.
  *****************************************************************************/
/*
CalMatrix::~CalMatrix()
{
}
*/
 /*****************************************************************************/
/** Copying a Matrix
  *
  * This function copies one matrix into another.
  *****************************************************************************/
/*
void CalMatrix::operator =(const CalMatrix& m)
{
  dxdx=m.dxdx; dxdy=m.dxdy; dxdz=m.dxdz;
  dydx=m.dydx; dydy=m.dydy; dydz=m.dydz;
  dzdx=m.dzdx; dzdy=m.dzdy; dzdz=m.dzdz;
}
*/
 /*****************************************************************************/
/** Quaternion to Matrix Conversion Constructor
  *
  * This function converts a quaternion into a rotation matrix.
  *****************************************************************************/

CalMatrix::CalMatrix(const CalQuaternion& q)
{
  *this = q;
}

 /*****************************************************************************/
/** Quaternion to Matrix Conversion.
  *
  * This function converts a quaternion into a rotation matrix.
  *****************************************************************************/

void CalMatrix::operator =(const CalQuaternion& q)
{
  float xx2=q.x*q.x*2;
  float yy2=q.y*q.y*2;
  float zz2=q.z*q.z*2;
  float xy2=q.x*q.y*2;
  float zw2=q.z*q.w*2;
  float xz2=q.x*q.z*2;
  float yw2=q.y*q.w*2;
  float yz2=q.y*q.z*2;
  float xw2=q.x*q.w*2;
  dxdx=1-yy2-zz2;   dxdy=  xy2+zw2;  dxdz=  xz2-yw2;
  dydx=  xy2-zw2;   dydy=1-xx2-zz2;  dydz=  yz2+xw2;
  dzdx=  xz2+yw2;   dzdy=  yz2-xw2;  dzdz=1-xx2-yy2;
}

 /*****************************************************************************/
/** Matrix Initialization
  *
  * This function sets one matrix to a factor times another.
  *****************************************************************************/
/*
CalMatrix::CalMatrix(float factor, const CalMatrix& m)
{
  dxdx = m.dxdx*factor;
  dxdy = m.dxdy*factor;
  dxdz = m.dxdz*factor;
  dydx = m.dydx*factor;
  dydy = m.dydy*factor;
  dydz = m.dydz*factor;
  dzdx = m.dzdx*factor;
  dzdy = m.dzdy*factor;
  dzdz = m.dzdz*factor;
}
*/
 /*****************************************************************************/
/** Matrix Blending
  *
  * This function adds a weight times another matrix to the current matrix.
  *****************************************************************************/
/*
void CalMatrix::blend(float factor, const CalMatrix& m)
{
  dxdx += m.dxdx*factor;
  dxdy += m.dxdy*factor;
  dxdz += m.dxdz*factor;
  dydx += m.dydx*factor;
  dydy += m.dydy*factor;
  dydz += m.dydz*factor;
  dzdx += m.dzdx*factor;
  dzdy += m.dzdy*factor;
  dzdz += m.dzdz*factor;
}
*/
 /*****************************************************************************/
/** Matrix Scaling
  *
  * This function multiplies every element in the matrix by the factor.
  *****************************************************************************/
/*
void CalMatrix::operator *=(float factor)
{
  dxdx *= factor;
  dxdy *= factor;
  dxdz *= factor;
  dydx *= factor;
  dydy *= factor;
  dydz *= factor;
  dzdx *= factor;
  dzdy *= factor;
  dzdz *= factor;
}
*/
 /*****************************************************************************/
/** Matrix Multiplication
  *
  * This function multiplies two matrices.
  *****************************************************************************/
/*
void CalMatrix::operator *=(const CalMatrix &m)
{
  float ndxdx=m.dxdx*dxdx+m.dxdy*dydx+m.dxdz*dzdx;
  float ndydx=m.dydx*dxdx+m.dydy*dydx+m.dydz*dzdx;
  float ndzdx=m.dzdx*dxdx+m.dzdy*dydx+m.dzdz*dzdx;

  float ndxdy=m.dxdx*dxdy+m.dxdy*dydy+m.dxdz*dzdy;
  float ndydy=m.dydx*dxdy+m.dydy*dydy+m.dydz*dzdy;
  float ndzdy=m.dzdx*dxdy+m.dzdy*dydy+m.dzdz*dzdy;

  float ndxdz=m.dxdx*dxdz+m.dxdy*dydz+m.dxdz*dzdz;
  float ndydz=m.dydx*dxdz+m.dydy*dydz+m.dydz*dzdz;
  float ndzdz=m.dzdx*dxdz+m.dzdy*dydz+m.dzdz*dzdz;

  dxdx=ndxdx;
  dxdy=ndxdy;
  dxdz=ndxdz;
  dydx=ndydx;
  dydy=ndydy;
  dydz=ndydz;
  dzdx=ndzdx;
  dzdy=ndzdy;
  dzdz=ndzdz;
}
*/
/*****************************************************************************/
/** Matrix determinant
  *
  * This function compute the determinant of the matrix.
  *****************************************************************************/

/*
float CalMatrix::det()
{
   return dxdx * (dydy*dzdz-dydz*dzdy)
            -dxdy* ( dydx*dzdz-dzdx*dydz)
	    +dxdz* (dydx*dzdy-dzdx*dydy);
}
*/
//****************************************************************************//
