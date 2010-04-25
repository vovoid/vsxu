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

//#include <stdlib.h>
//#include <stdio.h>
#include <math.h>
//#include <time.h>
//#include <sys/types.h>	// needed to use _timeb
//#include <sys/timeb.h>	// needed to use _timeb
/*extern "C"
{
#include "jpeglib.h"
}*/

//#include <iostream>
//using namespace std;

//
// There is only one instance of the application class.
//
//Alaska	 app;

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


///////////////////////////// cg declarations/////////////////////
//float fresnelbias=.6;
//float fresnelscale=0.5;
//float fresnelpower=0.7;
//float fresnelbias=.5;
//float fresnelscale=1.0;
//float fresnelpower=1.0;
//float etaratio=ETARATIO;

//float texturerotmat[16];
//float watertoworld[16];
//float mirror=.52;//this is the reflectivity value
//float eye_is_at[]={127.0, 130.0, -135.0};
//float eye_is_at[]={128.0, 128.0, -135.0};
//float light_is_at[]={-600.,600.,194.};
//float *vertexPositions;
//float *normalvector;
//float *vertexTexCoords;//new
//GLuint cubetexture; //this should contain binder for cubemap
//GLuint CubeMap;
//GLuint BottomMap;
//float constantColor[4]={1.,0.,0.,1.};

//CGcontext context;
//CGprogram vertexProgram, fragmentProgram, f, v;
//CGprofile vertexProfile,fragmentProfile, fprofile, vprofile;
//CGparameter position, texCoord, normal, foam, modelToWorld,modelViewProj,eyepositionW,bottomMap,environmentMap,fresnelBias,fresnelScale,fresnelPower,etaRatio,normalizeCube,normalizeCube2,lightposition,normalMap,normalMapTexCoord;



////////////////////////////end cg declarations//////////////////

/*struct TheCubes
{
	GLubyte *wall;
} TheCube[6];

TheCubes TheBottom[6];
*/
/////////////////////////////////////////////////////////////
/*
GLuint cube_map_directions[6] =
{
	GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB
};

char *CubeMapNames[6] =
{
	"skypictures/ixpos.jpg",
	"skypictures/ixneg.jpg",
	"skypictures/iypos.jpg",
	"skypictures/iyneg.jpg",
	"skypictures/izpos.jpg",
	"skypictures/izneg.jpg"
};
*/
/*
GLuint SkyboxTextures[6];
GLubyte *sky_pnt[6];
char *SkyboxTexturesNames[6] =
{
	"skypictures/xpos.jpg",
	"skypictures/xneg.jpg",
	"skypictures/ypos.jpg",
	"skypictures/yneg.jpg",
	"skypictures/zpos.jpg",
	"skypictures/zneg.jpg"
};

/////////////skybox from nutman//////////////
float SkyboxVerts[24][3] =
{
	//Right side of sky box.xpos
	{	 512.0f,	 512.0f,	-512.0f	},	//Top left.
	{	 512.0f,	 512.0f,	 512.0f	},	//Top right.
	{	 512.0f,	-512.0f,	 512.0f	},	//Bottom right.
	{	 512.0f,	-512.0f,	-512.0f	},	//Bottom left.

	//left side of sky box.xneg
	{	-512.0f,	 512.0f,	 512.0f	},	//Top left.
	{	-512.0f,	 512.0f,	-512.0f	},	//Top right.
	{	-512.0f,	-512.0f,	-512.0f	},	//Bottom right.
	{	-512.0f,	-512.0f,	 512.0f	},	//Bottom left.

	//Top side of sky box.ypose
	{	-512.0f,	 512.0f,	 512.0f	},	//Top left.
	{	 512.0f,	 512.0f,	 512.0f	},	//Top right.
	{	 512.0f,	 512.0f,	-512.0f	},	//Bottom right.
	{	-512.0f,	 512.0f,	-512.0f	},	//Bottom left.

	//Bottom side of sky box.yneg
	{	-512.0f,	-512.0f,	-512.0f	},	//Top left.
	{	 512.0f,	-512.0f,	-512.0f	},	//Top right.
	{	 512.0f,	-512.0f,	 512.0f	},	//Bottom right.
	{	-512.0f,	-512.0f,	 512.0f	},	//Bottom left.

	//Front side of sky box.zpos
	{	 512.0f,	 512.0f,	 512.0f	},	//Top left.
	{	-512.0f,	 512.0f,	 512.0f	},	//Top right.
	{	-512.0f,	-512.0f,	 512.0f	},	//Bottom right.
	{	 512.0f,	-512.0f,	 512.0f	},	//Bottom left.

	//Back side of sky box.zneg
	{	-512.0f,	 512.0f,	-512.0f	},	//Top left.
	{	 512.0f,	 512.0f,	-512.0f	},	//Top right.
	{	 512.0f,	-512.0f,	-512.0f	},	//Bottom right.
	{	-512.0f,	-512.0f,	-512.0f	},	//Bottom left.
};
*/
//
// Constructor
//
Alaska::Alaska()
{
  GRAV_CONSTANT = 30.81f; //gravitational constant metric
	lambda = 0.35;//0.29;
	dir = 1;
	a_global=.0008f; // phillips constant
	wind_global[0] = 20;
	wind_global[1] = 30;
	scale_height = 0.25;//scale the wave heights
	deep = 120;
//	wind = .7;
	wind = 0.1;
	factor = 10.0;	//this determines speed of wave
}


///////////////////////////////////////////////////////////////////////////////////////////
//************************ dynamic cube map function **************************************
///////////////////////////////////////////////////////////////////////////////////////////
//These are the rotation angles, to rotate the world at, to create the proper cube map textures.
//Left and right must be inverted, so to do this we spin in Z axis, on first 2!! Hack I know..I'm working on it!
/*float CubeMapRots[6][4] =	// 4, cos it's angle, then XYZ component of that angle.
{
	{ -90.0f,	0.0f,	1.0f,	0.0f	},
	{  90.0f,	0.0f,	1.0f,	0.0f	},

	{ -90.0f,	1.0f,	0.0f,	0.0f	},
	{  90.0f,	1.0f,	0.0f,	0.0f	},

	{ 180.0f,	1.0f,	0.0f,	0.0f	},
	{ 180.0f,	0.0f,	0.0f,	1.0f	}
};

*/
/*void Alaska::CreateDynamicCubeMap(void)
{
	glPushMatrix();
	//Set viewport. and fov.
	glViewport(0, 0, 256, 256);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//MUST USE 90 degree FOV!
	gluPerspective (90, 1.0f, 1, 5000);
//	glRotatef(vert,1,0,0);
 //	glRotatef(horiz,0,1,0);

	glMatrixMode(GL_MODELVIEW);

	for (GLuint i=0; i<6; i++)
	{
		//Clear the depth buffer!
		glClear(GL_DEPTH_BUFFER_BIT);

		//Reset modelview matrix.
		glLoadIdentity();

		//Rotate required amount.ATTENTION!!I dumped the following line to make the plane reflection work
		glRotatef(CubeMapRots[i][0], CubeMapRots[i][1], CubeMapRots[i][2], CubeMapRots[i][3]);

		//Hack to invert Xpos and Xneg renders.
		if (i == 0  ||  i == 1)
		{
			glRotatef(180.0f, 0.0f, 0.0f, 1.0f);

		}
	//	if (i==3)continue;
		//Render!

		RenderSkyBox();

		draw_the_land();

		//Now we need to grab the backbuffer into the texture.
		glEnable(GL_TEXTURE_CUBE_MAP_ARB);

		//Bind the cube map.
		glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, CubeMap);

		glCopyTexSubImage2D(cube_map_directions[i], 0, 0, 0, 0, 0, 256, 256);

		glFlush();
		//Disable cube mapping.
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);

		//Un-comment these to see the cube map textures appear on screen!
		//glutSwapBuffers();
		//Sleep(200);
	}
	glPopMatrix();

	////////ATTENTION  ATTENION!! these resize lines need to be reactivated correctly
	//Delare resize function.
//	void Resize(int width, int height);

	myReshape(window_width,window_height);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Reset back to what it should be!
//	Resize(window_width, window_height);
}*/

///////////////////////////////////////////////////////////////////////////////////////////
//*********************end of dynamic cube map function ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


/*double Alaska::get_current_seconds()
{
	long int the_seconds,the_milliseconds;// the time comes in long ints

	struct _timeb tstruct; //time related
	_ftime(&tstruct);
	the_milliseconds=tstruct.millitm;//get the milliseconds more than above
	the_seconds=tstruct.time; // get the seconds

	return (double)tstruct.time + (double)tstruct.millitm/1000.0;
}*/


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

// this function prints out the frame rate
/*void Alaska::text(int x, int y, char* s)
{
	int lines;
	char* p;
	glGetFloatv(GL_MODELVIEW_MATRIX, hold_modelview_matrix);
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, glutGet(GLUT_WINDOW_WIDTH),
		0, glutGet(GLUT_WINDOW_HEIGHT), -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3ub(0, 0, 0);
	glRasterPos2i(x+1, y-1);
	for (p = s, lines = 0; *p; p++)
	{
		if (*p == '\n')
		{
			lines++;
			glRasterPos2i(x+1, y-1-(lines*18));
		}
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
	}
	glColor3ub(128, 0, 255);

	glRasterPos2i(x, y);
	for (p = s, lines = 0; *p; p++)
	{
		if (*p == '\n') {
			lines++;
			glRasterPos2i(x, y-(lines*18));
		}
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
	}
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
	glLoadMatrixf(hold_modelview_matrix);
//	glClearColor(0,0,0,0);
	glColor3f(1.,1.,1.);
}

//frame count function
void	Alaska::frame_counter()
{
	static int frames=0,framesp=0;
	static double new_time,delta_time,old_time;
	static char s[32];
	frames +=1;
	new_time=get_current_seconds();
	delta_time=new_time-old_time;
	if (delta_time >=1.)
	{
		old_time=new_time;
		framesp=frames;
		frames=0;
	}
	
	text(15,75,s);
}
*/

////////////////////////////////////////////////////////////

/////////////////////// CG initialization.. called from myinit()
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

/*void  Alaska::__MakeNormalizationCubeMap(const int size, const int level)
{
	unsigned char *data = new unsigned char [size * size * 3 ];


	float offset = .5;
	float delta = 1;
	float halfsize = size/2.f;
	Vector3 v;
	unsigned char *ip;

	// positive x image
	{
		ip = data;
		for (int j = 0; j < size; j++)
		{
			for (int i=0; i < size; i++)
			{
				v.z = -(i*delta + offset - halfsize);
				v.y = -(j*delta + offset - halfsize);
				v.x = halfsize;
				my_normalize(v);
				make_signed(v, ip);
				ip += 3;
			}
		}
		glTexImage2D(cube_map_directions[0],
					 level, GL_RGB8, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}

	// negative x image
	{
		ip = data;
		for (int j = 0; j < size; j++)
		{
			for (int i=0; i < size; i++)
			{
				v.z = (i*delta + offset - halfsize);
				v.y = -(j*delta + offset - halfsize);
				v.x = -halfsize;
				my_normalize(v);
				make_signed(v, ip);
				ip += 3;
			}
		}
		glTexImage2D(cube_map_directions[1],
					 level, GL_RGB8, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}

	// positive y image
	{
		ip = data;
		for (int j = 0; j < size; j++)
		{
			for (int i=0; i < size; i++)
			{
				v.x = (i*delta + offset - halfsize);
				v.z = (j*delta + offset - halfsize);
				v.y = halfsize;
				my_normalize(v);
				make_signed(v, ip);
				ip += 3;
			}
		}
		glTexImage2D(cube_map_directions[2],
					 level, GL_RGB8, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	// negative. y image
	{
		ip = data;
		for (int j = 0; j < size; j++)
		{
			for (int i=0; i < size; i++)
			{
				v.x = (i*delta + offset - halfsize);
				v.z = -(j*delta + offset - halfsize);
				v.y = -halfsize;
				my_normalize(v);
				make_signed(v, ip);
				ip += 3;
			}
		}
		glTexImage2D(cube_map_directions[3],
					 level, GL_RGB8, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	// positive z image
	{
		ip = data;
		for (int j = 0; j < size; j++)
		{
			for (int i=0; i < size; i++)
			{
				v.x = (i*delta + offset - halfsize);
				v.y = -(j*delta + offset - halfsize);
				v.z = halfsize;
				my_normalize(v);
				make_signed(v, ip);
				ip += 3;
			}
		}
		glTexImage2D(cube_map_directions[4],
					 level, GL_RGB8, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}

	// negative z image
	{
		ip = data;
		for (int j = 0; j < size; j++)
		{
			for (int i=0; i < size; i++)
			{
				v.x = -(i*delta + offset - halfsize);
				v.y = -(j*delta + offset - halfsize);
				v.z = -halfsize;
				my_normalize(v);
				make_signed(v, ip);
				ip += 3;
			}
		}
		glTexImage2D(cube_map_directions[5],
					 level, GL_RGB8, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	delete data;
}
*/

/*GLuint Alaska::MakeNormalizationCubeMap(void)
{
	GLuint texture;

	glEnable(GL_TEXTURE_CUBE_MAP_ARB);

	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, texture);

	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE_EXT);

	__MakeNormalizationCubeMap(32, 0);

	glDisable(GL_TEXTURE_CUBE_MAP_ARB);

	return texture;*/
//}

/////////////////// end of calculate normal map///////////
////////////////////  set the cg parameters /////////////////////////

//void Alaska::set_cube_cg()
//{
/*	cgGLEnableProfile(vprofile);
	cgGLEnableProfile(fprofile);

	cgGLBindProgram(v);
	cgGLBindProgram(f);

	cgGLEnableClientState(position);
	cgGLSetParameterPointer(position,3,GL_FLOAT,0,vertexPositions);

	cgGLEnableClientState(normal);
	cgGLSetParameterPointer(normal,3,GL_FLOAT,0,normalvector);

	cgGLEnableClientState(normalMapTexCoord);//new

	cgGLEnableClientState(texCoord);//new

	cgGLSetMatrixParameterfr(modelToWorld,watertoworld);//got this from jallen
//	cgGLEnableClientState(texrotmat);//??
//	cgGLSetMatrixParameterfr(texrotmat,texturerotmat);

	cgGLSetParameterPointer(texCoord,2,GL_FLOAT,0,vertexTexCoords);//new
	cgGLSetStateMatrixParameter(modelViewProj,
							CG_GL_MODELVIEW_PROJECTION_MATRIX,
							CG_GL_MATRIX_IDENTITY);
	cgGLSetParameter3fv(eyepositionW,eye_is_at);
	cgGLSetParameter3fv(lightposition,light_is_at);//new
//	cgGLSetParameter1f(reflectivity,mirror);
	cgGLSetParameter1f(fresnelBias,fresnelbias);
	cgGLSetParameter1f(fresnelScale,fresnelscale);
	cgGLSetParameter1f(fresnelPower,fresnelpower);
	cgGLSetParameter1f(etaRatio,etaratio);
	cgGLEnableTextureParameter(normalMap);//new
	cgGLEnableTextureParameter(normalizeCube);//new
	cgGLEnableTextureParameter(normalizeCube2);//new
	cgGLEnableTextureParameter(environmentMap);
	cgGLEnableTextureParameter(bottomMap);
//	cgGLEnableClientState(texCoord);
*/
//}

//void Alaska::unset_cube_cg()
//{
  /*
//	cgGLDisableTextureParameter(baseTexture);
//	cgGLDisableTextureParameter(decalmap);
	cgGLDisableTextureParameter(environmentMap);
//	cgGLDisableClientState(color);
	cgGLDisableClientState(position);
	cgGLDisableClientState(normal);
//	cgGLDisableClientState(texCoord);

//	cgGLDisableClientState(texCoord1);////////////////////////////////////////
//	cgGLDisableClientState(normalMapTexCoord);
	cgGLDisableProfile(fprofile);
	cgGLDisableProfile(vprofile);
	*/
//}

/*void Alaska::set_cg_water()
{
}*/

/*void Alaska::unset_cg_water()
{
}*/

//void Alaska::draw_the_land()
//{
	/*glPushMatrix();
	glRotatef(-90,1,0,0);
	glRotatef(vert,1,0,0);
	glRotatef(horiz,0,0,1.);
	glTranslatef(-75,50,-20);//left-right, depth , up_down
	glBindTexture(GL_TEXTURE_2D, tex[2]);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glEnable(GL_TEXTURE_2D);

	for (int i=1;i<landusize-1;i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for (int j=1;j<landvsize-1;j++)
		{
			glNormal3d(land_normals(i, j, 0),land_normals(i, j, 1),land_normals(i, j, 2));
			glTexCoord2f((float)(i*8)/landusize,(float)(j*8)/landvsize);
			glVertex3f(land(i, j, 0),land(i, j, 1),land(i, j, 2));

			glNormal3d(land_normals(i+1, j, 0),land_normals(i+1, j, 1),land_normals(i+1, j, 2));
			glTexCoord2f((float)(i+1)*8/landusize,(float)(j*8)/landvsize);
			glVertex3f(land(i+1, j, 0),land(i+1, j, 1),land(i+1, j, 2));
		}
		glEnd();
	}

//		forest();
	glPopMatrix();*/
//}

//void Alaska::RenderSkyBox(void)
//{
/*//	Turn z writing off.
//	glDepthMask(0);
	//Need to disable cube mapping, as cube mapping takes priority over 2D textures.
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslated (0.0, -100.0, 0.0);// ?,up/down,?
//	glRotatef(-anglex,0,0,1);//rock the boat

	glRotatef(vert,1,0,0);
	glRotatef(horiz,0,1,0);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);
//	glTranslatef(0,sea[BIG_NX-1][BIG_NY-1][2]*scale_height,0);//for ocean up and down
	for (int i=0; i<6; i++)
	{
		glBindTexture(GL_TEXTURE_2D, SkyboxTextures[i]);
		int v_start = i * 4;

		glBegin(GL_QUADS);
			glTexCoord2f(0.01f, 0.99f);				//Top left of texture.
			glVertex3f(SkyboxVerts[v_start+0][0], SkyboxVerts[v_start+0][1]*1.0f, SkyboxVerts[v_start+0][2]);

			glTexCoord2f(0.99f, 0.99f);				//Top right of texture.
			glVertex3f(SkyboxVerts[v_start+1][0], SkyboxVerts[v_start+1][1]*1.0f, SkyboxVerts[v_start+1][2]);

			glTexCoord2f(0.99f, 0.01f);				//Bottom right of texture.
			glVertex3f(SkyboxVerts[v_start+2][0], SkyboxVerts[v_start+2][1]*1.0f, SkyboxVerts[v_start+2][2]);

			glTexCoord2f(0.01f, 0.01f);				//Bottom left of texture.
			glVertex3f(SkyboxVerts[v_start+3][0], SkyboxVerts[v_start+3][1]*1.0f, SkyboxVerts[v_start+3][2]);
		glEnd();
	}
	glPopMatrix();
//	Turn z writing back on.
//	glDepthMask(1);
}

void	Alaska::show_normals()
{
	for (int L=0;L<2;L++)
	{
		for (int i=0;i<BIG_NX-1;i++)
		{
			glBegin(GL_LINES);
			for (int k=0;k<2;k++)
			{
				for (int j=0;j<BIG_NY;j++)
				{
			//	glTexCoord2f((float)(i)/(float)NX*4,(float)j/(float)NY*4);
			//	glMultiTexCoord2fARB(GL_TEXTURE1_ARB,(float)(i)/(float)NX*4,(float)j/(float)NY*4);
			//	glNormal3d(big_normals[i][j][0],big_normals[i][j][1],big_normals[i][j][2]);

				glVertex3f(sea[i][j][0]+L*MAX_WORLD_X,
					sea[i][j][1]+k*MAX_WORLD_Y,
					sea[i][j][2]*scale_height);

				glVertex3f(sea[i][j][0]+L*MAX_WORLD_X+5*big_normals[i][j][0],
					sea[i][j][1]+k*MAX_WORLD_Y+5*big_normals[i][j][1],
					sea[i][j][2]*scale_height+5*big_normals[i][j][2]);

			//	glTexCoord2f(((float)(i)+1)/(float)NX*4,(float)j/(float)NY*4);
			//	glMultiTexCoord2fARB(GL_TEXTURE1_ARB,((float)(i)+1)/(float)NX*4,(float)j/(float)NY*4);
			//	glNormal3d(big_normals[i+1][j][0],big_normals[i+1][j][1],big_normals[i+1][j][2]);

			//	glVertex3f(sea[i+1][j][0]+L*MAX_WORLD_X,
			//		sea[i+1][j][1]+k*MAX_WORLD_Y,
			//		sea[i+1][j][2]*scale_height);
				}
			}
			glEnd();
		}
	}*/
//}


void	Alaska::RenderOcean()
{

//	static float kounter=0.;
//	kounter=kounter-(1./100.);
	//glPushMatrix();

	//setIdentity(watertoworld);

//	glActiveTextureARB(GL_TEXTURE0_ARB);
//	glEnable(GL_TEXTURE_2D);
//	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);
//	glBindTexture(GL_TEXTURE_2D, tex[1]);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

//	glActiveTextureARB(GL_TEXTURE1_ARB);//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//Enable cube mapping.
	//glEnable(GL_TEXTURE_CUBE_MAP_ARB);
	//bind the texture.
	//glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, CubeMap);
	//Enable cube mapping
	//glEnable(GL_TEXTURE_GEN_S);
	//glEnable(GL_TEXTURE_GEN_T);
	//glEnable(GL_TEXTURE_GEN_R);
	//Enable cube mapping.
	//glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
  //glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
	//glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);

	//glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE_EXT);
	//glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);

	//glRotatef(-90,1,0,0);
	//glRotatef(vert,1,0,0);
	//glRotatef(horiz,0,0,1.);

	//myrotate(watertoworld,-90.,0.0,0.0);
//	myrotate(watertoworld,vert,0.0,0.0);
	//myrotate(watertoworld,0.0,horiz,0.0);

//	glRotatef(anglex,0,1,0);//rock the boat

	//glTranslated (-127.0, -255.0, -50.0);//(-=left,-=forward,-=down) when -90 above
	//mytranslate(watertoworld,-127.0,-255.0,-10.0); // -10
	//glTranslatef(0,deep,0);//starts at 180
	//mytranslate(watertoworld,0.0,deep,0.0);
	//glScalef(6.0f,7.5f,1.4f);

	//glTranslatef(0,0,-sea[BIG_NX-1][BIG_NY-1][2]*scale_height);//ocean up and down
	//glMatrixMode(GL_TEXTURE);//this line allows texture to slide opposite way
	//glPushMatrix();

//Here is the view point corrected. NOTE we're in Texture matrix mode! reverse all previous rotations

	//glRotatef(-horiz, 0.0f, 1.0f, 0.0f);//be sure to rotate in reverse order ++++++old vert
	//glRotatef(-vert,1.,0.,0.);

	//setIdentity(texturerotmat);

//		myrotate(texturerotmat,0.0,horiz,0.0);
//		myrotate(texturerotmat,-vert,0.0,0.0);
	//	myrotate(texturerotmat,-90,0.0,0.0);


//	glGetFloatv(GL_TEXTURE_MATRIX,texturerotmat);

//	glActiveTextureARB(GL_TEXTURE0_ARB);
//	glBindTexture(GL_TEXTURE_2D,tex[1]);//<<<<<<<<<<<<<<<<<<<<

	//if (cgswitch==-1)
//		set_cube_cg();

}

void Alaska::display(void)
{

	double kvector[2],klength,wkt;
	//static double rate=1.0;				//controls rate of movement
	//new_time=get_current_seconds();			// what time is it?
	//time_diff=(start_time-new_time)*rate;	// time passed since program started *rate
	/*for (int i=0;i<NX;i++)			// outer loop starts for x values
	{
		for (int j=0;j<NY;j++)		// inner loop starts for y values
		{
	////////the suspect lines that put the origin in the middle

	//		kvector[0]=2.0*PI*((float)i-.5*NX)/MAX_WORLD_X;
	//		kvector[1]=2.0*PI*((float)j-.5*NY)/MAX_WORLD_Y;

			kvector[0]=2.0*PI*((float)i-.5*NX)/NX;
			kvector[1]=2.0*PI*((float)j-.5*NY)/NY;

			klength=sqrt(kvector[0]*kvector[0]+kvector[1]*kvector[1]);
			wkt = sqrt(klength*GRAV_CONSTANT)*dtime;
		//	wkt = sqrt(G)*klength*time_diff;

	/////////////// original was.. -++ and ++-
//corrected version is based on h* being the conjugate(means multiply imag X (-1))
				c[i][j].real= mH0[i][j].real*cos(wkt)
				+ mH0[i][j].imag*sin(wkt)
				+ mH0[NX - i-1][NY - j-1].real*cos(wkt)
				- mH0[NX - i-1][NY -j-1].imag*sin(wkt);

				c[i][j].imag= mH0[i][j].imag*cos(wkt)
				+ mH0[i][j].real*sin(wkt)
				-mH0[NX - i-1][NY - j-1].imag*cos(wkt)
	 			- mH0[NX - i-1][NY -j-1].real*sin(wkt);


		}
	}*/

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

			kvector[0]=hold_horizontal[i][j][0];
			kvector[1]=hold_horizontal[i][j][1];

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

	//calculate_normal_texture_map();
	//if (panswitch==1)
//		horiz -=.1;


  
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
//	glTranslatef(0,-20,0);

	///////////// rock the boat//////////
	// figure out these arctangents then turn on the rotations in sky and waves
/*	double x_to_my_left=sea[BIG_NX-2][BIG_NY-1][0];
	double y_to_my_left=sea[BIG_NX-2][BIG_NY-1][1];
	double z_to_my_left=sea[BIG_NX-2][BIG_NY-1][2];

	double x_to_my_right=sea[0][0][0]+MAX_WORLD_X;
	double y_to_my_right=sea[0][0][0]+MAX_WORLD_Y;
	double z_to_my_right=sea[0][0][2];

	double x_to_my_back=sea[BIG_NX-1][BIG_NY-2][0];
	double y_to_my_back=sea[BIG_NX-1][BIG_NY-2][1];
	double z_to_my_back=sea[BIG_NX-1][BIG_NY-2][2];

	double x_to_my_front=sea[BIG_NX-1][BIG_NY-2][0];
	double y_to_my_front=sea[BIG_NX-1][BIG_NY-2][1];
	double z_to_my_front=sea[BIG_NX-1][BIG_NY-2][2];

	double xdiff=x_to_my_right-x_to_my_left;
	double zdiff=z_to_my_right-z_to_my_left;
	 anglex=.05*atan(zdiff/xdiff)/TO_RADS;
	 angley=.05*atan(zdiff/ydiff)/TO_RADS;*/

	////////end rock/////////////////////
	//CreateDynamicCubeMap();
//	draw_the_land();
//	forest();
	//RenderSkyBox();
	//RenderOcean();
	//if (frameswitch==-1)frame_counter();

 	//glutSwapBuffers();
	//glutPostRedisplay();
 	
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
	/*GLfloat ambient[]={1.0,1.0,1.0,1.0};
	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 1.0 };

	GLfloat light_zero[]={0.24,0.83,0.64,1.0};
	GLfloat light_one[]={0.0,0.5,1.0,1.0};
	GLfloat light_two[]={0.0,0.0,1.0,1.0};

	GLfloat light_position0[] = { 300.0,	50.0,	-300.0,	 1.0};
	GLfloat light_position1[] = {	-300.0,	500.0,	300.0,	 1.0};
	GLfloat light_position2[] = {	00.0,	-10.0,	10.0,	 1.0};

	glClearColor(0.0,0.0,0.5,1.0);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);*/

	/*glLightfv(GL_LIGHT0, GL_DIFFUSE, light_zero);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
//	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_one);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
	glEnable(GL_LIGHT1);

	glLightfv(GL_LIGHT2, GL_DIFFUSE, light_two);
	glLightfv(GL_LIGHT2, GL_POSITION, light_position2);
//	glEnable(GL_LIGHT2);

	glEnable(GL_LIGHTING);
	glPointSize(8);*/

	///////////////////////////// fog ////////////////////
	//GLfloat fogColor[4]={.5,.5,.5,1.0};
//	GLfloat fogColor[4]={0.9,0.9,.9,1.0};
//	GLfloat fogColor[4]={0.0,0.5,1.0,1.0};
//	float dense=.05;
	/*glFogfv(GL_FOG_COLOR,fogColor);
	glFogf(GL_FOG_DENSITY,fog_dense);
	glFogf(GL_FOG_START,0.0);
	glFogf(GL_FOG_END,1.0);
	glFogi(GL_FOG_MODE,GL_EXP2);
	glEnable(GL_FOG);*/

	/////////////////end fog//////////////////////

	/*glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);*/

/*	GLfloat ambient[]={1.0,1.0,1.0,1.0};
	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[]={1.0,1.0,1.0,1.0};
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat light_position[] = { 0.0, 10.0, 0.0, 1.0};
	GLfloat light_position2[] = {-10.0,-10.0,-10.0, 1.0};
	GLfloat light_position3[] = {00.0,-10.0,10.0, 1.0};
	glClearColor(0.0,0.0,1.0,1.0);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_specular);
		glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
		glLightfv(GL_LIGHT2, GL_POSITION, light_position3);


	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
//	glEnable(GL_LIGHT1);
//	glEnable(GL_LIGHT2);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);*/

	//glEnable(GL_DEPTH_TEST);
	//glShadeModel(GL_SMOOTH);
	//glDrawBuffer(GL_BACK);
	//srand((unsigned)time(NULL)); /* use rand() to create random integers */


	//tex[0]=Loadjpg("textures/foam512.jpg");
	//tex[1]=Loadjpg("textures/bottom.jpg");
	//tex[2]=Loadjpg("textures/wall.jpg");
	//tex[3]=init_normals_texturemap();//this initializes redgreenbluemap
	//cubetexture=MakeNormalizationCubeMap();//this is the normalization cube map's value


	////////////////////////////////////////////////////////////
	//for (int i=0;i<6;i++)
	//{
    //SkyboxTextures[i]=Loadjpg(SkyboxTexturesNames[i]);
		//SkyboxTextures[i]=LoadRepeatjpg(sky_pnt[i],SkyboxTexturesNames[i]);
		//SkyboxTextures[i]=LoadTexture2D(SkyboxTexturesNames[i]);
	//}
	/////////////////////////////////////////////////////////
//	read_land_in();
	//look at maps

	calculate_ho();
//	make_trees();

	//start_time=get_current_seconds();
	//cout<<glGetString(GL_EXTENSIONS);
//	cout<<glGetString(GL_RENDERER);

	//cubetexture=CreateCubemap();
	//CubeMap=CreateCubemapa();
	//CubeMap=CreateCubemap();
	//BottomMap=CreateBottomMap();

	//Cg_init();
}


/*void Alaska::myReshape(int w, int h)
{
	glViewport(0, 0, w, h);

	window_width = w;
	window_height = h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective (70.0, (GLdouble)w/(GLdouble)h, 3.0, 1000.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef (0.0, 0.0, -30.0);
}*/


/*void	Alaska::menu(int item)
{
	switch(item)
	{
	case 'p'://togg;e frame count print
	panswitch = -panswitch;
	glutPostRedisplay();
		break;

	case 'n'://togg;e frame count print
		shownormalsswitch = -shownormalsswitch;

	glutPostRedisplay();
		break;

	case 'c'://togg;e frame count print
		frameswitch = -frameswitch;

	glutPostRedisplay();
		break;

	case ' ':
		cgswitch = -cgswitch;

	glutPostRedisplay();
		break;

	case 'l'://ell

	stripswitch=-stripswitch;

	glutPostRedisplay();
		break;

	case 'r':
//	mirror +=.01;
	mirror =(mirror>=1.) ? 1.0 : (mirror +.01);
//	cout<<mirror<<endl;
	glutPostRedisplay();
		break;

	case 'R':
		mirror =(mirror<=0.0) ? 0.0 : (mirror -.01);
	//	cout<<mirror<<endl;
	glutPostRedisplay();
		break;

	case 'f':
	fog_dense +=.0001;
		glFogf(GL_FOG_DENSITY,fog_dense);
	glutPostRedisplay();
		break;

	case 'F':
		fog_dense -=.0001;
			if (fog_dense<0.0)fog_dense=0.0;
		glFogf(GL_FOG_DENSITY,fog_dense);
	glutPostRedisplay();
		break;

	case 'h':
	scale_height +=.01;
//	cout<<"scale_height="<<scale_height<<endl;
		glutPostRedisplay();
		break;

	case 'H':
	scale_height -=.01;
//	cout<<"scale_height="<<scale_height<<endl;
		glutPostRedisplay();
		break;

	case 's':
	lambda +=.01;
//	cout<<"lambda="<<lambda<<endl;
		glutPostRedisplay();
		break;

	case 'S':
	lambda -=.01;
//	cout<<"lambda="<<lambda<<endl;
		glutPostRedisplay();
		break;

	case 'x':
	 light_is_at[0] +=1.;
//	 cout<<light_is_at[0]<<" "<<light_is_at[1]<<" "<<light_is_at[2]<<endl;
		glutPostRedisplay();
		break;

	case 'X':
	light_is_at[0] -=1;
//		cout<<light_is_at[0]<<" "<<light_is_at[1]<<" "<<light_is_at[2]<<endl;
		glutPostRedisplay();
		break;

	case 'y':
	 light_is_at[1] +=1;
//	 cout<<light_is_at[0]<<" "<<light_is_at[1]<<" "<<light_is_at[2]<<endl;
		glutPostRedisplay();
		break;

	case 'Y':
	light_is_at[1] -=1.;
//	cout<<light_is_at[0]<<" "<<light_is_at[1]<<" "<<light_is_at[2]<<endl;
		glutPostRedisplay();
		break;


	case 'z':
	 light_is_at[2] +=1.;
//	 cout<<light_is_at[0]<<" "<<light_is_at[1]<<" "<<light_is_at[2]<<endl;
		glutPostRedisplay();
		break;

	case 'Z':
	light_is_at[2] -=1.;
//cout<<light_is_at[0]<<" "<<light_is_at[1]<<" "<<light_is_at[2]<<endl;
		glutPostRedisplay();
		break;


	case '+':
		wind +=.01;
		if (wind>1.0)wind =1.0;
		glutPostRedisplay();
		break;

	case '-':
		wind -=.01;
		if (wind<0.0)wind =0.0;
		glutPostRedisplay();
		break;


	case '\033':
		exit(0);
		break;

	default:
		break;
	}
}

void Alaska::special_menu(int item_too)
{
	switch(item_too)
	{
	case GLUT_KEY_RIGHT :
	horiz +=1;

//	glutPostRedisplay();//I killed these since I am using an idle function that calls it
		break;

	case GLUT_KEY_LEFT :
	horiz -=1;

//	glutPostRedisplay();
		break;

	case GLUT_KEY_UP :
	vert +=1;
//	glutPostRedisplay();
		break;

	case GLUT_KEY_DOWN :
	vert -=1;
//	glutPostRedisplay();
		break;

	default:
		break;

	}
}

void key(unsigned char key,int x,int y)
{
	app.menu((int) key);
}

void special_key(int s_key,int x,int y)
{
	app.special_menu( s_key);
}

void mouse(int b, int s, int x, int y)
{
	if (s==GLUT_DOWN && b==GLUT_LEFT_BUTTON)
	{
		glutPostRedisplay();
		return;
	}

	if (s==GLUT_UP && b==GLUT_LEFT_BUTTON)
	{
		glutPostRedisplay();
	}
}

void drag(int x, int y)
{
	glutPostRedisplay();
}
*/
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

//void display(void)
//{
//	app.display();
//}

//void idle(void)
//{
//	app.idle();
//}

//void reshape(int w, int h)
//{
	//app.myReshape(w, h);
//}

/*int main(int argc, char** argv)
{
   	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(640, 512);

	glutCreateWindow(argv[0]);
	glutFullScreen();
	glutSetCursor(GLUT_CURSOR_NONE);
	int err = glewInit();
	if (err != GLEW_OK)
	{
		cout<<"glewInit failed"<<endl;
		exit(1);
	}
	app.myinit();
	glutReshapeFunc(reshape);
	glutKeyboardFunc(key);
	glutSpecialFunc(special_key);
//	glutMouseFunc(mouse);
//	glutMotionFunc(drag);
//	glutIdleFunc(idle);
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}

*/
