/**
* Project: VSXu: Realtime modular visual programming language, music/audio visualizer.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
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

/** KempoApi: The Turloc Toolkit *****************************/
/** *    *                                                  **/
/** **  **  Filename: ArcBall.h                             **/
/**   **    Version:  Common                                **/
/**   **                                                    **/
/**                                                         **/
/**  Arcball class for mouse manipulation.                  **/
/**                                                         **/
/**                                                         **/
/**                                                         **/
/**                                                         **/
/**                              (C) 1999-2003 Tatewake.com **/
/**   History:                                              **/
/**   08/17/2003 - (TJG) - Creation                         **/
/**   09/23/2003 - (TJG) - Bug fix and optimization         **/
/**   09/25/2003 - (TJG) - Version for NeHe Basecode users  **/
/**                                                         **/
/*************************************************************/

#ifndef _ArcBall_h
#define _ArcBall_h

// 8<--Snip here if you have your own math types/funcs-->8 

//Only support assertions in debug builds
#ifdef _DEBUG
# include "assert.h"
#else
#ifndef assert
# define assert(x) { }
#endif
#endif

//Math types derived from the KempoApi tMath library
    typedef union Tuple2f_t
    {
        struct
        {
            GLfloat X, Y;
        } s;

        GLfloat T[2];
    } Tuple2fT;      //A generic 2-element tuple that is represented by single-precision floating point x,y coordinates. 

    typedef union Tuple3f_t
    {
        struct
        {
            GLfloat X, Y, Z;
        } s;

        GLfloat T[3];
    } Tuple3fT;      //A generic 3-element tuple that is represented by single precision-floating point x,y,z coordinates. 

    typedef union Tuple4f_t
    {
        struct
        {
            GLfloat X, Y, Z, W;
        } s;

        GLfloat T[4];
    } Tuple4fT;      //A 4-element tuple represented by single-precision floating point x,y,z,w coordinates. 

    typedef union Matrix3f_t
    {
            struct
            {
                //column major
                union { GLfloat M00; GLfloat XX; GLfloat SX; };  //XAxis.X and Scale X
                union { GLfloat M10; GLfloat XY;             };  //XAxis.Y
                union { GLfloat M20; GLfloat XZ;             };  //XAxis.Z
                union { GLfloat M01; GLfloat YX;             };  //YAxis.X
                union { GLfloat M11; GLfloat YY; GLfloat SY; };  //YAxis.Y and Scale Y
                union { GLfloat M21; GLfloat YZ;             };  //YAxis.Z
                union { GLfloat M02; GLfloat ZX;             };  //ZAxis.X
                union { GLfloat M12; GLfloat ZY;             };  //ZAxis.Y
                union { GLfloat M22; GLfloat ZZ; GLfloat SZ; };  //ZAxis.Z and Scale Z
            } s;
            GLfloat M[9];
            GLfloat m[3][3];
    } Matrix3fT;     //A single precision floating point 3 by 3 matrix. 

    typedef union Matrix4f_t
    {
            struct
            {
                //column major
                union { GLfloat M00; GLfloat XX; GLfloat SX; };  //XAxis.X and Scale X
                union { GLfloat M10; GLfloat XY;             };  //XAxis.Y
                union { GLfloat M20; GLfloat XZ;             };  //XAxis.Z
                union { GLfloat M30; GLfloat XW;             };  //XAxis.W
                union { GLfloat M01; GLfloat YX;             };  //YAxis.X
                union { GLfloat M11; GLfloat YY; GLfloat SY; };  //YAxis.Y and Scale Y
                union { GLfloat M21; GLfloat YZ;             };  //YAxis.Z
                union { GLfloat M31; GLfloat YW;             };  //YAxis.W
                union { GLfloat M02; GLfloat ZX;             };  //ZAxis.X
                union { GLfloat M12; GLfloat ZY;             };  //ZAxis.Y
                union { GLfloat M22; GLfloat ZZ; GLfloat SZ; };  //ZAxis.Z and Scale Z
                union { GLfloat M32; GLfloat ZW;             };  //ZAxis.W
                union { GLfloat M03; GLfloat TX;             };  //Trans.X
                union { GLfloat M13; GLfloat TY;             };  //Trans.Y
                union { GLfloat M23; GLfloat TZ;             };  //Trans.Z
                union { GLfloat M33; GLfloat TW; GLfloat SW; };  //Trans.W and Scale W
            } s;
            GLfloat M[16];
    } Matrix4fT;     //A single precision floating point 4 by 4 matrix. 


//"Inherited" types
#define Point2fT    Tuple2fT   //A 2 element point that is represented by single precision floating point x,y coordinates. 

#define Quat4fT     Tuple4fT   //A 4 element unit quaternion represented by single precision floating point x,y,z,w coordinates. 

#define Vector2fT   Tuple2fT   //A 2-element vector that is represented by single-precision floating point x,y coordinates. 
#define Vector3fT   Tuple3fT   //A 3-element vector that is represented by single-precision floating point x,y,z coordinates. 

//Custom math, or speed overrides
#define FuncSqrt    sqrtf

//utility macros
//assuming IEEE-754(GLfloat), which i believe has max precision of 7 bits
# define Epsilon 1.0e-5

//Math functions

    /**
     * Sets the value of this tuple to the vector sum of itself and tuple t1.
     * @param t1  the other tuple
     */
    inline
    static void Point2fAdd(Point2fT* NewObj, const Tuple2fT* t1)
    {
        assert(NewObj && t1);

        NewObj->s.X += t1->s.X;
        NewObj->s.Y += t1->s.Y;
    }

    /**
      * Sets the value of this tuple to the vector difference of itself and tuple t1 (this = this - t1).
      * @param t1 the other tuple
      */
    inline
    static void Point2fSub(Point2fT* NewObj, const Tuple2fT* t1)
    {
        assert(NewObj && t1);

        NewObj->s.X -= t1->s.X;
        NewObj->s.Y -= t1->s.Y;
    }

    /**
      * Sets this vector to be the vector cross product of vectors v1 and v2.
      * @param v1 the first vector
      * @param v2 the second vector
      */
    inline
    static void Vector3fCross(Vector3fT* NewObj, const Vector3fT* v1, const Vector3fT* v2)
    {
        Vector3fT Result; //safe not to initialize

        assert(NewObj && v1 && v2);

        // store on stack once for aliasing-safty
        // i.e. safe when a.cross(a, b)

        Result.s.X = (v1->s.Y * v2->s.Z) - (v1->s.Z * v2->s.Y);
        Result.s.Y = (v1->s.Z * v2->s.X) - (v1->s.X * v2->s.Z);
        Result.s.Z = (v1->s.X * v2->s.Y) - (v1->s.Y * v2->s.X);

        //copy result back
        *NewObj = Result;
    }

    /**
      * Computes the dot product of the this vector and vector v1.
      * @param  v1 the other vector
      */
    inline
    static GLfloat Vector3fDot(const Vector3fT* NewObj, const Vector3fT* v1)
    {
        assert(NewObj && v1);

        return  (NewObj->s.X * v1->s.X) +
                (NewObj->s.Y * v1->s.Y) +
                (NewObj->s.Z * v1->s.Z);
    }

    /**
      * Returns the squared length of this vector.
      * @return the squared length of this vector
      */
    inline
    static GLfloat Vector3fLengthSquared(const Vector3fT* NewObj)
    {
        assert(NewObj);

        return  (NewObj->s.X * NewObj->s.X) +
                (NewObj->s.Y * NewObj->s.Y) +
                (NewObj->s.Z * NewObj->s.Z);
    }

    /**
      * Returns the length of this vector.
      * @return the length of this vector
      */
    inline
    static GLfloat Vector3fLength(const Vector3fT* NewObj)
    {
        assert(NewObj);

        return FuncSqrt(Vector3fLengthSquared(NewObj));
    }

    inline
    static void Matrix3fSetZero(Matrix3fT* NewObj)
    {
        NewObj->s.M00 = NewObj->s.M01 = NewObj->s.M02 = 
        NewObj->s.M10 = NewObj->s.M11 = NewObj->s.M12 = 
        NewObj->s.M20 = NewObj->s.M21 = NewObj->s.M22 = 0.0f;
    }

    /**
     * Sets this Matrix3 to identity.
     */
    inline
    static void Matrix3fSetIdentity(Matrix3fT* NewObj)
    {
        Matrix3fSetZero(NewObj);

        //then set diagonal as 1
        NewObj->s.M00 = 
        NewObj->s.M11 = 
        NewObj->s.M22 = 1.0f;
    }
    
	inline void Quat4fMulQuat4f(Quat4fT* res, const Quat4fT* q)
	{
		float qx, qy, qz, qw;
		qx = res->s.X;
		qy = res->s.Y;
		qz = res->s.Z;
		qw = res->s.W;
		
		res->s.X = qw * q->s.X + qx * q->s.W + qy * q->s.Z - qz * q->s.Y;
		res->s.Y = qw * q->s.Y - qx * q->s.Z + qy * q->s.W + qz * q->s.X;
		res->s.Z = qw * q->s.Z + qx * q->s.Y - qy * q->s.X + qz * q->s.W;
		res->s.W = qw * q->s.W - qx * q->s.X - qy * q->s.Y - qz * q->s.Z;
	}
	


inline void QuatSlerp(Quat4fT* from, Quat4fT* to, float t, Quat4fT* res)
{
  float           to1[4];
  double        omega, cosom, sinom, scale0, scale1;
  // calc cosine
  cosom = from->s.X * to->s.X + from->s.Y * to->s.Y + from->s.Z * to->s.Z + from->s.W * to->s.W;
  // adjust signs (if necessary)
  if (cosom < 0.0)
  { 
    cosom = -cosom; to1[0] = - to->s.X;
		to1[1] = - to->s.Y;
		to1[2] = - to->s.Z;
		to1[3] = - to->s.W;
  } else {
		to1[0] = to->s.X;
		to1[1] = to->s.Y;
		to1[2] = to->s.Z;
		to1[3] = to->s.W;
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
	res->s.X = scale0 * from->s.X + scale1 * to1[0];
	res->s.Y = scale0 * from->s.Y + scale1 * to1[1];
	res->s.Z = scale0 * from->s.Z + scale1 * to1[2];
	res->s.W = scale0 * from->s.W + scale1 * to1[3];
}




/*00   10   20
	
01	 11   21
	
02   12   22
---
00   01   0*/
	
    
inline void QuatFromMat(Matrix3fT* a, Quat4fT& quat ) {
  //float trace = a->s.M00 + a->s.M11 + a->s.M22;
  int    i, j, k;
  float s;
  float q[4];
  int nxt[3] = {1, 2, 0};
  float tr =  a->m[0][0] + a->m[1][1] + a->m[2][2];

  if (tr > 0.0) {
    s = sqrt (tr + 1.0);
    quat.s.W = s / 2.0;
    s = 0.5 / s;
    quat.s.X = (a->m[1][2] - a->m[2][1]) * s;
    quat.s.Y = (a->m[2][0] - a->m[0][2]) * s;
    quat.s.Z = (a->m[0][1] - a->m[1][0]) * s;
  } else {
// diagonal is negative
    	  i = 0;
          if (a->m[1][1] > a->m[0][0]) i = 1;
	     if (a->m[2][2] > a->m[i][i]) i = 2;
            j = nxt[i];
            k = nxt[j];


            s = sqrt ((a->m[i][i] - (a->m[j][j] + a->m[k][k])) + 1.0);
      
	     q[i] = s * 0.5;
            
            if (s != 0.0) s = 0.5 / s;


	    q[3] = (a->m[j][k] - a->m[k][j]) * s;
            q[j] = (a->m[i][j] + a->m[j][i]) * s;
            q[k] = (a->m[i][k] + a->m[k][i]) * s;


	  quat.s.X = q[0];
	  quat.s.Y = q[1];
	  quat.s.Z = q[2];
	  quat.s.W = q[3];    
/*    if ( a->s.M00 > a->s.M11 && a->s.M00 > a->s.M22 ) {
      float s = 2.0f * sqrtf( 1.0f + a->s.M00 - a->s.M11 - a->s.M22);
      q.s.X = 0.25f * s;
      q.s.Y = (a->s.M10 + a->s.M01 ) / s;
      q.s.Z = (a->s.M20 + a->s.M01 ) / s;
      q.s.W = (a->s.M21 - a->s.M12 ) / s;
    } else if (a->s.M11 > a->s.M22) {
      float s = 2.0f * sqrtf( 1.0f + a->s.M11 - a->s.M00 - a->s.M22);
      q.s.X = (a->s.M10 + a->s.M01 ) / s;
      q.s.Y = 0.25f * s;
      q.s.Z = (a->s.M21 + a->s.M12 ) / s;
      q.s.W = (a->s.M20 - a->s.M01 ) / s;
    } else {
      float s = 2.0f * sqrtf( 1.0f + a->s.M22 - a->s.M00 - a->s.M11 );
      q.s.X = (a->s.M20 + a->s.M02 ) / s;
      q.s.Y = (a->s.M21 + a->s.M12 ) / s;
      q.s.Z = 0.25f * s;
      q.s.W = (a->s.M10 - a->s.M01 ) / s;
    }*/
  }
}    
    

    /**
      * Sets the value of this matrix to the matrix conversion of the
      * quaternion argument. 
      * @param q1 the quaternion to be converted 
      */
    //$hack this can be optimized some(if s == 0)
    inline
    static void Matrix3fSetRotationFromQuat4f(Matrix3fT* NewObj, const Quat4fT* q1)
    {
        GLfloat n, s;
        GLfloat xs, ys, zs;
        GLfloat wx, wy, wz;
        GLfloat xx, xy, xz;
        GLfloat yy, yz, zz;

        assert(NewObj && q1);

        n = (q1->s.X * q1->s.X) + (q1->s.Y * q1->s.Y) + (q1->s.Z * q1->s.Z) + (q1->s.W * q1->s.W);
        s = (n > 0.0f) ? (2.0f / n) : 0.0f;

        xs = q1->s.X * s;  ys = q1->s.Y * s;  zs = q1->s.Z * s;
        wx = q1->s.W * xs; wy = q1->s.W * ys; wz = q1->s.W * zs;
        xx = q1->s.X * xs; xy = q1->s.X * ys; xz = q1->s.X * zs;
        yy = q1->s.Y * ys; yz = q1->s.Y * zs; zz = q1->s.Z * zs;

        NewObj->s.XX = 1.0f - (yy + zz); NewObj->s.YX =         xy - wz;  NewObj->s.ZX =         xz + wy;
        NewObj->s.XY =         xy + wz;  NewObj->s.YY = 1.0f - (xx + zz); NewObj->s.ZY =         yz - wx;
        NewObj->s.XZ =         xz - wy;  NewObj->s.YZ =         yz + wx;  NewObj->s.ZZ = 1.0f - (xx + yy);
    }

    /**
     * Sets the value of this matrix to the result of multiplying itself
     * with matrix m1. 
     * @param m1 the other matrix 
     */
    inline
    static void Matrix3fMulMatrix3f(Matrix3fT* NewObj, const Matrix3fT* m1)
    {
        Matrix3fT Result; //safe not to initialize

        assert(NewObj && m1);

        // alias-safe way.
        Result.s.M00 = (NewObj->s.M00 * m1->s.M00) + (NewObj->s.M01 * m1->s.M10) + (NewObj->s.M02 * m1->s.M20);
        Result.s.M01 = (NewObj->s.M00 * m1->s.M01) + (NewObj->s.M01 * m1->s.M11) + (NewObj->s.M02 * m1->s.M21);
        Result.s.M02 = (NewObj->s.M00 * m1->s.M02) + (NewObj->s.M01 * m1->s.M12) + (NewObj->s.M02 * m1->s.M22);

        Result.s.M10 = (NewObj->s.M10 * m1->s.M00) + (NewObj->s.M11 * m1->s.M10) + (NewObj->s.M12 * m1->s.M20);
        Result.s.M11 = (NewObj->s.M10 * m1->s.M01) + (NewObj->s.M11 * m1->s.M11) + (NewObj->s.M12 * m1->s.M21);
        Result.s.M12 = (NewObj->s.M10 * m1->s.M02) + (NewObj->s.M11 * m1->s.M12) + (NewObj->s.M12 * m1->s.M22);

        Result.s.M20 = (NewObj->s.M20 * m1->s.M00) + (NewObj->s.M21 * m1->s.M10) + (NewObj->s.M22 * m1->s.M20);
        Result.s.M21 = (NewObj->s.M20 * m1->s.M01) + (NewObj->s.M21 * m1->s.M11) + (NewObj->s.M22 * m1->s.M21);
        Result.s.M22 = (NewObj->s.M20 * m1->s.M02) + (NewObj->s.M21 * m1->s.M12) + (NewObj->s.M22 * m1->s.M22);

        //copy result back to this
        *NewObj = Result;
    }

    inline
    static void Matrix4fSetRotationScaleFromMatrix4f(Matrix4fT* NewObj, const Matrix4fT* m1)
    {
        assert(NewObj && m1);

        NewObj->s.XX = m1->s.XX; NewObj->s.YX = m1->s.YX; NewObj->s.ZX = m1->s.ZX;
        NewObj->s.XY = m1->s.XY; NewObj->s.YY = m1->s.YY; NewObj->s.ZY = m1->s.ZY;
        NewObj->s.XZ = m1->s.XZ; NewObj->s.YZ = m1->s.YZ; NewObj->s.ZZ = m1->s.ZZ;
    }

    /**
      * Performs SVD on this matrix and gets scale and rotation.
      * Rotation is placed into rot3, and rot4.
      * @param rot3 the rotation factor(Matrix3d). if null, ignored
      * @param rot4 the rotation factor(Matrix4) only upper 3x3 elements are changed. if null, ignored
      * @return scale factor
      */
    inline
    static GLfloat Matrix4fSVD(const Matrix4fT* NewObj, Matrix3fT* rot3, Matrix4fT* rot4)
    {
        GLfloat s, n;

        assert(NewObj);

        // this is a simple svd.
        // Not complete but fast and reasonable.
        // See comment in Matrix3d.

        s = FuncSqrt(
                ( (NewObj->s.XX * NewObj->s.XX) + (NewObj->s.XY * NewObj->s.XY) + (NewObj->s.XZ * NewObj->s.XZ) + 
                  (NewObj->s.YX * NewObj->s.YX) + (NewObj->s.YY * NewObj->s.YY) + (NewObj->s.YZ * NewObj->s.YZ) +
                  (NewObj->s.ZX * NewObj->s.ZX) + (NewObj->s.ZY * NewObj->s.ZY) + (NewObj->s.ZZ * NewObj->s.ZZ) ) / 3.0f );

        if (rot3)   //if pointer not null
        {
            //this->getRotationScale(rot3);
            rot3->s.XX = NewObj->s.XX; rot3->s.XY = NewObj->s.XY; rot3->s.XZ = NewObj->s.XZ;
            rot3->s.YX = NewObj->s.YX; rot3->s.YY = NewObj->s.YY; rot3->s.YZ = NewObj->s.YZ;
            rot3->s.ZX = NewObj->s.ZX; rot3->s.ZY = NewObj->s.ZY; rot3->s.ZZ = NewObj->s.ZZ;

            // zero-div may occur.

            n = 1.0f / FuncSqrt( (NewObj->s.XX * NewObj->s.XX) +
                                      (NewObj->s.XY * NewObj->s.XY) +
                                      (NewObj->s.XZ * NewObj->s.XZ) );
            rot3->s.XX *= n;
            rot3->s.XY *= n;
            rot3->s.XZ *= n;

            n = 1.0f / FuncSqrt( (NewObj->s.YX * NewObj->s.YX) +
                                      (NewObj->s.YY * NewObj->s.YY) +
                                      (NewObj->s.YZ * NewObj->s.YZ) );
            rot3->s.YX *= n;
            rot3->s.YY *= n;
            rot3->s.YZ *= n;

            n = 1.0f / FuncSqrt( (NewObj->s.ZX * NewObj->s.ZX) +
                                      (NewObj->s.ZY * NewObj->s.ZY) +
                                      (NewObj->s.ZZ * NewObj->s.ZZ) );
            rot3->s.ZX *= n;
            rot3->s.ZY *= n;
            rot3->s.ZZ *= n;
        }

        if (rot4)   //if pointer not null
        {
            if (rot4 != NewObj)
            {
                Matrix4fSetRotationScaleFromMatrix4f(rot4, NewObj);  // private method
            }

            // zero-div may occur.

            n = 1.0f / FuncSqrt( (NewObj->s.XX * NewObj->s.XX) +
                                      (NewObj->s.XY * NewObj->s.XY) +
                                      (NewObj->s.XZ * NewObj->s.XZ) );
            rot4->s.XX *= n;
            rot4->s.XY *= n;
            rot4->s.XZ *= n;

            n = 1.0f / FuncSqrt( (NewObj->s.YX * NewObj->s.YX) +
                                      (NewObj->s.YY * NewObj->s.YY) +
                                      (NewObj->s.YZ * NewObj->s.YZ) );
            rot4->s.YX *= n;
            rot4->s.YY *= n;
            rot4->s.YZ *= n;

            n = 1.0f / FuncSqrt( (NewObj->s.ZX * NewObj->s.ZX) +
                                      (NewObj->s.ZY * NewObj->s.ZY) +
                                      (NewObj->s.ZZ * NewObj->s.ZZ) );
            rot4->s.ZX *= n;
            rot4->s.ZY *= n;
            rot4->s.ZZ *= n;
        }

        return s;
    }

    inline
    static void Matrix4fSetRotationScaleFromMatrix3f(Matrix4fT* NewObj, const Matrix3fT* m1)
    {
        assert(NewObj && m1);

        NewObj->s.XX = m1->s.XX; NewObj->s.YX = m1->s.YX; NewObj->s.ZX = m1->s.ZX;
        NewObj->s.XY = m1->s.XY; NewObj->s.YY = m1->s.YY; NewObj->s.ZY = m1->s.ZY;
        NewObj->s.XZ = m1->s.XZ; NewObj->s.YZ = m1->s.YZ; NewObj->s.ZZ = m1->s.ZZ;
    }

    inline
    static void Matrix4fMulRotationScale(Matrix4fT* NewObj, GLfloat scale)
    {
        assert(NewObj);

        NewObj->s.XX *= scale; NewObj->s.YX *= scale; NewObj->s.ZX *= scale;
        NewObj->s.XY *= scale; NewObj->s.YY *= scale; NewObj->s.ZY *= scale;
        NewObj->s.XZ *= scale; NewObj->s.YZ *= scale; NewObj->s.ZZ *= scale;
    }

    /**
      * Sets the rotational component (upper 3x3) of this matrix to the matrix
      * values in the T precision Matrix3d argument; the other elements of
      * this matrix are unchanged; a singular value decomposition is performed
      * on this object's upper 3x3 matrix to factor out the scale, then this
      * object's upper 3x3 matrix components are replaced by the passed rotation
      * components, and then the scale is reapplied to the rotational
      * components.
      * @param m1 T precision 3x3 matrix
      */
    inline
    static void Matrix4fSetRotationFromMatrix3f(Matrix4fT* NewObj, const Matrix3fT* m1)
    {
        GLfloat scale;

        assert(NewObj && m1);

        scale = Matrix4fSVD(NewObj, NULL, NULL);

        Matrix4fSetRotationScaleFromMatrix3f(NewObj, m1);
        Matrix4fMulRotationScale(NewObj, scale);
    }

// 8<--Snip here if you have your own math types/funcs-->8 

    class ArcBall_t
    {
        //protected:

        public:
            //inline
            void _mapToSphere(const Point2fT* NewPt, Vector3fT* NewVec);
            //Create/Destroy
                    ArcBall_t(GLfloat NewWidth, GLfloat NewHeight);
                   //~ArcBall_t() { /* nothing to do */ };

            //Set new bounds
            //inline
            void    setBounds(GLfloat NewWidth, GLfloat NewHeight)
            {
                assert((NewWidth > 1.0f) && (NewHeight > 1.0f));

                //Set adjustment factor for width/height
                this->AdjustWidth  = 1.0f / ((NewWidth  - 1.0f) * 0.5f);
                this->AdjustHeight = 1.0f / ((NewHeight - 1.0f) * 0.5f);
            }

            //Mouse down
            void    click(const Point2fT* NewPt);

            //Mouse drag, calculate rotation
            void    drag(const Point2fT* NewPt, Quat4fT* NewRot);

        protected:
            Vector3fT   StVec;          //Saved click vector
            Vector3fT   EnVec;          //Saved drag vector
            GLfloat     AdjustWidth;    //Mouse bounds width
            GLfloat     AdjustHeight;   //Mouse bounds height

    };

#endif
