#if defined(VSXU_OPENGL_ES)

#include "vsx_math_3d.h"
#include "gl_es_shader_wrapper.h"

vsx_matrix matrices[3];

int matrix_mode = 0;

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