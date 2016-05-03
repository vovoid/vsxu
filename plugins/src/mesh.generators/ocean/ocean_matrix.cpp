/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Joe Dart. Cleaned up for VSXu by Jonatan "jaw" Wallmander
*
* License: Public Domain
*/

//
// Helper functions for matrix and vector math.
//

#include "matrix.h"
#include <math.h>

/////////////////////////////////// the work matrices for transforms//////////////
//all of these must be initialized with setIdentity() at the start of each pass.

// to rotate over x
//	rotmatx[5]=cos(a)
//	rotmatx[6]=-sin(a)
//	rotmatx[9]=sin(a)
//	rotmatx[10]=cos(a)

//to rotate over y
//	rotmaty[0]=cos(a)
//	rotmaty[2]=sin(a)
//	rotmaty[8]=-sin(a)
//	rotmaty[10]=cos(a)

// to rotate over z
//	rotmatz[0]=cos(a)
//	rotmatz[1]=-sin(a)
//	rotmatz[4]=sin(a)
//	rotmatz[5]=cos(a)

// to translate
//	transmat[3]=x
//	transmat[7]=y
//	transmat[11=z

float identity[16]={1.0,0.0,0.0,0.0,
					0.0,1.0,0.0,0.0,
					0.0,0.0,1.0,0.0,
					0.0,0.0,0.0,1.0};
float rotmatx[16]={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
float rotmaty[16]={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
float rotmatz[16]={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
float transmat[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void	setIdentity(float a[16])
{
	for (int i=0;i<16;i++)
		a[i]=identity[i];
}

void	multMatrices( float a[16],  float b[16])  //this may not back multiply
{
	int i, j;
	float work_matrix[16];

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			work_matrix[i * 4 + j] =
			a[i * 4 + 0] * b[0 * 4 + j] +
			a[i * 4 + 1] * b[1 * 4 + j] +
			a[i * 4 + 2] * b[2 * 4 + j] +
			a[i * 4 + 3] * b[3 * 4 + j];

			/*	a[0 * 4 + j]*b[i * 4 + 0]+
				a[1 * 4 + j]*b[i * 4 + 1]+
				a[2 * 4 + j]*b[i * 4 + 2]+
				a[3 * 4 + j]*b[i * 4 + 3]; */
		}
	}
	for (i=0;i<16;i++)
	{
		a[i]=work_matrix[i];
	}
}

void	mytranslate(float a[16],float x,float y,float z)
{
	a[3]  = (x==0.0f) ? 0.0f : x;
	a[7]  = (y==0.0f) ? 0.0f : y;
	a[11] = (z==0.0f) ? 0.0f : z;
}

// to rotate over x
//	rotmatx[5]=cos(a)
//	rotmatx[6]=-sin(a)
//	rotmatx[9]=sin(a)
//	rotmatx[10]=cos(a)

//to rotate over y
//	rotmaty[0]=cos(a)
//	rotmaty[2]=sin(a)
//	rotmaty[8]=-sin(a)
//	rotmaty[10]=cos(a)

// to rotate over z
//	rotmatz[0]=cos(a)
//	rotmatz[1]=-sin(a)
//	rotmatz[4]=sin(a)
//	rotmatz[5]=cos(a)

void	myrotate(float a[16],float x,float y,float z)
{
	float cosa,sina;
	if (x !=0.0)
	{
		cosa = cosf(x*TO_RADS);
		sina = sinf(x*TO_RADS);
		rotmatx[5]=cosa;
		rotmatx[6]=-sina;
		rotmatx[9]=sina;
		rotmatx[10]=cosa;
		multMatrices(a,rotmatx);
	}

	if (y != 0.0)
	{
		cosa = cosf(y*TO_RADS);
		sina = sinf(y*TO_RADS);
		rotmaty[0] = cosa;
		rotmaty[2] = sina;
		rotmaty[8] = -sina;
		rotmaty[10] = cosa;
		multMatrices(a,rotmaty);
	}
	if (z !=0.0)
	{
		cosa=cosf(z*TO_RADS);
		sina=sinf(z*TO_RADS);
		rotmatz[0] = cosa;
		rotmatz[1] = -sina;
		rotmatz[4] = sina;
		rotmatz[5] = cosa;
		multMatrices(a,rotmatz);
	}
}

void	normalize(double vec[])
{
	double Length=sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);
	if (Length==0.0) Length=1.0;
	vec[0] /=Length;
	vec[1] /=Length;
	vec[2] /=Length;
}

void	cross_prod(double in1[],double in2[],double out[3])
{
	out[0]=(in1[1]*in2[2])-(in2[1]*in1[2]);
	out[1]=(in1[2]*in2[0])-(in2[2]*in1[0]);
	out[2]=(in1[0]*in2[1])-(in2[0]*in1[1]);
	normalize(out);
}

