#if defined(VSXU_OPENGL_ES)
#ifndef VSX_GL_ES_SHADER_WRAPPER_H
#define VSX_GL_ES_SHADER_WRAPPER_H

#include "vsx_string.h"
#include "vsx_gl_global.h"


// TODO: remove this after all "hidden" shader ops are moved into the cpp file
//void vsx_load_shader(GLuint *pShader, vsx_string shader_source, GLint iShaderType);


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


#endif
#endif