/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Joe Dart. Cleaned up for VSXu by Jonatan "jaw" Wallmander
*
* License: Public Domain
*/


#ifndef FFTREFRACTION_H
#define FFTREFRACTION_H

#include <stdio.h>
#include <GL/glew.h>
#include "paulslib.h"

struct SVertex3
{
	float x, y, z;
};

struct Vector3
{
	float x, y, z;
};

#define NX 64
#define NY 64

#define INV_SQRT_TWO (1.0f)/sqrt(2.0f)
#define MAX_WORLD_X 64
#define MAX_WORLD_Y 64
#define BIG_NX (NX+1)
#define BIG_NY (NY+1)
#define STEPX MAX_WORLD_X/NX

#define MINX 0
#define MAXX 100
#define MINY 0
#define MAXY 100

//#define GRAV_CONSTANT	9.81f //gravitational constant metric



/**
 * This class contains all the methods and data for the application.
 */
class Alaska
{
public:
  bool normals_only;
  float GRAV_CONSTANT;
  double hold_horizontal[NX][NY][4];//store k[0],k[1],klen,klen*klen
  double sea[BIG_NX][BIG_NY][3];
  COMPLEX c[NX][NY];
  COMPLEX mH0[NX][NY];
  COMPLEX mDeltaX[NX][NY];
  COMPLEX mDeltaY[NX][NY];
  GLubyte * byteptr;
  double displayXY[NX][NY][2]; // holds the actual x and y for display
  
  double a_global; // phillips constant
  double wind_global[2];
  double scale_height;	//scale the wave heights
  int deep;
  double normals[NX][NY][3];
  double big_normals[BIG_NX][BIG_NY][3];
  double ta[3],tb[3],tc[3];
  


  Alaska();

  float time;
  float dtime;
  void	calculate_ho();
  void display(void);
  void make_normals(COMPLEX c[NX][NY]);
  void myinit(void);
  void pre_choppy();
  void prep_loop();
  void idle(void);

  // Helpers
  double phillips(double a,double k[2],double wind[2]);
  void my_normalize(Vector3 &vec);
  void make_signed(Vector3 &v, unsigned char *ip);
  int	Powerof2(int,int *,int *);
  float	neg1Pow(int k);



  GLubyte alpha_color_array[3];
  double lambda;
  float fog_dense;
  int dir;
  float wind;

  double factor;	//this determines speed of wave
  double start_time;
};

  void gauss(double mywork[2]);
int	FFT(int,int,double *,double *);
int	DFT(int,int,double *,double *);
int	FFT2D(COMPLEX [][NY], int,int,int);


#endif
