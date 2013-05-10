/**
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Lesser General Public License (LGPL)
*
* VSXu Engine is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU Lesser General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


#include "_configuration.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include "vsx_math_3d.h"
#include "vsx_glsl.h"

GLfloat blobMat[16];

GLfloat blobVec0[4];
GLfloat blobVec1[4];


// this is the method used in starlight aurora vsx, quite handy actually
void beginBlobs(vsx_module_engine_info* engine)
{
	glEnable(GL_TEXTURE_2D);
	GLfloat tmpMat[16];


  glGetFloatv(GL_MODELVIEW_MATRIX, blobMat);


  glMatrixMode(GL_PROJECTION);

	glPushMatrix();


  glGetFloatv(GL_PROJECTION_MATRIX, tmpMat);

	tmpMat[3] = 0;
	tmpMat[7] = 0;
	tmpMat[11] = 0;
	tmpMat[12] = 0;
	tmpMat[13] = 0;
	tmpMat[14] = 0;
	tmpMat[15] = 1;

	v_norm(tmpMat);
	v_norm(tmpMat + 4);
	v_norm(tmpMat + 8);

  glLoadIdentity();

  glMultMatrixf(tmpMat);

	blobMat[3] = 0;
	blobMat[7] = 0;
	blobMat[11] = 0;
	blobMat[12] = 0;
	blobMat[13] = 0;
	blobMat[14] = 0;
	blobMat[15] = 1;

	v_norm(blobMat);
	v_norm(blobMat + 4);
	v_norm(blobMat + 8);

  glMultMatrixf(blobMat);

  glGetFloatv(GL_PROJECTION_MATRIX, blobMat);

	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);

	//inv_mat(blobMat, blobMatInv);

	GLfloat upLeft[] = {-0.5f, 0.5f, 0.0f, 1.0f};
	GLfloat upRight[] = {0.5f, 0.5f, 0.0f, 1.0f};

	mat_vec_mult(blobMat, upLeft, blobVec0);
	mat_vec_mult(blobMat, upRight, blobVec1);
}

inline void drawBlob(GLfloat x, GLfloat y, GLfloat z, GLfloat size)
{
	GLfloat tmpVec0[] = {blobVec0[0] * size, blobVec0[1] * size, blobVec0[2] * size};
	GLfloat tmpVec1[] = {blobVec1[0] * size, blobVec1[1] * size, blobVec1[2] * size};
		glTexCoord2f(1, 1);
		glVertex3f(x + tmpVec0[0], y + tmpVec0[1],  z + tmpVec0[2]);
		glTexCoord2f(1, 0);
		glVertex3f(x - tmpVec1[0], y - tmpVec1[1],  z - tmpVec1[2]);
		glTexCoord2f(0, 0);
		glVertex3f(x - tmpVec0[0], y - tmpVec0[1],  z - tmpVec0[2]);
		glTexCoord2f(0, 1);
		glVertex3f(x + tmpVec1[0], y + tmpVec1[1],  z + tmpVec1[2]);
}

inline void drawBlob_c(GLfloat x, GLfloat y, GLfloat z, GLfloat size, GLfloat dx, GLfloat dy, GLfloat dz)
{
	GLfloat tmpVec0[] = {blobVec0[0] * size, blobVec0[1] * size, blobVec0[2] * size};
	GLfloat tmpVec1[] = {blobVec1[0] * size, blobVec1[1] * size, blobVec1[2] * size};
		glTexCoord2f(1, 1);
		glVertex3f(dx, dy, dz);
		glTexCoord2f(1, 0);
		glVertex3f(x - tmpVec1[0], y - tmpVec1[1],  z - tmpVec1[2]);
		glTexCoord2f(0, 0);
		glVertex3f(x - tmpVec0[0], y - tmpVec0[1],  z - tmpVec0[2]);
		glTexCoord2f(0, 1);
		glVertex3f(dx + tmpVec1[0], dy+tmpVec1[1], dz+tmpVec1[2]);
}

#include "render_particlesystem.h"

#include "render_particlesystem_c.h"

#include "render_particlesystem_sparks.h"

#include "render_particlesystem_ext.h"

//******************************************************************************
//*** F A C T O R Y ************************************************************
//******************************************************************************

#ifndef _WIN32
#define __declspec(a)
#endif

extern "C" {
__declspec(dllexport) vsx_module* create_new_module(unsigned long module, void* args);
__declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
__declspec(dllexport) unsigned long get_num_modules();
}


vsx_module* create_new_module(unsigned long module, void* args)
{
  VSX_UNUSED(args);
  switch (module) {
    case 0: return (vsx_module*)(new module_render_particlesystem);
    case 1: return (vsx_module*)(new module_render_particlesystem_c);
    case 2: return (vsx_module*)(new module_render_particlesystem_sparks);
    case 3: return (vsx_module*)(new module_render_particlesystem_ext);
  }
  return 0;
}


void destroy_module(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (module_render_particlesystem*)m; break;
    case 1: delete (module_render_particlesystem_c*)m; break;
    case 2: delete (module_render_particlesystem_sparks*)m; break;
    case 3: delete (module_render_particlesystem_ext*)m; break;
  }
}

unsigned long get_num_modules() {
  glewInit();
  // we have only one module. it's id is 0
  return 4;
}

