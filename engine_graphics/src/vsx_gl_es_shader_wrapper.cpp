#if defined(VSXU_OPENGL_ES)

#include "vsx_math_3d.h"
#include <stdio.h>
#include <stdlib.h>
#include "vsx_gl_es_shader_wrapper.h"

vsx_matrix matrices[3];

int matrix_mode = 0;


#define GL_CHECK(x) \
        x; \
        { \
          GLenum glError = glGetError(); \
          if(glError != GL_NO_ERROR) { \
            fprintf(stderr, "glGetError() = %i (0x%.8x) at line %i\n", glError, glError, __LINE__); \
            exit(1); \
          } \
        }

void vsx_load_shader(GLuint *pShader, vsx_string shader_source, GLint iShaderType)
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
  if(iStatus != GL_TRUE) {
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


GLuint shader_program;
GLuint fragmeent_shader_handle;
GLuint vertex_shader_handle;

void shader_wrapper_init_shaders()
{

  /* Shader Initialisation */
  vsx_load_shader(&vertex_shader_handle, vsx_string("\
attribute vec4 av4position;\
attribute vec3 av3colour;\
\
uniform mat4 mvp;\
\
varying vec3 vv3colour;\
\
void main() {\
  vv3colour = av3colour;\
  gl_Position = mvp * av4position;\
}\
"), GL_VERTEX_SHADER);
  vsx_load_shader(&fragmeent_shader_handle, vsx_string("\
precision lowp float;\
varying vec3 vv3colour;\
void main() {\
  gl_FragColor = vec4(vv3colour, 1.0);\
}\
"), GL_FRAGMENT_SHADER);

  /* Create uiProgram (ready to attach shaders) */
    shader_program = GL_CHECK(glCreateProgram());

    /* Attach shaders and link uiProgram */
    GL_CHECK(glAttachShader(shader_program, vertex_shader_handle));
    GL_CHECK(glAttachShader(shader_program, fragmeent_shader_handle));
    GL_CHECK(glLinkProgram(shader_program));

    /* Get attribute locations of non-fixed attributes like colour and texture coordinates. */
    iLocPosition = GL_CHECK(glGetAttribLocation(shader_program, "av4position"));
    iLocColour = GL_CHECK(glGetAttribLocation(shader_program, "av3colour"));

#ifdef DEBUG
  printf("iLocPosition = %i\n", iLocPosition);
  printf("iLocColour   = %i\n", iLocColour);
#endif

  /* Get uniform locations */
    iLocMVP = GL_CHECK(glGetUniformLocation(shader_program, "mvp"));

#ifdef DEBUG
  printf("iLocMVP      = %i\n", iLocMVP);
#endif
}

void glTranslatef(float x, float y, float z)
{
}

void glScalef(float x, float y, float z)
{
}

void glRotatef(float angle, float x, float y, float z)
{

  /*
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
        R[i] = 0.0;
    }

    R[15] = 1.0;

    for (i = 0; i < 3; i++) {
        R[i * 4 + (i + 1) % 3] = u[(i + 2) % 3] * s;
        R[i * 4 + (i + 2) % 3] = -u[(i + 1) % 3] * s;
    }

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            R[i * 4 + j] += c1 * u[i] * u[j] + (i == j ? c : 0.0);
        }
    }
  */
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