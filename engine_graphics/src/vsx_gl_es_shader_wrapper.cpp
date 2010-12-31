#if defined(VSXU_OPENGL_ES)

#include "vsx_math_3d.h"
#include <stdio.h>
#include <stdlib.h>
#include "vsx_gl_es_shader_wrapper.h"

vsx_matrix matrices[3];

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
            exit(1); \
          } \
        }

void vsx_es_load_shader(GLuint *pShader, vsx_string shader_source, GLint iShaderType)
{
  GLint iStatus;
  const char *aStrings[1] = { NULL };

  /* Create shader and load into GL. */
  *pShader = GL_CHECK(glCreateShader(iShaderType));

  //aStrings[0] = load_shader(sFilename);

  //(shader_source.c_str())
  const char* source = shader_source.c_str();
  GL_CHECK(glShaderSource(*pShader, 1, &source, NULL));

  /* Clean up shader source. */
  free((void *)aStrings[0]);
  aStrings[0] = NULL;

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
attribute vec3 colors;\
\
uniform mat4 mm;\
\
varying vec3 col;\
\
void main() {\
  col = colors;\
  gl_Position = mm * vertices;\
}\
"), GL_VERTEX_SHADER);
  vsx_es_load_shader(&fragmeent_shader_handle, vsx_string("\
precision lowp float;\
varying vec3 col;\
void main() {\
  gl_FragColor = vec4(col, 1.0);\
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
  printf("iLocMVP      = %i\n", iLocMVP);
#endif

  GL_CHECK(glUseProgram(shader_program));


  GL_CHECK(glEnableVertexAttribArray(vertices_attrib_handle));
  GL_CHECK(glEnableVertexAttribArray(colors_attrib_handle));

  matrices[GL_MODELVIEW].load_identity();
  matrices[GL_PROJECTION].load_identity();
}

void vsx_es_set_default_arrays(GLvoid* vertices, GLvoid* colors)
{
  GL_CHECK(glVertexAttribPointer(vertices_attrib_handle, 3, GL_FLOAT, GL_FALSE, 0, vertices));
  GL_CHECK(glVertexAttribPointer(colors_attrib_handle, 3, GL_FLOAT, GL_FALSE, 0, colors));
}

void vsx_es_begin()
{
  if (!compound_matrix_valid)
  {
    compound_matrix.multiply(&matrices[GL_MODELVIEW], &matrices[GL_PROJECTION]);
    GL_CHECK(glUniformMatrix4fv(mm_uniform_handle, 1, GL_FALSE, compound_matrix.m));
  }
}

void vsx_es_end()
{
  
}

void glTranslatef(float x, float y, float z)
{
}

void glScalef(float x, float y, float z)
{
}

void glRotatef(float angle, float x, float y, float z)
{
  static vsx_matrix m;
  
  double radians, c, s, c1, u[3], length;
  int i, j;

  radians = (angle * M_PI) / 180.0;

  c = cos(radians);
  s = sin(radians);

  c1 = 1.0 - cos(radians);

  length = sqrt(x * x + y * y + z * z);

  u[0] = x / length;
  u[1] = y / length;
  u[2] = z / length;

  for (i = 0; i < 16; i++) {
    m.m[i] = 0.0;
  }

  m.m[15] = 1.0;

  for (i = 0; i < 3; i++) {
    m.m[i * 4 + (i + 1) % 3] = u[(i + 2) % 3] * s;
    m.m[i * 4 + (i + 2) % 3] = -u[(i + 1) % 3] * s;
  }

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      m.m[i * 4 + j] += c1 * u[i] * u[j] + (i == j ? c : 0.0);
    }
  }
}

void glMatrixMode(int new_value)
{
  matrix_mode = new_value;
}

void glLoadIdentity()
{
  matrices[matrix_mode].load_identity();
}

void glPushMatrix()
{
}

void glPopMatrix()
{
}

void glMultMatrixf(GLfloat matrix[16])
{
}


#endif