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
#include <module/vsx_module.h>
#include "vsx_glsl.h"

GLfloat blobMat[16];

GLfloat blobVec0[4];
GLfloat blobVec1[4];


// this is the method used in starlight aurora vsx, quite handy actually

void beginBlobs(vsx_gl_state* gl_state)
{
  GLfloat tmpMat[16];

  gl_state->matrix_get_v( VSX_GL_MODELVIEW_MATRIX, blobMat );

  gl_state->matrix_mode( VSX_GL_PROJECTION_MATRIX );

  gl_state->matrix_push();

  gl_state->matrix_get_v( VSX_GL_PROJECTION_MATRIX, tmpMat );

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

  gl_state->matrix_load_identity();

  gl_state->matrix_mult_f(tmpMat);

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

  gl_state->matrix_mult_f(blobMat);


  gl_state->matrix_get_v( VSX_GL_PROJECTION_MATRIX, blobMat );

  gl_state->matrix_pop();

  gl_state->matrix_mode (VSX_GL_MODELVIEW_MATRIX );

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

#include "module_particlesystem_render.h"
#include "module_particlesystem_render_c.h"
#include "module_particlesystem_render_ext.h"
#include "module_particlesystem_render_sparks.h"


//******************************************************************************
//*** F A C T O R Y ************************************************************
//******************************************************************************

#ifndef _WIN32
#define __declspec(a)
#endif

extern "C" {
__declspec(dllexport) vsx_module* create_new_module(unsigned long module, void* args);
__declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
__declspec(dllexport) unsigned long get_num_modules(vsx_module_engine_environment* environment);
}

#ifndef MOD_CM
#define MOD_CM vsx_module_particlesystem_render_cm
#define MOD_DM vsx_module_particlesystem_render_dm
#define MOD_NM vsx_module_particlesystem_render_nm
#endif

vsx_module* MOD_CM(unsigned long module, void* args)
{
  VSX_UNUSED(args);
  switch (module) {
    case 0: return (vsx_module*)(new module_particlesystem_render);
    case 1: return (vsx_module*)(new module_particlesystem_render_c);
    case 2: return (vsx_module*)(new module_particlesystem_render_sparks);
    case 3: return (vsx_module*)(new module_particlesystem_render_ext);
  }
  return 0;
}


void MOD_DM(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (module_particlesystem_render*)m; break;
    case 1: delete (module_particlesystem_render_c*)m; break;
    case 2: delete (module_particlesystem_render_sparks*)m; break;
    case 3: delete (module_particlesystem_render_ext*)m; break;
  }
}

unsigned long MOD_NM(vsx_module_engine_environment* environment) {
  VSX_UNUSED(environment);
  // we have only one module. it's id is 0
  return 4;
}

