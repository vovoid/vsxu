/*===============================================================================================
 geometry.exe main.cpp
 Copyright (c), Firelight Technologies Pty, Ltd 2005-2008.

 Example to show occlusion
===============================================================================================*/
#include <math.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>

#include "../../api/inc/fmod.hpp"
#include "../../api/inc/fmod_errors.h"

#include "gl/glut.h"

void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}

int INTERFACE_UPDATETIME = 17; // milliseconds

// window size
int width = 500;
int height = 500;

// mouse control
bool doRotate = false;
int xMouse = 0;
int yMouse = 0;

// listener orientation
float xRotation = 0.0f;
float yRotation = -90.0f;

// listerer position
float xListenerPos = 30.0f;
float yListenerPos = 1.3f;
float zListenerPos = 0.4f;

// keyboard control
bool moveForward = false;
bool moveBackward = false;
bool moveLeft = false;
bool moveRight = false;
bool moveUp = false;
bool moveDown = false;

const float PI = 3.14159265f;

float accumulatedTime = 0.0f;

GLuint texture;

// sounds placement
struct Object
{
	float xPos;
	float yPos;
	float zPos;
	float intensity;
	int sound;
	FMOD::Channel *channel;
};

const int NUM_OBJECTS = 7;
Object objects[NUM_OBJECTS] =
{
	{  -11.0f,    1.0f,    0.0f,    1.0f,    0,    0},
	{   12.0f,    2.0f,    0.0f,    1.0f,    1,    0},
	{   45.0f,    1.0f,    0.0f,    1.0f,    3,    0},
	{  -30.0f,    1.0f,   21.0f,    1.0f,    2,    0},
	{  -30.0f,    1.0f,  -21.0f,    1.0f,    3,    0},
	{   12.0f,    1.0f,  -27.0f,    1.0f,    0,    0},
	{    4.0f,    1.0f,   16.0f,    1.0f,    0,    0},
};

// geometry structers for loading and drawing
struct Polygon
{
	int numVertices;
	int indicesOffset;
	float directOcclusion;
	float reverbOcclusion;
	FMOD_VECTOR normal;
};

struct Mesh
{
	int numVertices;
	FMOD_VECTOR *vertices;
	float (*texcoords)[2];
	int numPolygons;
	Polygon* polygons;
	int numIndices;
	int *indices;
	FMOD::Geometry *geometry;
};

class GlutCloseClass
{
  public:
    GlutCloseClass() {};
   ~GlutCloseClass();
};


GlutCloseClass gCloseObject;

Mesh walls;
Mesh rotatingMesh;
Mesh doorList[4];

// fmod sounds structures
FMOD::System    *fmodSystem = 0;
FMOD::Sound     *sounds[4] = { 0, 0, 0, 0};
FMOD::Geometry	*geometry = 0;

void initGeometry(const char* szFileName, Mesh& mesh)
{
	FMOD_RESULT result;

	FILE* file = fopen(szFileName, "rb");
	if (file)
	{
		// read vertices
		fread(&mesh.numVertices, sizeof (mesh.numVertices), 1, file);	
		mesh.vertices = new FMOD_VECTOR[mesh.numVertices];
		mesh.texcoords = new float[mesh.numVertices][2];
		fread(mesh.vertices, sizeof (float) * 3, mesh.numVertices, file);	
		fread(mesh.texcoords, sizeof (float) * 2, mesh.numVertices, file);	


		fread(&mesh.numIndices, sizeof (mesh.numIndices), 1, file);
		mesh.indices = new int[mesh.numIndices];
		fread(mesh.indices, sizeof (int), mesh.numIndices, file);	

		
		fread(&mesh.numPolygons, sizeof (mesh.numPolygons), 1, file);
		mesh.polygons = new Polygon[mesh.numPolygons];

		// read polygons
		for (int poly = 0; poly < mesh.numPolygons; poly++)
		{
			Polygon* polygon = &mesh.polygons[poly];


			fread(&polygon->numVertices, sizeof (polygon->numVertices), 1, file);
			fread(&polygon->indicesOffset, sizeof (polygon->indicesOffset), 1, file);
			fread(&polygon->directOcclusion, sizeof (polygon->directOcclusion), 1, file);
			fread(&polygon->reverbOcclusion, sizeof (polygon->reverbOcclusion), 1, file);

			int* indices = &mesh.indices[polygon->indicesOffset];			

			// calculate polygon normal
			float xN = 0.0f;
			float yN = 0.0f;
			float zN = 0.0f;
			// todo: return an error if a polygon has less then 3 vertices.
			for (int vertex = 0; vertex < polygon->numVertices - 2; vertex++)
			{
				float xA = mesh.vertices[indices[vertex + 1]].x -mesh.vertices[indices[0]].x;
				float yA = mesh.vertices[indices[vertex + 1]].y -mesh.vertices[indices[0]].y;
				float zA = mesh.vertices[indices[vertex + 1]].z -mesh.vertices[indices[0]].z;
				float xB = mesh.vertices[indices[vertex + 2]].x -mesh.vertices[indices[0]].x;
				float yB = mesh.vertices[indices[vertex + 2]].y -mesh.vertices[indices[0]].y;
				float zB = mesh.vertices[indices[vertex + 2]].z -mesh.vertices[indices[0]].z;
				// cross product
				xN += yA * zB - zA * yB;
				yN += zA * xB - xA * zB;
				zN += xA * yB - yA * xB;
			}	
			float fMagnidued = (float)sqrt(xN * xN + yN * yN + zN * zN);
			if (fMagnidued > 0.0f) // a tollerance here might be called for
			{
				xN /= fMagnidued;
				yN /= fMagnidued;
				zN /= fMagnidued;
			}
			polygon->normal.x = xN;
			polygon->normal.y = yN;
			polygon->normal.z = zN;
		}
		fclose(file);
	}	

	result = fmodSystem->createGeometry(mesh.numPolygons, mesh.numIndices, &mesh.geometry);
	ERRCHECK(result);

	for (int poly = 0; poly < mesh.numPolygons; poly++)
	{
		Polygon* polygon = &mesh.polygons[poly];
		FMOD_VECTOR vertices[16];
        int i;

		for (i = 0; i < polygon->numVertices; i++)
        {
			vertices[i] = mesh.vertices[mesh.indices[polygon->indicesOffset + i]];
        }

        int polygonIndex = 0;
		    
		result = mesh.geometry->addPolygon(polygon->directOcclusion, polygon->reverbOcclusion, false, // single sided
                                           polygon->numVertices, 
			                               vertices, 
			                              &polygonIndex);
		ERRCHECK(result);
	}
}


void freeGeometry(Mesh& mesh)
{
    mesh.geometry->release();

    delete [] mesh.vertices;
	delete [] mesh.texcoords;
	delete [] mesh.polygons;
	delete [] mesh.indices;
}


void drawGeometry(Mesh& mesh)
{
	FMOD_RESULT result;

	FMOD_VECTOR pos;
	result = mesh.geometry->getPosition(&pos);
	ERRCHECK(result);

	glPushMatrix();
	// create matrix and set gl transformation for geometry
	glTranslatef(pos.x, pos.y, pos.z);	
	FMOD_VECTOR forward;
	FMOD_VECTOR up;
	result = mesh.geometry->getRotation(&forward, &up);
	ERRCHECK(result);
	float matrix[16] = 
	{
		up.y * forward.z - up.z * forward.y,		up.x,		forward.x,		0.0f,
		up.z * forward.x - up.x * forward.z,		up.y,		forward.y,		0.0f,
		up.x * forward.y - up.y * forward.x,		up.z,		forward.z,		0.0f,
		0.0f,										0.0f,		0.0f,			1.0f,
	};
	glMultMatrixf(matrix);

	// draw all polygons in object
	glEnable(GL_LIGHTING);	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	for (int poly = 0; poly < mesh.numPolygons; poly++)
	{
		Polygon* polygon = &mesh.polygons[poly];
		if (polygon->directOcclusion == 0.0f)
        {
			continue; // don't draw because it is an open door way
        }

		glBegin(GL_TRIANGLE_FAN);
		glNormal3fv(&polygon->normal.x);

		for (int i = 0; i < polygon->numVertices; i++)
		{
			int index = mesh.indices[polygon->indicesOffset + i];
			glTexCoord2f(mesh.texcoords[index][0], mesh.texcoords[index][1]);
			glVertex3fv(&mesh.vertices[index].x);
		}
		glEnd();
	}
	glPopMatrix();
}

void initObjects()
{	
	FMOD_RESULT result;
    int i;

	for (i = 0; i < NUM_OBJECTS; i++)
	{
		// play object sounds
		FMOD_VECTOR pos = { objects[i].xPos, objects[i].yPos, objects[i].zPos };
		FMOD_VECTOR vel = {  0.0f,  0.0f, 0.0f };

		result = fmodSystem->playSound(FMOD_CHANNEL_FREE, sounds[objects[i].sound], true, &objects[i].channel);
		ERRCHECK(result);
		result = objects[i].channel->set3DAttributes(&pos, &vel);
		ERRCHECK(result);
		result = objects[i].channel->set3DSpread(40.0f);
		ERRCHECK(result);
		result = objects[i].channel->setPaused(false);
		ERRCHECK(result);
	}
}

void updateObjectSoundPos(Object* object)
{
	FMOD_RESULT result;
	FMOD_VECTOR pos = { object->xPos, object->yPos, object->zPos };
	FMOD_VECTOR oldPos;
	object->channel->get3DAttributes(&oldPos, 0);

	FMOD_VECTOR vel;
	vel.x = (pos.x - oldPos.x) *  (1000.0f / (float)INTERFACE_UPDATETIME);
	vel.y = (pos.y - oldPos.y) *  (1000.0f / (float)INTERFACE_UPDATETIME);
	vel.z = (pos.z - oldPos.z) *  (1000.0f / (float)INTERFACE_UPDATETIME);
	result = object->channel->set3DAttributes(&pos, &vel);
	ERRCHECK(result);
}

void mouseFunc(int button, int state, int x, int y)
{
	switch (button) 
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			doRotate = true;
			xMouse = x;
			yMouse = y;
		}
		else
		if (state == GLUT_UP)
		{
			doRotate = false;
		}
		break;

	default:
		break;
	}
}

void motionFunc(int x, int y)
{
	int dx = x - xMouse;
	int dy = y - yMouse;

	// view rotation about y-axis
	yRotation += (float)dx * 0.5f;
	if (yRotation > 180.0f)
		yRotation -= 360.0f;
	else
	if (yRotation < -180.0f)
		yRotation += 360.0f;

	// view rotation about x-axis
	const float xExtent = 88.0f;
	xRotation += (float)dy * 0.5f;
	if (xRotation > xExtent)
		xRotation = xExtent;
	else 
	if (xRotation < -xExtent)
		xRotation = -xExtent;

	xMouse = x;
	yMouse = y;
}

void doGeometryMovement()
{
	FMOD_RESULT result;

	// example of moving individual polygon vertices
	int xGeometryWarpPos = -30.0f;
	int zGeometryWarpPos = -21.0f;
	int dx = xListenerPos - xGeometryWarpPos;
	int dz = zListenerPos - zGeometryWarpPos;
	if (dx * dx + dz * dz < 30.0f * 30.0f)
	{
		if (sin(accumulatedTime * 1.0f) > 0.0f)
		{
			static FMOD_VECTOR lastOffset = { 0.0f, 0.0f, 0.0f };
			FMOD_VECTOR offset = { sin(accumulatedTime * 2.0f), 0.0f, cos(accumulatedTime * 2.0f) };
			for (int poly = 0; poly < walls.numPolygons; poly++)
			{
				Polygon* polygon = &walls.polygons[poly];
				for (int i = 0; i < polygon->numVertices; i++)
				{
					FMOD_VECTOR& vertex = walls.vertices[walls.indices[polygon->indicesOffset + i]];

					dx = vertex.x - xGeometryWarpPos;
					dz = vertex.z - zGeometryWarpPos;
					if (dx * dx + dz * dz > 90.0f)
						continue;
					vertex.x -= lastOffset.x;
					vertex.y -= lastOffset.y;
					vertex.z -= lastOffset.z;

					vertex.x += offset.x;
					vertex.y += offset.y;
					vertex.z += offset.z;
					result = walls.geometry->setPolygonVertex(poly, i, &vertex);
					ERRCHECK(result);
				}	
			}
			lastOffset = offset;
		}
	}

	// example of rotation and a geometry object
	FMOD_VECTOR up = { 0.0f, 1.0f, 0.0f };
	FMOD_VECTOR forward = { (float)sin(accumulatedTime * 0.5f), 0.0f, (float)cos(accumulatedTime * 0.5f) };
	result = rotatingMesh.geometry->setRotation(&forward, &up);
	ERRCHECK(result);
	FMOD_VECTOR pos;
	pos.x = 12.0f;
	pos.y = (float)sin(accumulatedTime) * 0.4f + 0.1f;
	pos.z = 0.0f;
	result = rotatingMesh.geometry->setPosition(&pos);
	ERRCHECK(result);

	// example of moving doors
	// door 1
	pos.x = 3.25f;
	pos.y = ((float)sin(accumulatedTime)) * 2.0f + 1.0f;
	if (pos.y < 0.0f)
		pos.y = 0;
	if (pos.y > 2.0f)
		pos.y = 2.0f;
	pos.z = 11.5f;
	result = doorList[0].geometry->setPosition(&pos);
	ERRCHECK(result);

	// door 2
	pos.x = 0.75f;
	pos.y = ((float)sin(accumulatedTime)) * 2.0f + 1.0f;
	if (pos.y < 0.0f)
		pos.y = 0;
	if (pos.y > 2.0f)
		pos.y = 2.0f;
	pos.z = 14.75f;
	result = doorList[1].geometry->setPosition(&pos);
	ERRCHECK(result);

	// door 3
	pos.x = 8.25f;
	pos.y = ((float)sin(accumulatedTime)) * 2.0f + 1.0f;
	if (pos.y < 0.0f)
		pos.y = 0;
	if (pos.y > 2.0f)
		pos.y = 2.0f;
	pos.z = 14.75f;
	result = doorList[2].geometry->setPosition(&pos);
	ERRCHECK(result);

	// door 4
	pos.x = 33.0f;
	pos.y = ((float)sin(accumulatedTime)) * 2.0f + 1.0f;
	if (pos.y < 0.0f)
		pos.y = 0;
	if (pos.y > 2.0f)
		pos.y = 2.0f;
	pos.z = -0.75f;
	result = doorList[3].geometry->setPosition(&pos);
	ERRCHECK(result);
}

void doSoundMovement()
{	
	objects[0].zPos = 10.0f * sin(accumulatedTime);
	updateObjectSoundPos(&objects[0]);
	objects[5].zPos = -22 + 8.0f * sin(accumulatedTime);
	updateObjectSoundPos(&objects[5]);
}

void doListenerMovement()
{
	// Update user movement
	const float MOVEMENT_SPEED = 0.1f;
	float forward = 0.0f;

	if (moveForward)
		forward += MOVEMENT_SPEED;
	if (moveBackward)
		forward -= MOVEMENT_SPEED;

	float right = 0.0f;
	if (moveLeft)
		right -= MOVEMENT_SPEED;
	if (moveRight)
		right += MOVEMENT_SPEED;	

	float up = 0.0f;
    if (moveUp)
        up += MOVEMENT_SPEED;
    if (moveDown)
        up -= MOVEMENT_SPEED;
	
	float xRight = (float)cos(yRotation * (PI / 180.0f));
	float yRight = 0.0f;
	float zRight = (float)sin(yRotation * (PI / 180.0f));

	xListenerPos += xRight * right;
	yListenerPos += yRight * right;
	zListenerPos += zRight * right;

	float xForward = (float)sin(yRotation * (PI / 180.0f)) * cos(xRotation  * (PI / 180.0f));
	float yForward = -(float)sin(xRotation  * (PI / 180.0f));
	float zForward = -(float)cos(yRotation * (PI / 180.0f)) * cos(xRotation  * (PI / 180.0f));

	xListenerPos += xForward * forward;
	yListenerPos += yForward * forward;
	zListenerPos += zForward * forward;

	yListenerPos += up;

	if (yListenerPos < 1.0f)
    {
		yListenerPos = 1.0f;
    }

	// cross product
	float xUp = yRight * zForward - zRight * yForward;
	float yUp = zRight * xForward - xRight * zForward;
	float zUp = xRight * yForward - yRight * xForward;

	// Update listener
	{
		FMOD_VECTOR listenerVector;
		listenerVector.x = xListenerPos;
		listenerVector.y = yListenerPos;
		listenerVector.z = zListenerPos;

		static FMOD_VECTOR lastpos = { 0.0f, 0.0f, 0.0f };
		static bool bFirst = true;
		FMOD_VECTOR forward;
		FMOD_VECTOR up;
		FMOD_VECTOR vel;

		forward.x = xForward;
		forward.y = yForward;
		forward.z = zForward;
		up.x = xUp;
		up.y = yUp;
		up.z = zUp;

		// ********* NOTE ******* READ NEXT COMMENT!!!!!
		// vel = how far we moved last FRAME (m/f), then time compensate it to SECONDS (m/s).
		vel.x = (listenerVector.x - lastpos.x) * (1000.0f / (float)INTERFACE_UPDATETIME);
		vel.y = (listenerVector.y - lastpos.y) * (1000.0f / (float)INTERFACE_UPDATETIME);
		vel.z = (listenerVector.z - lastpos.z) * (1000.0f / (float)INTERFACE_UPDATETIME);
		if (bFirst)
		{
			bFirst = false;
			vel.x = 0;
			vel.y = 0;
			vel.z = 0;
		}

		static FMOD_VECTOR lastVel = { 0.0f, 0.0f, 0.0f };

		// store pos for next time
		lastpos = listenerVector;
		lastVel = vel;

		FMOD_RESULT result = fmodSystem->set3DListenerAttributes(0, &listenerVector, &vel, &forward, &up);
		ERRCHECK(result);
	}
}

void timerFunc(int nValue)
{
	FMOD_RESULT result;

	doGeometryMovement();

	doSoundMovement();
	doListenerMovement();
	
    result = fmodSystem->update();
	ERRCHECK(result);

	accumulatedTime += (float)INTERFACE_UPDATETIME / 1000.0f;

	glutPostRedisplay();
	glutTimerFunc(INTERFACE_UPDATETIME, timerFunc, 0);

    if (0)
    {
        float dsp, stream, update, total;
        int chansplaying;

        fmodSystem->getCPUUsage(&dsp, &stream, &update, &total);
        fmodSystem->getChannelsPlaying(&chansplaying);

        printf("chans %d : cpu : dsp = %.02f stream = %.02f update = %.02f total = %.02f\n", chansplaying, dsp, stream, update, total);
    }
}

void keyboardFunc(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
		moveForward = true;
		break;
	case 's':
		moveBackward = true;
		break;
	case 'a':
		moveLeft = true;
		break;
	case 'd':
		moveRight = true;
		break;
	case ' ':
		moveUp = true;
		break;
	case 'c':
		moveDown = true;
		break;
	}
}

void keyboardUpFunc(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
		moveForward = false;
		break;
	case 's':
		moveBackward = false;
		break;
	case 'a':
		moveLeft = false;
		break;
	case 'd':
		moveRight = false;
		break;
	case ' ':
		moveUp = false;
		break;
	case 'c':
		moveDown = false;
		break;
	}
}

void outputText(int x, int y, char *string)
{
	int len, i;
	
	glRasterPos2f(x, y);
	len = (int) strlen(string);
	for (i = 0; i < len; i++) 
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
	}
}

void display(void)
{	
	// update view	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(
		60.0,								// fov
		(float)width / (float)height,		// aspect
		0.1,								// near
		500.0								// far
		);


	glRotatef(xRotation, 1.0f, 0.0f, 0.0f);
	glRotatef(yRotation, 0.0f, 1.0f, 0.0f);
	glTranslatef(-xListenerPos, -yListenerPos, -zListenerPos);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.4, 0.6f, 1.0f, 0.0f);

	glEnable(GL_TEXTURE_2D);
	glBindTexture( GL_TEXTURE_2D, texture );

	// draw geometry
	drawGeometry(walls);
	drawGeometry(rotatingMesh);
	drawGeometry(doorList[0]);
	drawGeometry(doorList[1]);
	drawGeometry(doorList[2]);
	drawGeometry(doorList[3]);


	
	glDisable(GL_TEXTURE_2D);


	// draw sound objects
	int object;
	for (object = 0; object < NUM_OBJECTS; object++)
	{
		float directOcclusion = 1.0f;
		float reverbOcclusion = 1.0f;

		// set colour baced on direct occlusion
		objects[object].channel->get3DOcclusion(&directOcclusion, &reverbOcclusion);
		float intensity = 1.0f - directOcclusion;
				
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glPushMatrix();
		glTranslatef(objects[object].xPos, objects[object].yPos, objects[object].zPos);

		glPushAttrib(GL_LIGHTING_BIT);
			
		intensity *= 0.75f;
		float color[4] = { intensity, intensity, 0.0f, 0.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
		intensity *= 0.5f;
		float ambient[4] = { intensity, intensity, 0.0f, 0.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
		
		glRotatef(accumulatedTime * 200.0f, 0.0f, 1.0f, 0.0f);
		glutSolidTorus(0.15f, 0.6f, 8, 16);
		glPopAttrib();
		glPopMatrix();
	}

	// finish
	glutSwapBuffers();
}

void reshapeFunc(int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

GLuint loadTexture(const char * filename)
{
    GLuint texture;
    int width;
	int height;
    unsigned char *data;
    FILE *file;

    // open texture data
    file = fopen( filename, "rb" );
    if ( file == NULL ) 
		return 0;

    width = 128;
    height = 128;
    data = (unsigned char*)malloc(width * height * 3);

    fread(data, width * height * 3, 1, file );
    fclose(file);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    gluBuild2DMipmaps(
		GL_TEXTURE_2D, 
		3, 
		width, 
		height,
        GL_RGB, 
		GL_UNSIGNED_BYTE,
		data);

    free(data);

    return texture;
}


void init(void)
{
    FMOD_RESULT      result;
    bool             listenerflag = true;
    FMOD_VECTOR      listenerpos  = { 0.0f, 0.0f, 0.0f };
    unsigned int     version;
    FMOD_SPEAKERMODE speakermode;
    FMOD_CAPS        caps;
    char             name[256];

    printf("==================================================================\n");
    printf("Geometry example.  Copyright (c) Firelight Technologies 2004-2008.\n");
    printf("==================================================================\n\n");

    /*
        Create a System object and initialize.
    */
    result = FMOD::System_Create(&fmodSystem);
    ERRCHECK(result);
    
    result = fmodSystem->getVersion(&version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
        exit(-1);
    }

    result = fmodSystem->getDriverCaps(0, &caps, 0, 0, &speakermode);
    ERRCHECK(result);

    result = fmodSystem->setSpeakerMode(speakermode);       /* Set the user selected speaker mode. */
    ERRCHECK(result);

    if (caps & FMOD_CAPS_HARDWARE_EMULATED)             /* The user has the 'Acceleration' slider set to off!  This is really bad for latency!. */
    {                                                   /* You might want to warn the user about this. */
        result = fmodSystem->setDSPBufferSize(1024, 10);
        ERRCHECK(result);
    }

    result = fmodSystem->getDriverInfo(0, name, 256, 0);
    ERRCHECK(result);

    if (strstr(name, "SigmaTel"))   /* Sigmatel sound devices crackle for some reason if the format is PCM 16bit.  PCM floating point output seems to solve it. */
    {
        result = fmodSystem->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0,0, FMOD_DSP_RESAMPLER_LINEAR);
        ERRCHECK(result);
    }

    result = fmodSystem->init(100, FMOD_INIT_3D_RIGHTHANDED | FMOD_INIT_SOFTWARE_OCCLUSION | FMOD_INIT_SOFTWARE_HRTF, 0);
    if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)         /* Ok, the speaker mode selected isn't supported by this soundcard.  Switch it back to stereo... */
    {
        result = fmodSystem->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
        ERRCHECK(result);
                
        result = fmodSystem->init(100, FMOD_INIT_NORMAL, 0);/* ... and re-init. */
        ERRCHECK(result);
    };
  
    // Load sounds
    result = fmodSystem->createSound("../media/drumloop.wav", FMOD_SOFTWARE | FMOD_3D, 0, &sounds[0]);
    ERRCHECK(result);
    result = sounds[0]->set3DMinMaxDistance(1.0f, 10000.0f);
    ERRCHECK(result);
    result = sounds[0]->setMode(FMOD_LOOP_NORMAL);
    ERRCHECK(result);

    result = fmodSystem->createSound("../media/wave.mp3", FMOD_SOFTWARE | FMOD_CREATECOMPRESSEDSAMPLE |  FMOD_3D, 0, &sounds[1]);
    ERRCHECK(result);
    result = sounds[1]->set3DMinMaxDistance(1.0f, 10000.0f);
    ERRCHECK(result);
    result = sounds[1]->setMode(FMOD_LOOP_NORMAL);
    ERRCHECK(result);

    result = fmodSystem->createSound("../media/jaguar.wav", FMOD_SOFTWARE | FMOD_3D, 0, &sounds[2]);
    ERRCHECK(result);
    result = sounds[2]->set3DMinMaxDistance(1.0f, 10000.0f);
    ERRCHECK(result);
    result = sounds[2]->setMode(FMOD_LOOP_NORMAL);
    ERRCHECK(result);

    result = fmodSystem->createSound("../media/swish.wav", FMOD_SOFTWARE | FMOD_3D, 0, &sounds[3]);
    ERRCHECK(result);
    result = sounds[3]->set3DMinMaxDistance(1.0f, 10000.0f);
    ERRCHECK(result);
    result = sounds[3]->setMode(FMOD_LOOP_NORMAL);
    ERRCHECK(result);

		
	initObjects();

	result = fmodSystem->setGeometrySettings(200.0f);
    ERRCHECK(result);


	// load objects
	initGeometry("../media/walls.bin", walls);
	initGeometry("../media/center.bin", rotatingMesh);
	initGeometry("../media/door.bin", doorList[0]);
	initGeometry("../media/door.bin", doorList[1]);
	initGeometry("../media/door.bin", doorList[2]);
	initGeometry("../media/door.bin", doorList[3]);

	// place doors in desired orientatins
	FMOD_VECTOR up      = { 0.0f, 1.0f, 0.0f };
	FMOD_VECTOR forward = { 1.0f, 0.0f, 0.0f };
	result = doorList[1].geometry->setRotation(&forward, &up);
    ERRCHECK(result);
	result = doorList[2].geometry->setRotation(&forward, &up);
    ERRCHECK(result);
	result = doorList[3].geometry->setRotation(&forward, &up);
    ERRCHECK(result);

	texture = loadTexture("../media/texture.img");		

	// setup lighting
	GLfloat lightDiffuse[] = {1.0, 1.0, 1.0, 1.0}; 
	GLfloat lightPosition[] = {300.0, 1000.0, 400.0, 0.0};
	GLfloat lightAmbiant[] = {1.25, 1.25, 1.25, 1.0};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbiant);	
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0f);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);	
	
	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

GlutCloseClass::~GlutCloseClass()
{
    glDeleteTextures( 1, &texture ); 

	freeGeometry(walls);
	freeGeometry(rotatingMesh);
	freeGeometry(doorList[0]);
	freeGeometry(doorList[1]);
	freeGeometry(doorList[2]);
	freeGeometry(doorList[3]);

    sounds[0]->release();
    sounds[1]->release();
    sounds[2]->release();
    sounds[3]->release();

    fmodSystem->release();
}


int	main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutCreateWindow("FMOD Geometry example.");
	glutDisplayFunc(display);	
	glutReshapeFunc(reshapeFunc);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(motionFunc);
	glutKeyboardFunc(keyboardFunc);
	glutKeyboardUpFunc(keyboardUpFunc);
	glutTimerFunc(INTERFACE_UPDATETIME, timerFunc, 0);

	init();

	glutMainLoop();
	
	return 0;
}

