#if defined(VSXU_OPENGL_ES)
#ifndef VSX_GL_ES_SHADER_WRAPPER_H
#define VSX_GL_ES_SHADER_WRAPPER_H



void glTranslatef(float x, float y, float z);
void glScalef(float x, float y, float z);
void glRotatef(float angle, float x, float y, float z);

#define GL_PROJECTION 0
#define GL_MODELVIEW 1
#define GL_TEXTURE_MATRIX 2

void glMatrixMode(int new_value);
void glLoadIdentity();

void glPushMatrix();
void glPopMatrix();

void glMultMatrixf(GLfloat matrix[16]);

#endif
#endif