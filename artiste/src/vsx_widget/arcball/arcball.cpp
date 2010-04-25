#ifndef VSX_NO_CLIENT
/** KempoApi: The Turloc Toolkit *****************************/
/** *    *                                                  **/
/** **  **  Filename: ArcBall.cpp                           **/
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

//#include <windows.h>											// Header File For Windows
//#include <gl\gl.h>												// Header File For The OpenGL32 Library
//#include <gl\glu.h>												// Header File For The GLu32 Library
//#include <gl\glaux.h>											// Header File For The GLaux Library
#include <vsx_gl_global.h>
#include <stdio.h>
#include <stdlib.h>

#include "math.h"                                               // Needed for sqrtf

#include "arcball.h"                                            // ArcBall header

//Arcball sphere constants:
//Diameter is       2.0f
//Radius is         1.0f
//Radius squared is 1.0f

void ArcBall_t::_mapToSphere(const Point2fT* NewPt, Vector3fT* NewVec) 
{
    Point2fT TempPt;
    GLfloat length;

    //Copy paramter into temp point
    TempPt = *NewPt;

    //Adjust point coords and scale down to range of [-1 ... 1]
    //TempPt.s.X  =        (TempPt.s.X * this->AdjustWidth)  - 1.0f;
    //TempPt.s.Y  = 1.0f - (TempPt.s.Y * this->AdjustHeight);

    //Compute the square of the length of the vector to the point from the center
    length      = (TempPt.s.X * TempPt.s.X) + (TempPt.s.Y * TempPt.s.Y);

    //If the point is mapped outside of the sphere... (length > radius squared)
    if (length > 1.0f)
    {
        GLfloat norm;

        //Compute a normalizing factor (radius / sqrt(length))
        norm    = 1.0f / FuncSqrt(length);

        //Return the "normalized" vector, a point on the sphere
        NewVec->s.X = TempPt.s.X * norm;
        NewVec->s.Y = TempPt.s.Y * norm;
        NewVec->s.Z = 0.0f;
    }
    else    //Else it's on the inside
    {
        //Return a vector to a point mapped inside the sphere sqrt(radius squared - length)
        NewVec->s.X = TempPt.s.X;
        NewVec->s.Y = TempPt.s.Y;
        NewVec->s.Z = FuncSqrt(1.0f - length);
    }
}

//Create/Destroy
ArcBall_t::ArcBall_t(GLfloat NewWidth, GLfloat NewHeight)
{
    //Clear initial values
    this->StVec.s.X     =
    this->StVec.s.Y     = 
    this->StVec.s.Z     = 

    this->EnVec.s.X     =
    this->EnVec.s.Y     = 
    this->EnVec.s.Z     = 0.0f;

    //Set initial bounds
    this->setBounds(NewWidth, NewHeight);
}

//Mouse down
void    ArcBall_t::click(const Point2fT* NewPt)
{
    //Map the point to the sphere
    this->_mapToSphere(NewPt, &this->StVec);
}

//Mouse drag, calculate rotation
void    ArcBall_t::drag(const Point2fT* NewPt, Quat4fT* NewRot)
{
    //Map the point to the sphere
    this->_mapToSphere(NewPt, &this->EnVec);

    //Return the quaternion equivalent to the rotation
    if (NewRot)
    {
        Vector3fT  Perp; 
        
        //this->StVec.s.X     = 0;
        //this->StVec.s.Y     = 0;
        //this->StVec.s.Z     =  1;

        //Compute the vector perpendicular to the begin and end vectors
        Vector3fCross(&Perp, &this->StVec, &this->EnVec);
        //printf("perp.z = %f\n",Perp.s.Z);
        //Compute the length of the perpendicular vector
        if (Vector3fLength(&Perp) > Epsilon)    //if its non-zero
        {
            //We're ok, so return the perpendicular vector as the transform after all
            NewRot->s.X = Perp.s.X;
            NewRot->s.Y = Perp.s.Y;
            NewRot->s.Z = Perp.s.Z;

            //In the quaternion values, w is cosine (theta / 2), where theta is rotation angle
            NewRot->s.W= -Vector3fDot(&this->StVec, &this->EnVec);
        }
        else                                    //if its zero
        {
            //The begin and end vectors coincide, so return an identity transform
            NewRot->s.X = 
            NewRot->s.Y = 
            NewRot->s.Z = 
            NewRot->s.W = 0.0f;
        }
        //printf("new rot quat = %f,%f,%f,%f\n",NewRot->s.X,NewRot->s.Y,NewRot->s.Z,NewRot->s.W);
    }
}


#endif
