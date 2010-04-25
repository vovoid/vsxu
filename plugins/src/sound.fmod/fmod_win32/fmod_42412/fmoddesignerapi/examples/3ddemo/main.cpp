/*===============================================================================================
 3D Demo Example
 Copyright (c), Firelight Technologies Pty, Ltd 2005-2009.

 Example to show occlusion
===============================================================================================*/

#include "../../../api/inc/fmod.hpp"
#include "../../../api/inc/fmod_errors.h"
#include "../../api/inc/fmod_event.hpp"
#include "../../api/inc/fmod_event_net.h"

#include <windows.h>

#include "gl/glut.h"
#include "soil/include/SOIL.h"

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stack>
#include <string>

#include <crtdbg.h>

namespace FMOD
{

//#define SHOW_GUI_DEBUG_TEXT
#define GL_CLAMP_TO_EDGE 0x812F

void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}

int INTERFACE_UPDATETIME = 17; // milliseconds

bool fullscreen = false;

// window size
int width = 500;
int height = 500;

// mouse control
bool doRotate = false;
int xMouse = 0;
int yMouse = 0;

// listener orientation
float xRotation = 0.0f;
float yRotation = 90.0f;

// listerer position
float xListenerPos = -4.0f;
float yListenerPos = 1.3f;
float zListenerPos = 0.0f;

// keyboard control
bool moveForward    = false;
bool moveBackward   = false;
bool moveLeft       = false;
bool moveRight      = false;
bool moveUp         = false;
bool moveDown       = false;
bool moveFast       = false;
bool ambientVolUp   = false;
bool ambientVolDown = false;
bool masterVolUp    = false;
bool masterVolDown  = false;

const float PI = 3.14159265f;

float accumulatedTime = 0.0f;

// textures
GLuint texture;
GLuint skyboxTexture[6];

// sounds placement
struct Object
{
	float xPos;
	float yPos;
	float zPos;
	float intensity;
	int sound;
    FMOD::Event *event;
};

const int NUM_OBJECTS = 7;
Object objects[NUM_OBJECTS] =
{
	{  -11.0f,    1.0f,    0.0f,    1.0f,    0,    0 },
	{   12.0f,    2.0f,    0.0f,    1.0f,    1,    0 },
	{   45.0f,    1.0f,    0.0f,    1.0f,    3,    0 },
	{  -30.0f,    1.0f,   21.0f,    1.0f,    2,    0 },
	{  -30.0f,    1.0f,  -21.0f,    1.0f,    3,    0 },
	{   12.0f,    1.0f,  -27.0f,    1.0f,    0,    0 },
	{    4.0f,    1.0f,   16.0f,    1.0f,    0,    0 },
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
FMOD::EventSystem     *fmodEventSystem    = 0;
FMOD::EventProject    *fmodEventProject   = 0;
FMOD::EventGroup      *fmodEventGroup     = 0;
FMOD::EventParameter  *fmodEventParameter = 0;
FMOD::System          *fmodSystem         = 0;
FMOD::Geometry	      *geometry           = 0;
FMOD::DSP             *global_lowpass     = 0;

float   ambientVolume   = 0.2f;
float   masterVolume;

/*
    Global stack of strings to render
*/
#ifdef SHOW_GUI_DEBUG_TEXT
std::stack<std::string> debugText;
std::stack<std::string> statusText;
#endif

GLenum  rendermode = GL_FILL;

bool showdebug = false;
bool showhelp = false;

void outputText(int x, int y, std::string text)
{
	int i;
	const char *txt = text.c_str();

	glRasterPos2f(x, y);
	for (i = 0; i < (int)text.length(); i++) 
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, txt[i]);
	}
}

#ifdef SHOW_GUI_DEBUG_TEXT
void renderText(int x, int y, std::stack<std::string> *text)
{
    glColor3f(1.0f, 1.0f, 1.0f);
    for (int count = 0; count < text->size(); count++)
    {
        outputText(x, y, text->top());
        text->pop();

        y -= 17;
    }
}
#endif

void renderUiText()
{
    /*
        Render help text
    */
    if (showhelp)
    {
        int x = 10;
        int y = height - 20;

        glColor3f(1.0f, 1.0f, 1.0f);
        outputText(x, y,     "F1   - Toggle help");
        outputText(x, y-=18, "F2   - Toggle fullscreen");
        outputText(x, y-=18, "F3   - Toggle wireframe rendering");
        outputText(x, y-=18, "F11  - Toggle debug info");
        outputText(x, y-=18, "--");
        outputText(x, y-=18, "w     - Move forward");
        outputText(x, y-=18, "s     - Move backward");
        outputText(x, y-=18, "a     - Move left");
        outputText(x, y-=18, "d     - Move right");
        outputText(x, y-=18, "space - Move up");
        outputText(x, y-=18, "c     - Move down");
        outputText(x, y-=18, "Mouse (hold left button) - look direction");
        outputText(x, y-=18, "--");
  //      outputText(x, y-=18, "V/v   - Master volume up/down");
        outputText(x, y-=18, "Z/z   - Ambient sound volume up/down");
    }
    else
    {
        glColor3f(1.0f, 1.0f, 1.0f);
        outputText(10, height - 20, "F1 - Help");
    }

    /*
        Render debug text
    */
#ifdef SHOW_GUI_DEBUG_TEXT
    if (showdebug)
    {
        renderText(width - (width/2), height - 20, &debugText);
    }
    else
    {
        /*
            Otherwise just pop everything off the stack
        */
        for (int count = 0; count < debugText.size(); count++)
        {
            debugText.pop();
        }
    }

    /*
        Render status text
    */
    renderText(10, 20, &statusText);
#endif
}

void initGeometry(const char* szFileName, Mesh& mesh, bool alter = false)
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

    /*
        Tell FMOD about the geometry
    */
	for (int poly = 0; poly < mesh.numPolygons; poly++)
	{
		Polygon* polygon = &mesh.polygons[poly];
		FMOD_VECTOR vertices[16];
		for (int i = 0; i < polygon->numVertices; i++)
			vertices[i] = mesh.vertices[mesh.indices[polygon->indicesOffset + i]];
		int polygonIndex = 0;
        
        if (alter && polygon->directOcclusion == 0.85f)
        {
     //       polygon->directOcclusion = 0.95f;
        }

		result = mesh.geometry->addPolygon(
			polygon->directOcclusion, 
			polygon->reverbOcclusion, 
			false, // single sided
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


void inWater()
{
    FMOD_RESULT result;
    FMOD_REVERB_PROPERTIES reverbprops;
    static bool inwater = false;

    if (xListenerPos > -14.75f && xListenerPos < -7.6f
        && zListenerPos > -10.85f && zListenerPos < -3.75f
        && yListenerPos < 5.0f)
    {
        /*
            Use opengl fog to make it look like we are in water
        */
        if (!inwater)
        {
    	    glEnable(GL_FOG);

            result = fmodEventSystem->getReverbPreset("waterroom", &reverbprops);
            ERRCHECK(result);
            result = fmodEventSystem->setReverbProperties(&reverbprops);
            ERRCHECK(result);
            result = global_lowpass->setBypass(false);
            ERRCHECK(result);

            inwater = true;
        }
    }
    else
    {
        /*
            Disable fog (water)
        */
        if (inwater)
        {
            glDisable(GL_FOG);

            result = fmodEventSystem->getReverbPreset("scarycave", &reverbprops);
            ERRCHECK(result);
            result = fmodEventSystem->setReverbProperties(&reverbprops);
            ERRCHECK(result);
            if (global_lowpass)
            {
                result = global_lowpass->setBypass(true);
                ERRCHECK(result);
            }

            inwater = false;
        }
    }
}


void drawSkyBox()
{
    glPushMatrix();
        glTranslatef(xListenerPos, 0.0f, yListenerPos);
        glDisable(GL_LIGHTING);
        /*
            Walls
        */
        glBindTexture(GL_TEXTURE_2D, skyboxTexture[0]);
        glBegin(GL_QUADS);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(-150.0f, -150.0f, -150.0f);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(-150.0f, 150.0f, -150.0f);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(150.0f, 150.0f, -150.0f);
            glTexCoord2f(0.0f, 1.0f);  glVertex3f(150.0f, -150.0f, -150.0f);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, skyboxTexture[1]);
        glBegin(GL_QUADS);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(150.0f, -150.0f, -150.0f);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(150.0f, 150.0f,-150.0f);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(150.0f, 150.0f, 150.0f);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(150.0f, -150.0f, 150.0f);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, skyboxTexture[2]);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(-150.0f, -150.0f, 150.0f);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-150.0f, 150.0f, 150.0f);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(150.0f, 150.0f, 150.0f);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(150.0f, -150.0f, 150.0f);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, skyboxTexture[3]);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(-150.0f, -150.0f, -150.0f);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-150.0f, 150.0f, -150.0f);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(-150.0f, 150.0f, 150.0f);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(-150.0f, -150.0f, 150.0f);
        glEnd();

        /*
            Top
        */
        glBindTexture(GL_TEXTURE_2D, skyboxTexture[4]);
        glBegin(GL_QUADS);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(-150.0f, 150.0f, -150.0f);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(150.0f, 150.0f, -150.0f);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(150.0f, 150.0f, 150.0f);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-150.0f, 150.0f, 150.0f);
        glEnd();

        /*
            Bottom
        */
        glBindTexture(GL_TEXTURE_2D, skyboxTexture[5]);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(-150.0f, -150.0f, -150.0f);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(150.0f, -150.0f, -150.0f);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(150.0f, -150.0f, 150.0f);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(-150.0f, -150.0f, 150.0f);
        glEnd();

        glEnable(GL_LIGHTING);
    glPopMatrix();
}

void drawWaterRoom()
{
    glBlendFunc( GL_SRC_ALPHA, GL_ONE );
    glEnable(GL_BLEND);

    glPushMatrix();
        glColor4f(0.0f,0.1f,0.8f,1.0f);
        glDisable(GL_LIGHTING);

        glBegin(GL_QUADS);
            glVertex3f(-14.72f, 4.0f, -10.85f);
            glVertex3f(-7.68f, 4.0f, -10.85f);
            glVertex3f(-7.68f, 4.0f, -3.85f);
            glVertex3f(-14.72f, 4.0f, -3.85f);
        glEnd();

        // Door
        glBegin(GL_QUADS);
            glVertex3f(-7.6f, 0.0f, -6.3f);
            glVertex3f(-7.6f, 0.0f, -8.35f);
            glVertex3f(-7.6f, 2.0f, -8.35f);
            glVertex3f(-7.6f, 2.0f, -6.3f);
        glEnd();

        // Door
        glBegin(GL_QUADS);
            glVertex3f(-12.25, 0.0f, -3.75f);
            glVertex3f(-10.17, 0.0f, -3.75f);
            glVertex3f(-10.17, 2.0f, -3.75f);
            glVertex3f(-12.25, 2.0f, -3.75f);
        glEnd();

        glEnable(GL_LIGHTING);
    glPopMatrix();

    glDisable(GL_BLEND);
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
	glPolygonMode(GL_FRONT_AND_BACK, rendermode);
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

    result = fmodEventGroup->getEvent("3DSoundEmit", FMOD_EVENT_DEFAULT, &objects[0].event);
    ERRCHECK(result);

    result = fmodEventGroup->getEvent("3DSoundEmit", FMOD_EVENT_DEFAULT, &objects[1].event);
    ERRCHECK(result);

    result = fmodEventGroup->getEvent("3DSoundEmit", FMOD_EVENT_DEFAULT, &objects[2].event);
    ERRCHECK(result);

    result = fmodEventGroup->getEvent("3DSoundEmit", FMOD_EVENT_DEFAULT, &objects[3].event);
    ERRCHECK(result);

    result = fmodEventGroup->getEvent("3DSoundEmit", FMOD_EVENT_DEFAULT, &objects[4].event);
    ERRCHECK(result);

    result = fmodEventGroup->getEvent("3DSoundEmit", FMOD_EVENT_DEFAULT, &objects[5].event);
    ERRCHECK(result);

    result = fmodEventGroup->getEvent("3DSoundEmit", FMOD_EVENT_DEFAULT, &objects[6].event);
    ERRCHECK(result);

	for (int i=0; i < NUM_OBJECTS; i++)
	{
		FMOD_VECTOR pos = { objects[i].xPos, objects[i].yPos, objects[i].zPos };
		FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };

        if (objects[i].event)
        {
		    result = objects[i].event->set3DAttributes(&pos, &vel);
		    ERRCHECK(result);
            result = objects[i].event->start();
            ERRCHECK(result);
        }
	}
}


void updateObjectSoundPos(Object* object)
{
    FMOD_RESULT result;

    if (object->event)
    {
	    FMOD_VECTOR pos = { object->xPos, object->yPos, object->zPos };
	    FMOD_VECTOR oldPos;
     	object->event->get3DAttributes(&oldPos, 0);

	    FMOD_VECTOR vel;
	    vel.x = (pos.x - oldPos.x) *  (1000.0f / (float)INTERFACE_UPDATETIME);
	    vel.y = (pos.y - oldPos.y) *  (1000.0f / (float)INTERFACE_UPDATETIME);
	    vel.z = (pos.z - oldPos.z) *  (1000.0f / (float)INTERFACE_UPDATETIME);
    	result = object->event->set3DAttributes(&pos, &vel);
//    	ERRCHECK(result);
    }
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
	int test = 0;

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
    bool        doorMoving[4] = {true, true, true ,true};

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
	drawGeometry(rotatingMesh);


	// example of moving doors
	// door 1
	pos.x = 3.25f;
	pos.y = ((float)sin(accumulatedTime)) * 2.0f + 1.0f;
	if (pos.y < 0.0f)
    {
		pos.y = 0;
        doorMoving[0] = false;
    }
	if (pos.y > 2.0f)
    {
		pos.y = 2.0f;
        doorMoving[0] = false;
    }
	pos.z = 11.5f;


	result = doorList[0].geometry->setPosition(&pos);
	ERRCHECK(result);


	// door 2
	pos.x = 0.75f;
	pos.y = ((float)sin(accumulatedTime)) * 2.0f + 1.0f;
	if (pos.y < 0.0f)
    {
		pos.y = 0;
        doorMoving[1] = false;
    }
	if (pos.y > 2.0f)
    {
		pos.y = 2.0f;
        doorMoving[1] = false;
    }
	pos.z = 14.75f;

    
	result = doorList[1].geometry->setPosition(&pos);
	ERRCHECK(result);


	// door 3
	pos.x = 8.25f;
	pos.y = ((float)sin(accumulatedTime)) * 2.0f + 1.0f;
	if (pos.y < 0.0f)
    {
		pos.y = 0;
        doorMoving[2] = false;
    }
	if (pos.y > 2.0f)
    {
		pos.y = 2.0f;
        doorMoving[2] = false;
    }
	pos.z = 14.75f;


	result = doorList[2].geometry->setPosition(&pos);
	ERRCHECK(result);


	// door 4
	pos.x = 33.0f;
	pos.y = ((float)sin(accumulatedTime)) * 2.0f + 1.0f;
	if (pos.y < 0.0f)
    {
		pos.y = 0;
        doorMoving[3] = false;
    }
	if (pos.y > 2.0f)
    {
		pos.y = 2.0f;
        doorMoving[3] = false;
    }
	pos.z = -0.75f;

    
	result = doorList[3].geometry->setPosition(&pos);
	ERRCHECK(result);
}

void doSoundMovement()
{	
	objects[0].zPos = 10.0f * sin(accumulatedTime*0.3);
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
    {
		forward += (MOVEMENT_SPEED * (moveFast ? 2.0f : 1.0f));
    }
	if (moveBackward)
    {
		forward -= (MOVEMENT_SPEED * (moveFast ? 2.0f : 1.0f));
    }

	float right = 0.0f;
	if (moveLeft)
    {
		right -= (MOVEMENT_SPEED * (moveFast ? 2.0f : 1.0f));
    }
	if (moveRight)
    {
		right += (MOVEMENT_SPEED * (moveFast ? 2.0f : 1.0f));	
    }
	
	float up = 0.0f;
	if (moveUp)
    {
		up += (MOVEMENT_SPEED * (moveFast ? 2.0f : 1.0f));
    }
	if (moveDown)
    {
		up -= (MOVEMENT_SPEED * (moveFast ? 2.0f : 1.0f));
    }

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

		if (lastVel.x != 0.0f || lastVel.y != 0.0f || lastVel.z != 0.0f)
		{
			if (vel.x == 0.0f && vel.y == 0.0f && vel.z == 0.0f)
			{
				int test = 0;
			}
		}
		lastVel = vel;

		FMOD_RESULT result = fmodSystem->set3DListenerAttributes(0, &listenerVector, &vel, &forward, &up);
		ERRCHECK(result);
	}
}

void doUpdateVolume()
{
    if (ambientVolUp)
    {
        char volumestring[64];

        ambientVolume += 0.025;
        if (ambientVolume > 1.0f)
        {
            ambientVolume = 1.0f;
        }

        sprintf(volumestring, "Ambient Volume: %0.3f", ambientVolume);
        
        #ifdef SHOW_GUI_DEBUG_TEXT
        statusText.push(volumestring);
        #endif
    }
    else if (ambientVolDown)
    {
        char volumestring[64];

        ambientVolume -= 0.025;
        if (ambientVolume < 0.0f)
        {
            ambientVolume = 0.0f;
        }

        sprintf(volumestring,"Ambient Volume: %0.3f", ambientVolume);
        
        #ifdef SHOW_GUI_DEBUG_TEXT
        statusText.push(volumestring);
        #endif
    }
}

void timerFunc(int nValue)
{
    static bool firsttime = true;
	FMOD_RESULT result;

	doGeometryMovement();

	doSoundMovement();
	doListenerMovement();
    doUpdateVolume();
	
    result = FMOD::NetEventSystem_Update();
	ERRCHECK(result);
    result = fmodEventSystem->update();
	ERRCHECK(result);

	accumulatedTime += (float)INTERFACE_UPDATETIME / 1000.0f;

	glutPostRedisplay();
	glutTimerFunc(INTERFACE_UPDATETIME, timerFunc, 0);

#if 0
    if (firsttime)
    {
        SetWindowPos(
          GetForegroundWindow(),    // handle to window
          HWND_TOPMOST,             // placement-order handle
          0,                        // horizontal position
          0,                        // vertical position
          width,                    // width
          height,                   // height
          SWP_NOMOVE
        );
        firsttime = false;
    }
#endif
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
    case 'z' :
        ambientVolDown = true;
        break;
    case 'Z' :
        ambientVolUp = true;
        break;
    case 'v':
        masterVolDown = true;
        break;
    case 'V':
        masterVolUp = true;
        break;

    case 'p':
        {
            float value;

            fmodEventParameter->getValue(&value);

            fmodEventParameter->setValue(value - 20.0f);
        }
        break;

    case 'P':
        {
            float value;

            fmodEventParameter->getValue(&value);

            fmodEventParameter->setValue(value + 20.0f);
        }
        break;

    case 'f':
        moveFast = true;
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
    case 'z' :
        ambientVolDown = false;
        break;
    case 'Z' :
        ambientVolUp = false;
        break;
    case 'v':
        masterVolDown = false;
        break;
    case 'V':
        masterVolUp = false;
        break;
    case 'f':
        moveFast = false;
        break;
    case 27:
        exit(1);
        break;
	}
}

void specialKeyFunc(int key, int x, int y)
{
  /*  switch (key)
    {

    }*/

    key = key;
}

void specialKeyUpFunc(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_F1:
        showhelp = !showhelp;
        break;
    case GLUT_KEY_F2:
        if(fullscreen)
        {
            glutPositionWindow(20,40);
            glutReshapeWindow(500,500);
            fullscreen = false;
        }
        else
        {       
            glutFullScreen();
            fullscreen = true;
        }
        break;
    case GLUT_KEY_F3:
        rendermode = (rendermode == GL_LINE ? GL_FILL : GL_LINE);
        break;
    case GLUT_KEY_F11:
        showdebug = !showdebug;
        break;
    case GLUT_KEY_F8:
        rotatingMesh.geometry->setActive(false);
        walls.geometry->setActive(false);
        break;
    case GLUT_KEY_F9:
        rotatingMesh.geometry->setActive(true);
        walls.geometry->setActive(true);
        break;
    }
}

void display(void)
{
    // Show listener position
    {
        char text[64];

        sprintf(text, "Listener Pos: (%.2f, %.2f, %.2f)", xListenerPos, yListenerPos, zListenerPos);
        #ifdef SHOW_GUI_DEBUG_TEXT
        debugText.push(std::string(text));
        #endif
    }
    // Show cpu usage position
    {
        char text[64];
        float x,y,z,w;

        fmodSystem->getCPUUsage(&x,&y,&z,&w);
        sprintf(text, "CPU Usage : (%.2f, %.2f, %.2f %.2f)", x, y, z, w);
        #ifdef SHOW_GUI_DEBUG_TEXT
        debugText.push(std::string(text));
        #endif
    }

    /*
        3D RENDERING
    */
	
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


    // draw skybox
    drawSkyBox();

	glDisable(GL_TEXTURE_2D);

	// draw sound objects
	int object;
	for (object = 0; object < NUM_OBJECTS; object++)
	{
        char txt[256];


		float directOcclusion = 1.0f;
		float reverbOcclusion = 1.0f;

		// set colour baced on direct occlusion
//		objects[object].channel->getOcclusion(&directOcclusion, &reverbOcclusion);
		float intensity = 1.0f;// - directOcclusion;
				
		glPolygonMode(GL_FRONT_AND_BACK, rendermode);
		glPushMatrix();
		glTranslatef(objects[object].xPos, objects[object].yPos, objects[object].zPos);

        sprintf(txt, "Sound object (%d): %.2f, %.2f, %.2f", object, objects[object].xPos, objects[object].yPos, objects[object].zPos);
        #ifdef SHOW_GUI_DEBUG_TEXT
        debugText.push(std::string(txt));
        #endif

		glPushAttrib(GL_LIGHTING_BIT);
			
		intensity *= 0.75f;
		float color[4] = { intensity, intensity, 0.0f, 0.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
		intensity *= 0.5f;
		float ambient[4] = { intensity, intensity, 0.0f, 0.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
		

		glRotatef(accumulatedTime * 200.0f, 0.0f, 1.0f, 0.0f);
        {
            FMOD_VECTOR soundorientation;
            float rad = (accumulatedTime * 200.0f);
            
            rad *= 3.14159f;
            rad /= 180.0f;

            soundorientation.x = sinf(rad);
            soundorientation.y = 0;
            soundorientation.z = cosf(rad);

            objects[object].event->set3DAttributes(0, 0, &soundorientation);
        }

		glutSolidTorus(0.15f, 0.6f, 8, 16);
		glPopAttrib();
		glPopMatrix();
	}

    /*
        Draw blue transparent blue quads to entry to water room
    */
    drawWaterRoom();

    /*
        Do water effects if we are in the water room
    */  
    inWater();


    /*
        2D RENDERING
    */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0f, (GLsizei)width, 0.0f, (GLsizei)height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_LIGHTING);
	glDisable(GL_NORMALIZE);
    glDisable(GL_DEPTH_TEST);

    /*
        Render text
    */
    renderUiText();


    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
	glPolygonMode(GL_FRONT_AND_BACK, rendermode);

	// finish
	glutSwapBuffers();
}

void reshapeFunc(int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

GLuint loadTexturePNG(const char *filename)
{
    GLuint texture = SOIL_load_OGL_texture(filename, 0, 0, 0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return texture;
}

GLuint loadTexture(const char *filename)
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
    FMOD_SPEAKERMODE speakermode;

    printf("==================================================================\n");
    printf("3D example.  Copyright (c) Firelight Technologies 2004-2009.\n");
    printf("==================================================================\n\n");

    result = FMOD::EventSystem_Create(&fmodEventSystem);
    ERRCHECK(result);
    result = FMOD::NetEventSystem_Init(fmodEventSystem);
    ERRCHECK(result);
    result = fmodEventSystem->getSystemObject(&fmodSystem);
    ERRCHECK(result);
    result = fmodSystem->getDriverCaps(0,0,0,0,&speakermode);
    ERRCHECK(result);
    result = fmodSystem->setSpeakerMode(speakermode);
    ERRCHECK(result);
    result = fmodEventSystem->init(64, FMOD_INIT_3D_RIGHTHANDED | FMOD_INIT_SOFTWARE_OCCLUSION | FMOD_INIT_SOFTWARE_HRTF, 0, FMOD_EVENT_INIT_NORMAL);
    ERRCHECK(result);
    result = fmodEventSystem->setMediaPath("..\\media\\");  
    ERRCHECK(result);
    result = fmodEventSystem->load("examples.fev", 0, &fmodEventProject);
    ERRCHECK(result);
    result = fmodEventProject->getGroup("FeatureDemonstration/3D Events", true, &fmodEventGroup);
    ERRCHECK(result);

    /*
        Create a programmer created lowpass filter to apply to everything.
    */
    result = fmodSystem->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &global_lowpass);
    ERRCHECK(result);

    result = global_lowpass->setParameter(FMOD_DSP_LOWPASS_CUTOFF, 1000);
    ERRCHECK(result);

    result = global_lowpass->setBypass(true);   // turn it off to start with.
    ERRCHECK(result);

    result = fmodSystem->addDSP(global_lowpass, 0);
    ERRCHECK(result);


	initObjects();

	result = fmodSystem->setGeometrySettings(200.0f);
    ERRCHECK(result);

    printf("Loading geometry...");

	// load objects
	initGeometry("../media/walls.bin", walls, true);
	initGeometry("../media/center.bin", rotatingMesh);
	initGeometry("../media/door.bin", doorList[0]);
	initGeometry("../media/door.bin", doorList[1]);
	initGeometry("../media/door.bin", doorList[2]);
	initGeometry("../media/door.bin", doorList[3]);

    printf("done.\n");

	// place doors in desired orientatins
	FMOD_VECTOR up      = { 0.0f, 1.0f, 0.0f };
	FMOD_VECTOR forward = { 1.0f, 0.0f, 0.0f };
	result = doorList[1].geometry->setRotation(&forward, &up);
    ERRCHECK(result);
	result = doorList[2].geometry->setRotation(&forward, &up);
    ERRCHECK(result);
	result = doorList[3].geometry->setRotation(&forward, &up);
    ERRCHECK(result);

    /*
        Load textures
    */
    printf("Loading textures...\n");

	texture = loadTexture("../media/texture.img");
    skyboxTexture[0] = loadTexturePNG("../media/skybox/bluesky/front.png");
    skyboxTexture[1] = loadTexturePNG("../media/skybox/bluesky/right.png");
    skyboxTexture[2] = loadTexturePNG("../media/skybox/bluesky/back.png");
    skyboxTexture[3] = loadTexturePNG("../media/skybox/bluesky/left.png");
    skyboxTexture[4] = loadTexturePNG("../media/skybox/bluesky/top.png");
    skyboxTexture[5] = loadTexturePNG("../media/skybox/bluesky/bottom.png");

    printf("done.\n");

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

    // setup fog(water)
    GLfloat	fogColor[4] = {0.0f,0.1f,0.9f,1.0f};

	glFogi(GL_FOG_MODE, GL_EXP);	    // Fog Mode
	glFogfv(GL_FOG_COLOR, fogColor);    // Set Fog Color
	glFogf(GL_FOG_DENSITY, 0.15f);	    // How Dense Will The Fog Be
	glHint(GL_FOG_HINT, GL_DONT_CARE);	// Fog Hint Value
	glFogf(GL_FOG_START, 0.0f);			// Fog Start Depth
	glFogf(GL_FOG_END, 1.0f);			// Fog End Depth
	
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

    fmodEventSystem->release();
    FMOD::NetEventSystem_Shutdown();

    _CrtDumpMemoryLeaks();
}
}

using namespace FMOD;

int	main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutCreateWindow("FMOD Ex 3D Demo");
	glutDisplayFunc(display);	
	glutReshapeFunc(reshapeFunc);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(motionFunc);
	glutKeyboardFunc(keyboardFunc);
	glutKeyboardUpFunc(keyboardUpFunc);
    glutSpecialFunc(specialKeyFunc);
    glutSpecialUpFunc(specialKeyUpFunc);

	glutTimerFunc(INTERFACE_UPDATETIME, timerFunc, 0);

	init();

    Sleep(1000);

	glutMainLoop();
    
	return 0;
}

