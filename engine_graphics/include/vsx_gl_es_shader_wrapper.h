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


#if defined(VSXU_OPENGL_ES)
#ifndef VSX_GL_ES_SHADER_WRAPPER_H
#define VSX_GL_ES_SHADER_WRAPPER_H

#include <string/vsx_string.h>
#include "vsx_gl_global.h"

#define VSX_GL_ES_MATRIX_STACK_DEPTH 10


// TODO: remove this after all "hidden" shader ops are moved into the cpp file
//void vsx_load_shader(GLuint *pShader, vsx_string<>shader_source, GLint iShaderType);


void vsx_es_set_default_arrays(GLvoid* vertices, GLvoid* colors);

// init the shader wrapper (run once)
void vsx_es_shader_wrapper_init_shaders();

// needs to be run right before rendering to upload matrices to shader
void vsx_es_begin();

void vsx_es_end();

#define GL_PROJECTION 0
#define GL_MODELVIEW 1
#define GL_TEXTURE_MATRIX 2


// OpenGL mimics
void glTranslatef(float x, float y, float z);
void glScalef(float x, float y, float z);
void glRotatef(float angle, float x, float y, float z);


void glMatrixMode(int new_value);
void glLoadIdentity();

void glPushMatrix();
void glPopMatrix();

void glMultMatrixf(GLfloat matrix[16]);

void gluPerspective(double fovy, double aspect, double zNear, double zFar);

#endif
#endif
