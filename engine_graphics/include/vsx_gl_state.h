#ifndef VSX_GL_STATE_H
#define VSX_GL_STATE_H

// shadow state of OpenGL to avoid glGet* calls

#include "vsx_platform.h"

#include "vsx_math_3d.h"

///////////////////////////////////////////////////////////////////////////////
// WARNING:
//   NO Range checking performed in this class.
//   Be careful!
///////////////////////////////////////////////////////////////////////////////
// TODO:
//   get rid of all calls to glGet(...viewport in the code
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// face direction
#define VSX_GL_FRONT 0
#define VSX_GL_BACK 1

const int gl_face_direction[] =
{
  #ifndef VSX_NO_GL
  GL_FRONT,
  GL_BACK
  #endif
};

///////////////////////////////////////////////////////////////////////////////
// material types
#define VSX_GL_AMBIENT 0
#define VSX_GL_DIFFUSE 1
#define VSX_GL_SPECULAR 2
#define VSX_GL_EMISSION 3
#define VSX_GL_SHININESS 4

const int gl_material_types[] =
{
  #ifndef VSX_NO_GL
  GL_AMBIENT,
  GL_DIFFUSE,
  GL_SPECULAR,
  GL_EMISSION,
  GL_SHININESS
  #endif
};

///////////////////////////////////////////////////////////////////////////////
// matrix modes
#define VSX_GL_PROJECTION_MATRIX 0
#define VSX_GL_MODELVIEW_MATRIX 1
#define VSX_GL_TEXTURE_MATRIX 2
const int gl_matrix_modes[] =
{
  #ifndef VSX_NO_GL
    GL_PROJECTION,
    GL_MODELVIEW,
    GL_TEXTURE
  #endif
};


class vsx_gl_state
{
public:

  vsx_gl_state()
  {
    init_material_values_default();
    init_viewport_values_default();
    init_matrices();
  }

//***************************************************************************
//*** MATERIAL **************************************************************
//***************************************************************************

public:
  // res must be a float[4]
  inline void set_material_fv(int face_direction, int type, float* res)
  {
    material_colors[type][face_direction][0] = res[0];
    material_colors[type][face_direction][1] = res[1];
    material_colors[type][face_direction][2] = res[2];
    material_colors[type][face_direction][3] = res[3];
#ifndef VSX_NO_GL
    glMaterialfv(
          gl_face_direction[face_direction],
          gl_material_types[type],
      material_colors[type][face_direction]
    );
#endif
  }

  // res must be a float[4]
  inline void get_material_fv(int face_direction, int type, float* res)
  {
    res[0] = material_colors[type][face_direction][0];
    res[1] = material_colors[type][face_direction][1];
    res[2] = material_colors[type][face_direction][2];
    res[3] = material_colors[type][face_direction][3];
  }

  inline void get_material_fv_all( float* res)
  {
    memcpy(res, &material_colors[0][0][0], sizeof(material_colors));
  }

  inline void set_material_fv_all(float* res)
  {
    memcpy(&material_colors[0][0][0], res, sizeof(material_colors));
    for (size_t mat = 0; mat < 5; mat++)
    {
      for (size_t face = 0; face < 2; face++)
      {
        set_material_fv(face, mat, material_colors[mat][face]);
      }
    }
  }

  inline void set_default_material()
  {
    init_material_values_default();
    #ifndef VSX_NO_GL
      // ambient
      glMaterialfv(
        GL_FRONT,
        GL_AMBIENT,
        material_colors[VSX_GL_AMBIENT][VSX_GL_FRONT]
      );
      glMaterialfv(
        GL_BACK,
        GL_AMBIENT,
        material_colors[VSX_GL_AMBIENT][VSX_GL_BACK]
      );

      // diffuse
      glMaterialfv(
        GL_FRONT,
        GL_DIFFUSE,
        material_colors[VSX_GL_DIFFUSE][VSX_GL_FRONT]
      );
      glMaterialfv(
        GL_BACK,
        GL_DIFFUSE,
        material_colors[VSX_GL_DIFFUSE][VSX_GL_BACK]
      );

      // specular
      glMaterialfv(
        GL_FRONT,
        GL_SPECULAR,
        material_colors[VSX_GL_SPECULAR][VSX_GL_FRONT]
      );
      glMaterialfv(
        GL_BACK,
        GL_SPECULAR,
        material_colors[VSX_GL_SPECULAR][VSX_GL_BACK]
      );

      // emission
      glMaterialfv(
        GL_FRONT,
        GL_EMISSION,
        material_colors[VSX_GL_EMISSION][VSX_GL_FRONT]
      );
      glMaterialfv(
        GL_BACK,
        GL_EMISSION,
        material_colors[VSX_GL_EMISSION][VSX_GL_BACK]
      );

      // shininess
      glMaterialfv(
        GL_FRONT,
        GL_SHININESS,
        material_colors[VSX_GL_SHININESS][VSX_GL_FRONT]
      );
      glMaterialfv(
        GL_BACK,
        GL_SHININESS,
        material_colors[VSX_GL_SHININESS][VSX_GL_BACK]
      );
    #endif // ifndef VSX_NO_GL
  }

private:

  inline void init_material_values_default()
  {
    // ambient materials
    material_colors[VSX_GL_AMBIENT][VSX_GL_FRONT][0] = 0.2f;
    material_colors[VSX_GL_AMBIENT][VSX_GL_FRONT][1] = 0.2f;
    material_colors[VSX_GL_AMBIENT][VSX_GL_FRONT][2] = 0.2f;
    material_colors[VSX_GL_AMBIENT][VSX_GL_FRONT][3] = 1.0f;

    material_colors[VSX_GL_AMBIENT][VSX_GL_BACK] [0] = 0.2f;
    material_colors[VSX_GL_AMBIENT][VSX_GL_BACK] [1] = 0.2f;
    material_colors[VSX_GL_AMBIENT][VSX_GL_BACK] [2] = 0.2f;
    material_colors[VSX_GL_AMBIENT][VSX_GL_BACK] [3] = 1.0f;

    // diffuse
    material_colors[VSX_GL_DIFFUSE][VSX_GL_FRONT][0] = 0.8f;
    material_colors[VSX_GL_DIFFUSE][VSX_GL_FRONT][1] = 0.8f;
    material_colors[VSX_GL_DIFFUSE][VSX_GL_FRONT][2] = 0.8f;
    material_colors[VSX_GL_DIFFUSE][VSX_GL_FRONT][3] = 1.0f;

    material_colors[VSX_GL_DIFFUSE][VSX_GL_BACK] [0] = 0.8f;
    material_colors[VSX_GL_DIFFUSE][VSX_GL_BACK] [1] = 0.8f;
    material_colors[VSX_GL_DIFFUSE][VSX_GL_BACK] [2] = 0.8f;
    material_colors[VSX_GL_DIFFUSE][VSX_GL_BACK] [3] = 1.0f;

    // specular
    material_colors[VSX_GL_SPECULAR][VSX_GL_FRONT][0] = 0.0f;
    material_colors[VSX_GL_SPECULAR][VSX_GL_FRONT][1] = 0.0f;
    material_colors[VSX_GL_SPECULAR][VSX_GL_FRONT][2] = 0.0f;
    material_colors[VSX_GL_SPECULAR][VSX_GL_FRONT][3] = 1.0f;

    material_colors[VSX_GL_SPECULAR][VSX_GL_BACK] [0] = 0.0f;
    material_colors[VSX_GL_SPECULAR][VSX_GL_BACK] [1] = 0.0f;
    material_colors[VSX_GL_SPECULAR][VSX_GL_BACK] [2] = 0.0f;
    material_colors[VSX_GL_SPECULAR][VSX_GL_BACK] [3] = 1.0f;

    // emission
    material_colors[VSX_GL_EMISSION][VSX_GL_FRONT][0] = 0.0f;
    material_colors[VSX_GL_EMISSION][VSX_GL_FRONT][1] = 0.0f;
    material_colors[VSX_GL_EMISSION][VSX_GL_FRONT][2] = 0.0f;
    material_colors[VSX_GL_EMISSION][VSX_GL_FRONT][3] = 1.0f;

    material_colors[VSX_GL_EMISSION][VSX_GL_BACK] [0] = 0.0f;
    material_colors[VSX_GL_EMISSION][VSX_GL_BACK] [1] = 0.0f;
    material_colors[VSX_GL_EMISSION][VSX_GL_BACK] [2] = 0.0f;
    material_colors[VSX_GL_EMISSION][VSX_GL_BACK] [3] = 1.0f;

    // shininess
    material_colors[VSX_GL_SHININESS][VSX_GL_FRONT][0] = 0.0f;
    material_colors[VSX_GL_SHININESS][VSX_GL_BACK] [0] = 0.0f;
  }

  float material_colors[5][2][4];

//***************************************************************************
//*** VIEWPORT **************************************************************
//***************************************************************************

public:

  inline int get_viewport_width()
  {
    return viewport_size[2];
  }

  inline int get_viewport_height()
  {
    return viewport_size[3];
  }

  // called from window change event
  inline void change_viewport
  (
    int x0,
    int y0,
    int w,
    int h
  )
  {
    vsx_printf("change viewport internal %d %d\n", w, h);
    viewport_size[0] = x0;
    viewport_size[1] = y0;
    viewport_size[2] = w;
    viewport_size[3] = h;
  }

  // substitute for glGetIntegerv(GL_VIEWPORT)
  inline void get_viewport(int* res)
  {
    res[0] = viewport_size[0];
    res[1] = viewport_size[1];
    res[2] = viewport_size[2];
    res[3] = viewport_size[3];
  }

  // substitute for glViewport()
  inline void set_viewport
  (
    const int x0,
    const int y0,
    const int w,
    const int h
  )
  {
    viewport_size[0] = x0;
    viewport_size[1] = y0;
    viewport_size[2] = w;
    viewport_size[3] = h;
#ifndef VSX_NO_GL
    glViewport(x0,y0,w,h);
#endif
  }

private:
  int	viewport_size[4];

  inline void init_viewport_values_default()
  {
    viewport_size[0] = 0;
    viewport_size[1] = 0;
    viewport_size[2] = 0;
    viewport_size[3] = 0;
  }

//***************************************************************************
//*** MATRIX OPS ************************************************************
//***************************************************************************

  vsx_matrix core_matrix[3];
  vsx_matrix matrix_stack[3][32];
  int i_matrix_stack_index;
  int i_matrix_mode;
  vsx_matrix m_temp;
  vsx_matrix m_temp_2;

  inline void init_matrices()
  {
    i_matrix_stack_index = 0;
    i_matrix_mode = 0;
  }

public:

  inline void matrix_get_v(int mode, float* res)
  {
    memcpy(res, &core_matrix[mode].m[0], sizeof(vsx_matrix) );
  }

  inline void matrix_mode(int new_mode)
  {
    i_matrix_mode = new_mode;
    #ifndef VSX_NO_GL
      glMatrixMode(gl_matrix_modes[i_matrix_mode]);
    #endif
  }

  inline void matrix_load_identity()
  {
    core_matrix[i_matrix_mode].load_identity();
    #ifndef VSX_NO_GL
      glLoadIdentity();
    #endif
  }

  inline void matrix_translate_f(float x, float y, float z, bool gl = true)
  {
    /*
    1 0 0 x
    0 1 0 y
    0 0 1 z
    0 0 0 1
    */
    vsx_matrix m_trans;
    m_temp = core_matrix[i_matrix_mode];
    m_trans.m[12]  = x;
    m_trans.m[13]  = y;
    m_trans.m[14]  = z;
    core_matrix[i_matrix_mode].multiply( &m_trans, &m_temp);

    #ifndef VSX_NO_GL
    if (gl)
    {
      glTranslatef(x,y,z);
    }
    #else
    (void)gl;
    #endif
  }

  inline void matrix_rotate_f(float angle, float x, float y, float z)
  {
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

    m_rotate.m[0 ] = x*x*(c1)+c;
    m_rotate.m[4 ] = x*y*(c1)-z*s;
    m_rotate.m[8 ] = x*z*(c1)+y*s;

    m_rotate.m[1 ] = x*y*(c1)+z*s;
    m_rotate.m[5 ] = y*y*(c1)+c;
    m_rotate.m[9 ] = y*z*(c1)-x*s;

    m_rotate.m[2 ] = x*z*(c1)-y*s;
    m_rotate.m[6 ] = y*z*(c1)+x*s;
    m_rotate.m[10] = z*z*(c1)+c;

    memcpy(&m_temp.m[0], &core_matrix[i_matrix_mode].m[0], sizeof(vsx_matrix) );
    core_matrix[i_matrix_mode].multiply( &m_rotate, &m_temp);

    #ifndef VSX_NO_GL
      // TODO: use real OpenGL call here
      glLoadIdentity();
      glMultMatrixf(core_matrix[i_matrix_mode].m);
    #endif
  }

  inline void matrix_mult_f(float* res)
  {
    memcpy(&m_temp.m[0], res, sizeof(vsx_matrix) );
    memcpy(&m_temp_2.m[0], &core_matrix[i_matrix_mode].m[0], sizeof(vsx_matrix));
    core_matrix[i_matrix_mode].multiply( &m_temp, &m_temp_2 );
    #ifndef VSX_NO_GL
      glLoadIdentity();
      glMultMatrixf( core_matrix[i_matrix_mode].m );
    #endif

  }

  inline void matrix_ortho(double left, double right, double bottom, double top, double near, double far)
  {

    #define TX (right+left) / (right-left)
    #define TY (top + bottom) / (top - bottom)
    #define TZ (far + near) / (far - near)
    #define N0 2.0 / (right-left)
    #define N1 2.0 / (top-bottom)
    #define N2 -2.0 / (far-near)

    #define m m_temp.m

    // set up the matrix
    m[0] = N0;   m[4] = 0;    m[8]  =  0;    m[12]  = -TX;
    m[1] = 0;    m[5] = N1;   m[9]  =  0;    m[13]  = -TY;
    m[2] = 0;    m[6] = 0;    m[10] = N2;    m[14]  = -TZ;
    m[3] = 0;    m[7] = 0;    m[11] =  0;    m[15]  =  1;

    // undefine defines
    #undef m
    #undef TX
    #undef TY
    #undef TZ
    #undef N0
    #undef N1
    #undef N2

    // prepare for multiplication
    memcpy(&m_temp_2.m[0], &core_matrix[i_matrix_mode].m[0], sizeof(vsx_matrix));

    // multiply
    core_matrix[i_matrix_mode].multiply(&m_temp, &m_temp_2);

    // implement in OpenGL
    #ifndef VSX_NO_GL
      glMultMatrixf(m_temp.m);
    #endif
  }

  inline void matrix_glu_ortho_2d(
    float left,
    float right,
    float bottom,
    float top
  )
  {
    matrix_mode(VSX_GL_PROJECTION_MATRIX);
    matrix_load_identity();
    matrix_ortho( left, right, bottom, top, 0, 1 );
    matrix_mode(VSX_GL_MODELVIEW_MATRIX);
    matrix_load_identity();
  }

  inline void matrix_glu_perspective(
      double fovy,
      double aspect,
      double zNear,
      double zFar
  )
  {
    matrix_mode(VSX_GL_PROJECTION_MATRIX);
    matrix_load_identity();
    /*

    f is  cot( fovy / 2 )

    cot(x) = tan(HALF_PI - x)

    The generated matrix is
                 f
            ------------       0              0              0
               aspect


                0              f              0              0

                                          zFar+zNear    2*zFar*zNear
                0              0          ----------    ------------
                                          zNear-zFar     zNear-zFar

                0              0              -1             0
      */

#define N0 (zFar + zNear) / (zNear - zFar)
#define N1 (2.0 * zFar * zNear) / (zNear - zFar)
    double f = tan( HALF_PI - (fovy * (PI / 180.0f)) * 0.5 );
    double fdiva = f / aspect;
    #define m m_temp.m
    m[0] = fdiva;         m[4] = 0.0;      m[8] = 0.0;     m[12] = 0.0;
    m[1] = 0.0;          m[5] = f;         m[9] = 0.0;     m[13] = 0.0;
    m[2] = 0.0;          m[6] = 0.0;      m[10] = N0;      m[14] = N1;
    m[3] = 0.0;         m[7] = 0.0;     m[11] = -1.0;    m[15] = 0.0;

#undef m
#undef N0
#undef N1
  memcpy(&m_temp_2.m[0], &core_matrix[i_matrix_mode].m[0], sizeof(vsx_matrix));
  core_matrix[i_matrix_mode].multiply(&m_temp, &m_temp_2);
  #ifndef VSX_NO_GL
  glLoadIdentity();
  glMultMatrixf( core_matrix[i_matrix_mode].m );
#endif

    matrix_mode(VSX_GL_MODELVIEW_MATRIX);
  }

  inline void matrix_glu_lookat(
      float eyex, float eyey, float eyez,
      float centerx, float centery, float centerz,
      float upx, float upy, float upz
  )
  {
    float x[3], y[3], z[3];
    float mag;

    z[0] = eyex - centerx;
    z[1] = eyey - centery;
    z[2] = eyez - centerz;
    mag = sqrt(z[0] * z[0] + z[1] * z[1] + z[2] * z[2]);
    if (mag) {
      z[0] /= mag;
      z[1] /= mag;
      z[2] /= mag;
    }

    y[0] = upx;
    y[1] = upy;
    y[2] = upz;

    x[0] = y[1] * z[2] - y[2] * z[1];
    x[1] = -y[0] * z[2] + y[2] * z[0];
    x[2] = y[0] * z[1] - y[1] * z[0];

    y[0] = z[1] * x[2] - z[2] * x[1];
    y[1] = -z[0] * x[2] + z[2] * x[0];
    y[2] = z[0] * x[1] - z[1] * x[0];

    mag = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
    if (mag) {
      x[0] /= mag;
      x[1] /= mag;
      x[2] /= mag;
    }
    mag = sqrt(y[0] * y[0] + y[1] * y[1] + y[2] * y[2]);
    if (mag) {
      y[0] /= mag;
      y[1] /= mag;
      y[2] /= mag;
    }
    #define M(row,col)  m_temp.m[col*4+row]
       M(0,0) = x[0];  M(0,1) = x[1];  M(0,2) = x[2];  M(0,3) = 0.0;
       M(1,0) = y[0];  M(1,1) = y[1];  M(1,2) = y[2];  M(1,3) = 0.0;
       M(2,0) = z[0];  M(2,1) = z[1];  M(2,2) = z[2];  M(2,3) = 0.0;
       M(3,0) = 0.0;   M(3,1) = 0.0;   M(3,2) = 0.0;   M(3,3) = 1.0;
    #undef M

    memcpy(&m_temp_2.m[0], &core_matrix[i_matrix_mode].m[0], sizeof(vsx_matrix));
    core_matrix[i_matrix_mode].multiply(&m_temp,&m_temp_2);
    matrix_translate_f(-eyex, -eyey, -eyez, false);
    #ifndef VSX_NO_GL
      glLoadIdentity();
      glMultMatrixf(core_matrix[i_matrix_mode].m);
    #endif
  }


};


#endif
