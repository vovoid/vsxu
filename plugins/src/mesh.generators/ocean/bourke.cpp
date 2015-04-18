/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Paul Bourke
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

#include <iostream>
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "paulslib.h"
int    FFT(int,int,double *,double *);
int    FFT2D(COMPLEX **, int,int,int);
int    DFT(int,int,double *,double *);
int    Powerof2(int,int *,int *);

/*-------------------------------------------------------------------------
   This computes an in-place complex-to-complex FFT
   x and y are the real and imaginary arrays of 2^m points.
   dir =  1 gives forward transform
   dir = -1 gives reverse transform

     Formula: forward
                  N-1
                  ---
              1   \          - j k 2 pi n / N
      X(n) = ---   >   x(k) e                    = forward transform
              N   /                                n=0..N-1
                  ---
                  k=0

      Formula: reverse
                  N-1
                  ---
                  \          j k 2 pi n / N
      x(n) =       >   X(k) e                    = reverse transform?
                  /                                n=0..N-1
                  ---
                  k=0
*/
int FFT(int dir,int m,double *x,double *y)
{
   long nn,i,i1,j,k,i2,l,l1,l2;
   double c1,c2,tx,ty,t1,t2,u1,u2,z;

   /* Calculate the number of points 
   nn = 1;
   for (i=0;i<m;i++)
      nn *= 2;
	*/
	nn = 1 << m;

   /* Do the bit reversal */
   i2 = nn >> 1;
   j = 0;
   for (i=0;i<nn-1;i++) {
      if (i < j) {
         tx = x[i];
         ty = y[i];
         x[i] = x[j];
         y[i] = y[j];
         x[j] = tx;
         y[j] = ty;
      }
      k = i2;
      while (k <= j) {
         j -= k;
         k >>= 1;
      }
      j += k;
   }

   /* Compute the FFT */
   c1 = -1.0;
   c2 = 0.0;
   l2 = 1;
   for (l=0;l<m;l++) {
      l1 = l2;
      l2 <<= 1;
      u1 = 1.0;
      u2 = 0.0;
      for (j=0;j<l1;j++) {
         for (i=j;i<nn;i+=l2) {
            i1 = i + l1;
            t1 = u1 * x[i1] - u2 * y[i1];
            t2 = u1 * y[i1] + u2 * x[i1];
            x[i1] = x[i] - t1;
            y[i1] = y[i] - t2;
            x[i] += t1;
            y[i] += t2;
         }
         z =  u1 * c1 - u2 * c2;
         u2 = u1 * c2 + u2 * c1;
         u1 = z;
      }
      c2 = sqrt((1.0 - c1) / 2.0);
      if (dir == 1)
         c2 = -c2;
      c1 = sqrt((1.0 + c1) / 2.0);
   }

   /* Scaling for forward transform */
   if (dir == 1) {
      for (i=0;i<nn;i++) {
         x[i] /= (double)nn;
         y[i] /= (double)nn;
      }
   }

   return(TRUE);
}

/*-------------------------------------------------------------------------
   Perform a 2D FFT inplace given a complex 2D array
   The direction dir, 1 for forward, -1 for reverse
   The size of the array (nx,ny)
   Return false if there are memory problems or
      the dimensions are not powers of 2
*/
//int FFT2D(COMPLEX **c,int nx,int ny,int dir)

double *realx = 0;
double *imagx = 0;
double *realy = 0;
double *imagy = 0;

int FFT2D(COMPLEX c[][256],int nx,int ny,int dir)
{
   int i,j;
   int m,twopm;

   /* Transform the rows */
   if (realx == 0) {
    realx = (double *)malloc(nx * sizeof(double));
    imagx = (double *)malloc(nx * sizeof(double));
    realy = (double *)malloc(ny * sizeof(double));
    imagy = (double *)malloc(ny * sizeof(double));
   }
   //if (real == NULL || imag == NULL)
     // return(FALSE);
   if (!Powerof2(nx,&m,&twopm) || twopm != nx)
      return(FALSE);
   for (j=0;j<ny;j++) {
      for (i=0;i<nx;i++) {
         realx[i] = c[i][j].real;
         imagx[i] = c[i][j].imag;
      }
      FFT(dir,m,realx,imagx);
      for (i=0;i<nx;i++) {
         c[i][j].real = realx[i];
         c[i][j].imag = imagx[i];
      }
   }
   //free(real);
   //free(imag);

   /* Transform the columns */
   //real = (double *)malloc(ny * sizeof(double));
   //imag = (double *)malloc(ny * sizeof(double));
   //if (real == NULL || imag == NULL)
//      return(FALSE);
   if (!Powerof2(ny,&m,&twopm) || twopm != ny)
      return(FALSE);
   for (i=0;i<nx;i++) {
      for (j=0;j<ny;j++) {
         realy[j] = c[i][j].real;
         imagy[j] = c[i][j].imag;
      }
      FFT(dir,m,realy,imagy);
      for (j=0;j<ny;j++) {
         c[i][j].real = realy[j];
         c[i][j].imag = imagy[j];
      }
   }
   //free(real);
   //free(imag);

   return(TRUE);
}




int FFT2D(COMPLEX c[][128],int nx,int ny,int dir)
{
   int i,j;
   int m,twopm;

   /* Transform the rows */
   if (realx == 0) {
    realx = (double *)malloc(nx * sizeof(double));
    imagx = (double *)malloc(nx * sizeof(double));
    realy = (double *)malloc(ny * sizeof(double));
    imagy = (double *)malloc(ny * sizeof(double));
   }
   //if (real == NULL || imag == NULL)
     // return(FALSE);
   if (!Powerof2(nx,&m,&twopm) || twopm != nx)
      return(FALSE);
   for (j=0;j<ny;j++) {
      for (i=0;i<nx;i++) {
         realx[i] = c[i][j].real;
         imagx[i] = c[i][j].imag;
      }
      FFT(dir,m,realx,imagx);
      for (i=0;i<nx;i++) {
         c[i][j].real = realx[i];
         c[i][j].imag = imagx[i];
      }
   }
   //free(real);
   //free(imag);

   /* Transform the columns */
   //real = (double *)malloc(ny * sizeof(double));
   //imag = (double *)malloc(ny * sizeof(double));
   //if (real == NULL || imag == NULL)
//      return(FALSE);
   if (!Powerof2(ny,&m,&twopm) || twopm != ny)
      return(FALSE);
   for (i=0;i<nx;i++) {
      for (j=0;j<ny;j++) {
         realy[j] = c[i][j].real;
         imagy[j] = c[i][j].imag;
      }
      FFT(dir,m,realy,imagy);
      for (j=0;j<ny;j++) {
         c[i][j].real = realy[j];
         c[i][j].imag = imagy[j];
      }
   }
   //free(real);
   //free(imag);

   return(TRUE);
}



int FFT2D(COMPLEX c[][64],int nx,int ny,int dir)
{
   int i,j;
   int m,twopm;
   

   /* Transform the rows */
   if (realx == 0) {
    realx = (double *)malloc(nx * sizeof(double));
    imagx = (double *)malloc(nx * sizeof(double));
    realy = (double *)malloc(ny * sizeof(double));
    imagy = (double *)malloc(ny * sizeof(double));
   }
   //if (real == NULL || imag == NULL)
     // return(FALSE);
   if (!Powerof2(nx,&m,&twopm) || twopm != nx)
      return(FALSE);
   for (j=0;j<ny;j++) {
      for (i=0;i<nx;i++) {
         realx[i] = c[i][j].real;
         imagx[i] = c[i][j].imag;
      }
      FFT(dir,m,realx,imagx);
      for (i=0;i<nx;i++) {
         c[i][j].real = realx[i];
         c[i][j].imag = imagx[i];
      }
   }
   //free(real);
   //free(imag);

   /* Transform the columns */
   //real = (double *)malloc(ny * sizeof(double));
   //imag = (double *)malloc(ny * sizeof(double));
   //if (real == NULL || imag == NULL)
//      return(FALSE);
   if (!Powerof2(ny,&m,&twopm) || twopm != ny)
      return(FALSE);
   for (i=0;i<nx;i++) {
      for (j=0;j<ny;j++) {
         realy[j] = c[i][j].real;
         imagy[j] = c[i][j].imag;
      }
      FFT(dir,m,realy,imagy);
      for (j=0;j<ny;j++) {
         c[i][j].real = realy[j];
         c[i][j].imag = imagy[j];
      }
   }
   //free(real);
   //free(imag);

   return(TRUE);
}


int FFT2D(COMPLEX c[][32],int nx,int ny,int dir)
{
   int i,j;
   int m,twopm;
   

   /* Transform the rows */
   if (realx == 0) {
    realx = (double *)malloc(nx * sizeof(double));
    imagx = (double *)malloc(nx * sizeof(double));
    realy = (double *)malloc(ny * sizeof(double));
    imagy = (double *)malloc(ny * sizeof(double));
   }
   //if (real == NULL || imag == NULL)
     // return(FALSE);
   if (!Powerof2(nx,&m,&twopm) || twopm != nx)
      return(FALSE);
   for (j=0;j<ny;j++) {
      for (i=0;i<nx;i++) {
         realx[i] = c[i][j].real;
         imagx[i] = c[i][j].imag;
      }
      FFT(dir,m,realx,imagx);
      for (i=0;i<nx;i++) {
         c[i][j].real = realx[i];
         c[i][j].imag = imagx[i];
      }
   }
   //free(real);
   //free(imag);

   /* Transform the columns */
   //real = (double *)malloc(ny * sizeof(double));
   //imag = (double *)malloc(ny * sizeof(double));
   //if (real == NULL || imag == NULL)
//      return(FALSE);
   if (!Powerof2(ny,&m,&twopm) || twopm != ny)
      return(FALSE);
   for (i=0;i<nx;i++) {
      for (j=0;j<ny;j++) {
         realy[j] = c[i][j].real;
         imagy[j] = c[i][j].imag;
      }
      FFT(dir,m,realy,imagy);
      for (j=0;j<ny;j++) {
         c[i][j].real = realy[j];
         c[i][j].imag = imagy[j];
      }
   }
   //free(real);
   //free(imag);

   return(TRUE);
}



/*-------------------------------------------------------------------------
        Direct fourier transform
*/
int DFT(int dir,int m,double *x1,double *y1)
{
   long i,k;
   double arg;
   double cosarg,sinarg;
	double *x2=NULL,*y2=NULL;

	x2 =(double *) malloc(m*sizeof(double));
   y2 = (double *)malloc(m*sizeof(double));
	if (x2 == NULL || y2 == NULL)
		return(FALSE);

   for (i=0;i<m;i++) {
      x2[i] = 0;
      y2[i] = 0;
      arg = - dir * 2.0 * 3.141592654 * (double)i / (double)m;
      for (k=0;k<m;k++) {
         cosarg = cos(k * arg);
         sinarg = sin(k * arg);
         x2[i] += (x1[k] * cosarg - y1[k] * sinarg);
         y2[i] += (x1[k] * sinarg + y1[k] * cosarg);
      }
   }

   /* Copy the data back */
   if (dir == 1) {
      for (i=0;i<m;i++) {
         x1[i] = x2[i] / (double)m;
         y1[i] = y2[i] / (double)m;
      }
   } else {
      for (i=0;i<m;i++) {
         x1[i] = x2[i];
         y1[i] = y2[i];
      }
   }

	free(x2);
	free(y2);
   return(TRUE);
}

/*-------------------------------------------------------------------------
	Calculate the closest but lower power of two of a number
	twopm = 2**m <= n
	Return TRUE if 2**m == n
*/
int Powerof2(int n,int *m,int *twopm)
{
	if (n <= 1) {
		*m = 0;
		*twopm = 1;
		return(FALSE);
	}

   *m = 1;
   *twopm = 2;
   do {
      (*m)++;
      (*twopm) *= 2;
   } while (2*(*twopm) <= n);

   if (*twopm != n) 
		return(FALSE);
	else
		return(TRUE);
}
