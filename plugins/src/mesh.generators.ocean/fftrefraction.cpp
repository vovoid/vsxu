/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Joe Dart. Cleaned up for VSXu by Jonatan "jaw" Wallmander
*
* License: Public Domain
*/


//
// fftrefraction.cpp
// Visualize paul bourke's fft. This program takes Jensen into account.
// It also uses Chen water ideas and some Nutman cubemap code.
//
//	 joe dart ffjjd@uaf.edu
//		This program is first version of cg refraction.
//		This program runs correctly with "choppy" waves.
//		june 22, 2003
//

#include "fftrefraction.h"
#include "matrix.h"

#include <math.h>

//
// Definitions used only in this module.
//
#define ETARATIO (1.0003)/(1.3333)
//#define ETARATIO (1.3333)/(1.0003)

#define	GL_NORMAL_MAP_ARB						0x8511
#define	GL_REFLECTION_MAP_ARB					0x8512
#define	GL_TEXTURE_CUBE_MAP_ARB					0x8513
#define	GL_TEXTURE_BINDING_CUBE_MAP_ARB			0x8514
#define	GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB		0x8515
#define	GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB		0x8516
#define	GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB		0x8517
#define	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB		0x8518
#define	GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB		0x8519
#define	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB		0x851A
#define	GL_PROXY_TEXTURE_CUBE_MAP_ARB			0x851B
#define	GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB		0x851C

// AUTO MIPMAPPING EXTENSION
#define GL_GENERATE_MIPMAP_SGIS	0x8191
#define GL_CLAMP_TO_EDGE_EXT	0x812F //required for smooth normalization map(Nutman)


//
// Constructor
//
Alaska::Alaska()
{
  normals_only = false;
  GRAV_CONSTANT = 30.81f; //gravitational constant metric
  lambda = 0.29;
  dir = 1;
  a_global=.0008f; // phillips constant
  wind_global[0] = 20;
  wind_global[1] = 30;
  scale_height = 0.25;//scale the wave heights

  //	wind = .7;
  wind = 0.1;
  factor = 10.0;	//this determines speed of wave
}


// these two functions set up the phillips surface. calculate_h0() calls phillips()
double Alaska::phillips(double a,double k[2],double wind[2])
{
	double k2 = k[0]*k[0]+k[1]*k[1];
	if (k2==0)
		return 0;
	double v2 = wind[0]*wind[0]+wind[1]*wind[1];
	double EL = v2 / GRAV_CONSTANT;
	// the factor *exp(-sqrt(k2)*1.0) can get rid of small waves by increasing 1.0
	double ret = a*(exp(-1/(k2*(EL)*(EL)))/(k2*k2))*
		((k[0]*wind[0]+k[1]*wind[1])*(k[0]*wind[0]+k[1]*wind[1]) / (k2*v2))*exp(-sqrt(k2)*1.0);
	// this value must be positive since we take the square root later
	return ret;
}

void	Alaska::calculate_ho()
{

	double horizontal[2];
	double root_of_phillips;
	double gauss_value[2];// this is where gauss generator returns two random values
	for (int i=0;i<NX;i++)
	{
		for (int j=0;j<NY;j++)
		{
			// hold_horizontal saves these fixed calculations for later use:k[2],klen,klen*klen
			horizontal[0]=hold_horizontal[i][j][0]=2.0*PI*((double)i-.5*NX)/MAX_WORLD_X;// center the origin
			horizontal[1]=hold_horizontal[i][j][1]=2.0*PI*((double)j-.5*NX)/MAX_WORLD_Y;// center the origin
			hold_horizontal[i][j][3]=hold_horizontal[i][j][0]*hold_horizontal[i][j][0]+hold_horizontal[i][j][1]*hold_horizontal[i][j][1];
			hold_horizontal[i][j][2]=sqrt(hold_horizontal[i][j][3]);

		//	horizontal[0]=hold_horizontal[i][j][0]=2.0*PI*((float)i-.5*NX)/MAX_WORLD_X;
		//	horizontal[1]=hold_horizontal[i][j][1]=2.0*PI*((float)j-.5*NX)/MAX_WORLD_Y;

			gauss(gauss_value);
			root_of_phillips=sqrt(phillips(a_global,horizontal,wind_global));

			mH0[i][j].real=INV_SQRT_TWO*gauss_value[0]*root_of_phillips;
			mH0[i][j].imag=INV_SQRT_TWO*gauss_value[1]*root_of_phillips;
		}
	}
}


/////////////////// calculate the normal map///////////////


void Alaska::my_normalize(Vector3 &vec)
{
	float mag = (vec.x*vec.x) + (vec.y*vec.y) + (vec.z*vec.z);

	mag = 1.0f / sqrtf(mag);

	vec.x *= mag;
	vec.y *= mag;
	vec.z *= mag;
}

void Alaska::make_signed(Vector3 &v, unsigned char *ip)
{
	*(ip++) = (unsigned char) ((v.x * 127.0f) + 127.0f);
	*(ip++) = (unsigned char) ((v.y * 127.0f) + 127.0f);
	*(ip++) = (unsigned char) ((v.z * 127.0f) + 127.0f);
}


void Alaska::display(void)
{

  //double kvector[2];
  double klength,wkt;
  //////////////////alernativ below///////////
	int yHalf = NY/2 + 1;
	for (int i = 0; i<yHalf; ++i)
	{
		//int yLine = i*NY;
		// Mirror the y line index for calculation of -k
		// The line below evalutes yLineMirr = y == 0 ? 0 : (mYSize-y)*mXSize;
		// by wrapping the heightmap, since it is periodic.
		//int yLineMirr = ((NY-i)% NY)*NY;
		for (int j = 0; j<NX; ++j)
		{
			//kvector[0]=2.0*PI*((float)i-.5*NX)/NX;//I was using these
			//kvector[1]=2.0*PI*((float)j-.5*NY)/NY;

	//		kvector[0]=2.0*PI*((float)i-.5*NX)/MAX_WORLD_X;//but I think I should use these
	//		kvector[1]=2.0*PI*((float)j-.5*NY)/MAX_WORLD_Y;

      //kvector[0]=hold_horizontal[i][j][0];
      //kvector[1]=hold_horizontal[i][j][1];

		//	klength=sqrt(kvector[0]*kvector[0]+kvector[1]*kvector[1]);
			klength=hold_horizontal[i][j][2];

			wkt = sqrt(klength * GRAV_CONSTANT) * dtime;

			// yLineMirr+mXSize-x is the index of -k
			//int  kNegIndex = yLineMirr*NY + ((NY-j)% NY);

			// This is h~(K, t) from the Tessendorf paper.
		   	c[i][j].real= mH0[i][j].real*cos(wkt)
				+ mH0[i][j].imag*sin(wkt)
				+ mH0[NX - i-1][NY - j-1].real*cos(wkt)
				- mH0[NX - i-1][NY -j-1].imag*sin(wkt);

			c[i][j].imag= mH0[i][j].imag*cos(wkt)
				+ mH0[i][j].real*sin(wkt)
				-mH0[NX - i-1][NY - j-1].imag*cos(wkt)
	 			- mH0[NX - i-1][NY -j-1].real*sin(wkt);

			// Store it for later transformation

			// h~(-K) = conj(h~(K))
			if (i != yHalf-1)
			{
				c[NX - i-1][NY - j-1].imag= mH0[i][j].real*cos(wkt)
				+ mH0[i][j].imag*sin(wkt)
				+ mH0[NX - i-1][NY - j-1].real*cos(wkt)
				- mH0[NX - i-1][NY -j-1].imag*sin(wkt);

				c[NY - i-1][NY - j-1].real= mH0[i][j].imag*cos(wkt)
				+ mH0[i][j].real*sin(wkt)
				-mH0[NX - i-1][NY - j-1].imag*cos(wkt)
	 			- mH0[NX - i-1][NY -j-1].real*sin(wkt);
			}
		}
	}

		//////////////////end alternative////////////
	pre_choppy();
	dir=-1;
	FFT2D(c, NX, NY, dir);// do the inverse FFT to get the surface
	///////////////negative power term creation
	for (int i=0;i<NX;i++)
	{
		for (int j=0;j<NY;j++)
		{
			c[i][j].real *= float(neg1Pow(i+j));
	
			// while we are looping, set up the final x,y values for display
			displayXY[i][j][0]=((double)i/NX)*MAX_WORLD_X+mDeltaX[i][j].imag;
			displayXY[i][j][1]=((double)j/NY)*MAX_WORLD_Y+mDeltaY[i][j].imag;
		}
	}
	make_normals(c);

	prep_loop();	//this loop loads the actual sea vertices


}

void Alaska::make_normals(COMPLEX c[NX][NY])
{
	double x_value,y_value;
	x_value=MAX_WORLD_X/NX;
	y_value=MAX_WORLD_Y/NY;

	for (int i=0;i<NX-1;i++)
	{
		for (int j=0;j<NY-1;j++)
		{
			ta[0]=x_value;
			ta[1]=0.0;
			ta[2]=(c[i+1][j].real-c[i][j].real)*scale_height;
			tb[0]=0.0;
			tb[1]=y_value;
			tb[2]=(c[i][j+1].real-c[i][j].real)*scale_height;
			cross_prod(ta,tb,tc);
			normals[i][j][0]=tc[0];
			normals[i][j][1]=tc[1];
			normals[i][j][2]=tc[2];
		}
	}

	for (int i=0;i<NX;i++)
	{
		normals[i][NX-1][0]=normals[i][0][0];
		normals[i][NX-1][1]=normals[i][0][1];
		normals[i][NX-1][2]=normals[i][0][2];

		normals[NX-1][i][0]=normals[0][i][0];
		normals[NX-1][i][1]=normals[0][i][1];
		normals[NX-1][i][2]=normals[0][i][2];
	}
}


void Alaska::myinit(void)
{

	calculate_ho();
}



float	Alaska::neg1Pow(int k)
{
	return pow(-1,k);
}

void	Alaska::pre_choppy()
{
	//this function sets up the DX DY choppiness
	// it assumes that the current c values are in position
	// before the c values have been FFT'd

	double k[2],klen;

	for (int i=0;i<NX;i++)
	{
		for (int j=0;j<NY;j++)
		{

		//	k[0]=2.0*PI*((float)i-.5*NX)/MAX_WORLD_X;
		//	k[1]=2.0*PI*((float)j-.5*NY)/MAX_WORLD_Y;

			k[0]=hold_horizontal[i][j][0];
			k[1]=hold_horizontal[i][j][1];

			klen=sqrt(k[0]*k[0]+k[1]*k[1]);
			if (klen==0)
			{
				mDeltaX[i][j].real=0.0;
				mDeltaX[i][j].imag=0.0;
				mDeltaY[i][j].real=0.0;
				mDeltaY[i][j].imag=0.0;
			}
			else
			{
				mDeltaX[i][j].real=0.0;
				mDeltaX[i][j].imag=c[i][j].imag*(-k[0]/klen);
				mDeltaY[i][j].real=0.0;
				mDeltaY[i][j].imag=c[i][j].imag*(-k[1]/klen);
			}
		}
	}

	dir=-1;
	if (FFT2D(mDeltaX, NX, NY, dir)==FALSE){printf("false on DXFFT!\n");}
	dir=-1;
	if (FFT2D(mDeltaY, NX, NY, dir)==FALSE){printf("false on DYFFT!\n");}

	for (int i=0;i<NX;i++)
	{
		for (int j=0;j<NY;j++)
		{
			mDeltaX[i][j].real *= (double) neg1Pow(i+j)*lambda;
			mDeltaX[i][j].imag *= (double) neg1Pow(i+j)*lambda;
			mDeltaY[i][j].real *= (double) neg1Pow(i+j)*lambda;
			mDeltaY[i][j].imag *= (double) neg1Pow(i+j)*lambda;
		}
	}

}

void	Alaska::prep_loop()
{
  if (normals_only)
  {
    for (int i=0;i<NX;i++)
    {
      for (int j=0;j<NY;j++)
      {
        sea[i][j][0]=(float)i;
        sea[i][j][1]=(float)j;
        sea[i][j][2]=c[i][j].real;

        big_normals[i][j][0]=normals[i][j][0];
        big_normals[i][j][1]=normals[i][j][1];
        big_normals[i][j][2]=normals[i][j][2];
      }
    }
  }
  else
  {
    for (int i=0;i<NX;i++)
    {
      for (int j=0;j<NY;j++)
      {
        sea[i][j][0]=displayXY[i][j][0];
        sea[i][j][1]=displayXY[i][j][1];
        sea[i][j][2]=c[i][j].real;

        big_normals[i][j][0]=normals[i][j][0];
        big_normals[i][j][1]=normals[i][j][1];
        big_normals[i][j][2]=normals[i][j][2];
      }
    }
  }

	//now fill in the final row and column of the sea correctly
	// and fill in the normals
	for (int i=0;i<NX;i++)
	{
		sea[BIG_NX-1][i][0]=sea[0][i][0]+MAX_WORLD_X;
		sea[BIG_NX-1][i][1]=sea[0][i][1];
		sea[BIG_NX-1][i][2]=sea[0][i][2];

		sea[i][BIG_NY-1][0]=sea[i][0][0];
		sea[i][BIG_NY-1][1]=sea[i][0][1]+MAX_WORLD_Y;
		sea[i][BIG_NY-1][2]=sea[i][0][2];

		big_normals[BIG_NX-1][i][0]=big_normals[0][i][0];
		big_normals[BIG_NX-1][i][1]=big_normals[0][i][1];
		big_normals[BIG_NX-1][i][2]=big_normals[0][i][2];

		big_normals[i][BIG_NY-1][0]=big_normals[i][0][0];
		big_normals[i][BIG_NY-1][1]=big_normals[i][0][1];
		big_normals[i][BIG_NY-1][2]=big_normals[i][0][2];
	}
	sea[BIG_NX-1][BIG_NY-1][0]=sea[0][0][0]+MAX_WORLD_X;
	sea[BIG_NX-1][BIG_NY-1][1]=sea[0][0][1]+MAX_WORLD_Y;
	sea[BIG_NX-1][BIG_NY-1][2]=sea[0][0][2];

	big_normals[BIG_NX-1][BIG_NY-1][0]=big_normals[0][0][0];
	big_normals[BIG_NX-1][BIG_NY-1][1]=big_normals[0][0][1];
	big_normals[BIG_NX-1][BIG_NY-1][2]=big_normals[0][0][2];
}

// This function calculates the surface hight points c[x][y].real each time
void Alaska::idle(void)
{
}
