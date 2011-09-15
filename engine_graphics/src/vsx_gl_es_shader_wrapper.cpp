/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Vovoid Media Technologies Copyright (C) 2003-2011
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#if defined(VSXU_OPENGL_ES)

#include "vsx_math_3d.h"
#include <stdio.h>
#include <stdlib.h>
#include "vsx_gl_es_shader_wrapper.h"
#define DEBUG

vsx_matrix core_matrix[3];


vsx_matrix matrix_stack[3][VSX_GL_ES_MATRIX_STACK_DEPTH];
int matrix_stack_index = 0;

vsx_matrix compound_matrix;
bool compound_matrix_valid = false;

int matrix_mode = 0;

GLuint shader_program;
GLuint fragmeent_shader_handle;
GLuint vertex_shader_handle;
GLuint vertices_attrib_handle;
GLuint colors_attrib_handle;
GLuint mm_uniform_handle;

#define GL_CHECK(x) \
        x; \
        { \
          GLenum glError = glGetError(); \
          if(glError != GL_NO_ERROR) { \
            fprintf(stderr, "glGetError() = %i (0x%.8x) at line %i\n", glError, glError, __LINE__); \
            exit(0); \
          } \
        }

void vsx_es_load_shader(GLuint *pShader, vsx_string shader_source, GLint iShaderType)
{
  GLint iStatus;
  const char *aStrings[1] = { NULL };

  /* Create shader and load into GL. */
  *pShader = GL_CHECK(glCreateShader(iShaderType));

  const char* source = shader_source.c_str();
  GL_CHECK(glShaderSource(*pShader, 1, &source, NULL));

  /* Try compiling the shader. */
  GL_CHECK(glCompileShader(*pShader));
  GL_CHECK(glGetShaderiv(*pShader, GL_COMPILE_STATUS, &iStatus));

  // Dump debug info (source and log) if compilation failed.
  if(iStatus != GL_TRUE)
  {
#ifdef DEBUG
    GLint iLen;
    char *sDebugSource = NULL;
    char *sErrorLog = NULL;

    /* Get shader source. */
    GL_CHECK(glGetShaderiv(*pShader, GL_SHADER_SOURCE_LENGTH, &iLen));

    sDebugSource = (char*)malloc(iLen);

    GL_CHECK(glGetShaderSource(*pShader, iLen, NULL, sDebugSource));

    printf("Debug source START:\n%s\nDebug source END\n\n", sDebugSource);
    free(sDebugSource);

    /* Now get the info log. */
    GL_CHECK(glGetShaderiv(*pShader, GL_INFO_LOG_LENGTH, &iLen));

    sErrorLog = (char*)malloc(iLen);

    GL_CHECK(glGetShaderInfoLog(*pShader, iLen, NULL, sErrorLog));

    printf("Log START:\n%s\nLog END\n\n", sErrorLog);
    free(sErrorLog);
#endif

    exit(-1);
  }
}



void vsx_es_shader_wrapper_init_shaders()
{

  /* Shader Initialisation */
  vsx_es_load_shader(&vertex_shader_handle, vsx_string("\
attribute vec4 vertices;\
attribute vec4 colors;\
\
uniform mat4 mm;\
\
varying vec4 col;\
\
void main() {\
  col = colors;\
  gl_Position = mm * vertices;\
}\
"), GL_VERTEX_SHADER);
  vsx_es_load_shader(&fragmeent_shader_handle, vsx_string("\
precision lowp float;\
varying vec4 col;\
void main() {\
  gl_FragColor = col;\
}\
"), GL_FRAGMENT_SHADER);

  /* Create uiProgram (ready to attach shaders) */
    shader_program = GL_CHECK(glCreateProgram());

    /* Attach shaders and link uiProgram */
    GL_CHECK(glAttachShader(shader_program, vertex_shader_handle));
    GL_CHECK(glAttachShader(shader_program, fragmeent_shader_handle));
    GL_CHECK(glLinkProgram(shader_program));

    /* Get attribute locations of non-fixed attributes like colour and texture coordinates. */
    vertices_attrib_handle = GL_CHECK(glGetAttribLocation(shader_program, "vertices"));
    colors_attrib_handle = GL_CHECK(glGetAttribLocation(shader_program, "colors"));

#ifdef DEBUG
  printf("vertices_attrib_handle = %i\n", vertices_attrib_handle);
  printf("colors_attrib_handle   = %i\n", colors_attrib_handle);
#endif

  /* Get uniform locations */
    mm_uniform_handle = GL_CHECK(glGetUniformLocation(shader_program, "mm"));

#ifdef DEBUG
  printf("mm_uniform_handle      = %i\n", mm_uniform_handle);
#endif

  GL_CHECK(glUseProgram(shader_program));

  GL_CHECK(glEnableVertexAttribArray(vertices_attrib_handle));
  GL_CHECK(glEnableVertexAttribArray(colors_attrib_handle));

  core_matrix[GL_MODELVIEW].load_identity();
  core_matrix[GL_PROJECTION].load_identity();
}

void vsx_es_set_default_arrays(GLvoid* vertices, GLvoid* colors)
{
  GL_CHECK(glVertexAttribPointer(vertices_attrib_handle, 3, GL_FLOAT, GL_FALSE, 0, vertices));
  GL_CHECK(glVertexAttribPointer(colors_attrib_handle, 4, GL_FLOAT, GL_FALSE, 0, colors));
}

void vsx_es_begin()
{
  if (!compound_matrix_valid)
  {
    compound_matrix.multiply(&core_matrix[GL_MODELVIEW], &core_matrix[GL_PROJECTION]);
    //compound_matrix.dump();
    GL_CHECK(glUniformMatrix4fv(mm_uniform_handle, 1, GL_FALSE, compound_matrix.m));
  }
}

void vsx_es_end()
{
  
}

vsx_matrix m_temp;

void glTranslatef(float x, float y, float z)
{
  /*
  1 0 0 x
  0 1 0 y
  0 0 1 z
  0 0 0 1
  */
  vsx_matrix m_trans;
  m_temp = core_matrix[matrix_mode];
  m_trans.m[12]  = x;
  m_trans.m[13]  = y;
  m_trans.m[14]  = z;
  core_matrix[matrix_mode].multiply(&m_trans, &m_temp);
}

void glScalef(float x, float y, float z)
{
  /*
  x 0 0 0
  0 y 0 0
  0 0 z 0
  0 0 0 1
  */
  vsx_matrix m_scalef;
  m_temp = core_matrix[matrix_mode];
  m_scalef.m[0] = x;
  m_scalef.m[5] = y;
  m_scalef.m[10] = z;
  core_matrix[matrix_mode].multiply(&m_scalef, &m_temp);
}

void glRotatef(float angle, float x, float y, float z)
{
  m_temp = core_matrix[matrix_mode];
  
  vsx_matrix m_rotate;
  /*
    Rotation matrix:
    xx(1-c)+c   xy(1-c)-zs  xz(1-c)+ys   0
    yx(1-c)+zs  yy(1-c)+c   yz(1-c)-xs   0
    xz(1-c)-ys  yz(1-c)+xs  zz(1-c)+c    0
    0           0           0            1
    
    c = cos(angle), s = sin(angle), and ||( x,y,z )|| = 1
*/
  float radians;
  radians = (angle * M_PI) / 180.0;
  float c = cos(radians);
  float s = sin(radians);
  float c1 = 1.0 - c;

  float xx = x*x;
  float yy = y*y;
  float zz = z*z;
  
  //normalize vector
  float length = sqrt(xx + yy + zz);
  if (length != 1.0f)
  {
    x = x / length;
    y = y / length;
    z = z / length;
  }
  
  float xy = x*y;
  float xz = x*z;
  float yz = y*z;
  float xs = x*s;
  float ys = y*s;
  float zs = z*s;

  // note: awkward indexes due to gl es column major format
  //       this differs from regular GL.
  m_rotate.m[0 ] = x*x*(c1)+c;
  m_rotate.m[4 ] = xy*(c1)-zs;
  m_rotate.m[8 ] = xz*(c1)+y*s;

  m_rotate.m[1 ] = xy*(c1)+zs;
  m_rotate.m[5 ] = y*y*(c1)+c;
  m_rotate.m[9 ] = yz*(c1)-x*s;

  m_rotate.m[2 ] = xz*(c1)-y*s;
  m_rotate.m[6 ] = yz*(c1)+x*s;
  m_rotate.m[10] = z*z*(c1)+c;

  core_matrix[matrix_mode].multiply(&m_rotate,&m_temp);
}

void glMatrixMode(int new_value)
{
  matrix_mode = new_value;
}

void glLoadIdentity()
{
  core_matrix[matrix_mode].load_identity();
}

/*
vsx_matrix matrix_stack[3][10];
int matrix_stack_index = 0;
*/

void glPushMatrix()
{
  if (matrix_stack_index + 1 < VSX_GL_ES_MATRIX_STACK_DEPTH)
  {
    matrix_stack[matrix_mode][matrix_stack_index++] = core_matrix[matrix_mode];
  }
}

void glPopMatrix()
{
  if (matrix_stack_index - 1 >= 0)
  {
    core_matrix[matrix_mode] = matrix_stack[matrix_mode][matrix_stack_index--];
  }
}

void glMultMatrixf(GLfloat matrix[16])
{
}

void gluPerspective(double fovy, double aspect, double znear, double zfar ) {
  vsx_matrix m_pers;
  vsx_matrix m_temp = core_matrix[matrix_mode];
  int i;
  double f;

  f = 1.0/tan(fovy * 0.5);

  for (i = 0; i < 16; i++) {
      m_pers.m[i] = 0.0;
  }

  m_pers.m[0] = f / aspect;
  m_pers.m[5] = f;
  m_pers.m[10] = (znear + zfar) / (znear - zfar);
  m_pers.m[11] = -1.0;
  m_pers.m[14] = (2.0 * znear * zfar) / (znear - zfar);
  m_pers.m[15] = 0.0;
  
  core_matrix[matrix_mode].multiply(&m_pers,&m_temp);
}


#endif