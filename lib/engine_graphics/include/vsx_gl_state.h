#ifndef VSX_GL_STATE_H
#define VSX_GL_STATE_H

// shadow state of OpenGL to avoid glGet* calls

#include <vsx_platform.h>
#include <debug/vsx_error.h>
#include <engine_graphics_dllimport.h>
#include <math/vsx_matrix.h>
#include <string/vsx_string.h>


///////////////////////////////////////////////////////////////////////////////
// WARNING:
//   NO Range checking performed in this class.
//   Be careful!
///////////////////////////////////////////////////////////////////////////////

#define VSX_GL_STATE_STACK_DEPTH 32

///////////////////////////////////////////////////////////////////////////////
// face direction
#define VSX_GL_FRONT 0
#define VSX_GL_BACK 1

const int gl_face_direction[] =
{
  #ifndef VSX_NO_GL
  GL_FRONT,
  GL_BACK
  #else
  0, 0
  #endif
};

///////////////////////////////////////////////////////////////////////////////
// depth functions
#define VSX_GL_NEVER 0
#define VSX_GL_LESS 1
#define VSX_GL_EQUAL 2
#define VSX_GL_LEQUAL 3
#define VSX_GL_GREATER 4
#define VSX_GL_NOTEQUAL 5
#define VSX_GL_GEQUAL 6
#define VSX_GL_ALWAYS 7

const int gl_depth_functions[] =
{
  #ifndef VSX_NO_GL
  GL_NEVER,
  GL_LESS,
  GL_EQUAL,
  GL_LEQUAL,
  GL_GREATER,
  GL_NOTEQUAL,
  GL_GEQUAL,
  GL_ALWAYS
  #else
  0,0,0,0,0,0,0,0
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
  #else
  0
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
  #else
  0
  #endif
};

///////////////////////////////////////////////////////////////////////////////
// blend modes

// source factors
#define VSX_GL_ZERO 0
#define VSX_GL_ONE 1
#define VSX_GL_SRC_COLOR 2
#define VSX_GL_ONE_MINUS_SRC_COLOR 3
#define VSX_GL_DST_COLOR 4
#define VSX_GL_ONE_MINUS_DST_COLOR 5
#define VSX_GL_SRC_ALPHA 6
#define VSX_GL_ONE_MINUS_SRC_ALPHA 7
#define VSX_GL_DST_ALPHA 8
#define VSX_GL_ONE_MINUS_DST_ALPHA 9
#define VSX_GL_CONSTANT_COLOR 10
#define VSX_GL_ONE_MINUS_CONSTANT_COLOR 11
#define VSX_GL_CONSTANT_ALPHA 12
#define VSX_GL_ONE_MINUS_CONSTANT_ALPHA 13
#define VSX_GL_SRC_ALPHA_SATURATE 14
#define VSX_GL_SRC1_COLOR 15
#define VSX_GL_ONE_MINUS_SRC1_COLOR 16
#define VSX_GL_SRC1_ALPHA 17
#define VSX_GL_ONE_MINUS_SRC1_ALPHA 18

const int gl_blend_factors[] =
{
  #ifndef VSX_NO_GL
  GL_ZERO,
  GL_ONE,
  GL_SRC_COLOR,
  GL_ONE_MINUS_SRC_COLOR,
  GL_DST_COLOR,
  GL_ONE_MINUS_DST_COLOR,
  GL_SRC_ALPHA,
  GL_ONE_MINUS_SRC_ALPHA,
  GL_DST_ALPHA,
  GL_ONE_MINUS_DST_ALPHA,
  GL_CONSTANT_COLOR,
  GL_ONE_MINUS_CONSTANT_COLOR,
  GL_CONSTANT_ALPHA,
  GL_ONE_MINUS_CONSTANT_ALPHA,
  GL_SRC_ALPHA_SATURATE,
  GL_SRC1_COLOR,
  GL_ONE_MINUS_SRC1_COLOR,
  GL_SRC1_ALPHA,
  GL_ONE_MINUS_SRC1_ALPHA
  #else
  0
  #endif
};

///////////////////////////////////////////////////////////////////////////////
// polygon fill modes
#define VSX_GL_POINT 0
#define VSX_GL_LINE 1
#define VSX_GL_FILL 2

const int gl_polygon_fill[] =
{
  #ifndef VSX_NO_GL
  GL_POINT,
  GL_LINE,
  GL_FILL
  #else
  0
  #endif
};

VSX_ENGINE_GRAPHICS_DLLIMPORT class vsx_gl_state
{
public:

//***************************************************************************
//*** FRAMEBUFFER BINDING ***************************************************
//***************************************************************************
public:
  void framebuffer_bind(unsigned int id)
  {
    _framebuffer_binding = id;
    #ifndef VSX_NO_GL
//    vsx_printf(L"binding buffer %d\n", id);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, id);
    #endif
  }

  unsigned int framebuffer_bind_get()
  {
//    vsx_printf(L"getting buffer %d\n", _framebuffer_binding);
    return _framebuffer_binding;
  }

private:

  unsigned int _framebuffer_binding = 0;


//***************************************************************************
//*** POINT SIZE ************************************************************
//***************************************************************************
public:
  inline void point_size_set(float n)
  {
    _point_size = n;
    #ifndef VSX_NO_GL
    glPointSize( _line_width );
    #endif
  }

  inline float point_size_get()
  {
    return _point_size;
  }

private:
  float _point_size = 1.0f;


//***************************************************************************
//*** LINE WIDTH ************************************************************
//***************************************************************************
public:
  inline void line_width_set(float n)
  {
    if (n < 1.0f)
      n = 1.0f;
    _line_width = n;
    #ifndef VSX_NO_GL
    glLineWidth( _line_width );
    #endif
  }

  inline float line_width_get()
  {
    return _line_width;
  }

private:
  float _line_width = 1.0f;


//***************************************************************************
//*** DEPTH BUFFER **********************************************************
//***************************************************************************

  int _depth_mask_stack[VSX_GL_STATE_STACK_DEPTH];
  size_t _depth_mask_stack_pointer = 0;

  int _depth_test_stack[VSX_GL_STATE_STACK_DEPTH];
  size_t _depth_test_stack_pointer = 0;

public:

  void depth_mask_push()
  {
    if (_depth_test_stack_pointer + 1 == VSX_GL_STATE_STACK_DEPTH)
      VSX_ERROR_RETURN("Depth mask stack overflow!");

    _depth_mask_stack[_depth_test_stack_pointer] = _depth_mask_enabled;
    _depth_test_stack_pointer++;
  }

  void depth_mask_pop()
  {
    if (!_depth_test_stack_pointer)
      VSX_ERROR_RETURN("Depth mask stack underrun!");

    _depth_test_stack_pointer--;
    depth_mask_set(_depth_mask_stack[_depth_test_stack_pointer]);
  }

  void depth_test_push()
  {
    if (_depth_test_stack_pointer + 1 == VSX_GL_STATE_STACK_DEPTH)
      VSX_ERROR_RETURN("Depth test stack overflow!");

    _depth_test_stack[_depth_test_stack_pointer] = _depth_test_enabled;
    _depth_test_stack_pointer++;
  }

  void depth_test_pop()
  {
    if (!_depth_test_stack_pointer)
      VSX_ERROR_RETURN("Depth test stack underrun!");

    _depth_test_stack_pointer--;
    depth_test_set(_depth_test_stack[_depth_test_stack_pointer]);
  }

  // depth mask
  void depth_mask_set(int n, bool debug = false)
  {
    (void)debug;
//    if (n == _depth_mask_enabled)
//    {
//      vsx_printf(L"depth mask already this\n");
//      return;
//    }
    _depth_mask_enabled = n;
    #ifndef VSX_NO_GL
    if (n)
    {
      glDepthMask( GL_TRUE );
    } else
    {
      glDepthMask( GL_FALSE );
    }
    #else
    vsx_printf(L"warning: using depth mask without GL\n");
    #endif
  }

  int depth_mask_get()
  {
    return _depth_mask_enabled;
  }

  // depth test
  void depth_test_set(int n, bool debug = false)
  {
    (void)debug;
//    if (n == _depth_test_enabled)
//      return;
    _depth_test_enabled = n;
    #ifndef VSX_NO_GL
    if (_depth_test_enabled)
    {
      glEnable(GL_DEPTH_TEST);
    } else
    {
      glDisable(GL_DEPTH_TEST);
    }
    #endif
  }

  int depth_test_get()
  {
    return _depth_test_enabled;
  }

  // depth function
  int depth_function_get()
  {
    return _depth_function;
  }

  void depth_function_set(int n)
  {
//    if ( n == _depth_function )
//      return;
    _depth_function = n;
    #ifndef VSX_NO_GL
    glDepthFunc( gl_depth_functions[_depth_function] );
    #endif
  }

private:
  int _depth_mask_enabled = 0;
  int _depth_test_enabled = 0;
  int _depth_function = 1;



//***************************************************************************
//*** FILL MODE *************************************************************
//***************************************************************************
public:
  void polygon_mode_set(int face, int mode)
  {
    if (_polygon_mode[face] == mode) return;
    _polygon_mode[face] = mode;
    #ifndef VSX_NO_GL
      glPolygonMode(
        gl_face_direction[face],
        gl_polygon_fill[mode]
      );
    #endif
  }

  int polygon_mode_get(int face)
  {
    return _polygon_mode[face];
  }

private:
  int _polygon_mode[2];

  void polygon_mode_init_default()
  {
    _polygon_mode[0] = 0;
    _polygon_mode[1] = 0;
  }

//***************************************************************************
//*** BLEND MODE ************************************************************
//***************************************************************************

public:
  inline int blend_get()
  {
    return _blend_enabled;
  }

  inline void blend_set(int n)
  {
//    if (n == _blend_enabled) return;
    _blend_enabled = n;
    #ifndef VSX_NO_GL
    if (_blend_enabled)
    {
      glEnable(GL_BLEND);
    } else
    {
      glDisable(GL_BLEND);
    }
    #endif
  }

  inline int blend_src_get()
  {
    return _blend_src;
  }

  inline int blend_dst_get()
  {
    return _blend_dst;
  }

  inline void blend_color_v(float* res)
  {
    memcpy( res, &_blend_color[0], sizeof(float)*4 );
  }

  inline void blend_color_set(float r, float g, float b, float a)
  {
    _blend_color[0] = r;
    _blend_color[1] = g;
    _blend_color[2] = b;
    _blend_color[3] = a;

    #ifndef VSX_NO_GL
    if (!GLEW_EXT_blend_color) return;
    glBlendColor(
      _blend_color[0],
      _blend_color[1],
      _blend_color[2],
      _blend_color[3]
    );
    #endif
  }

  inline void blend_func(int src, int dst)
  {
    _blend_src = src;
    _blend_dst = dst;

    #ifndef VSX_NO_GL
    glBlendFunc
    (
      gl_blend_factors[src],
      gl_blend_factors[dst]
    );
    #endif
  }

  inline void blend_func_push()
  {
    _blend_func_stack[0][_blend_func_stack_pointer] = _blend_src;
    _blend_func_stack[1][_blend_func_stack_pointer] = _blend_dst;
    _blend_func_stack_pointer++;
  }

  inline void blend_func_pop()
  {
    _blend_func_stack_pointer--;
    blend_func(
      _blend_func_stack[0][_blend_func_stack_pointer],
      _blend_func_stack[1][_blend_func_stack_pointer]
    );
  }

private:

  int _blend_enabled = 0;
  int _blend_src = 0;
  int _blend_dst = 0;
  float _blend_color[4] = {0.0f, 0.0f, 0.0f, 1.0f};

  int _blend_func_stack[2][VSX_GL_STATE_STACK_DEPTH];
  size_t _blend_func_stack_pointer = 0;


//***************************************************************************
//*** MATERIAL **************************************************************
//***************************************************************************

public:
  // res must be a float[4]
  inline void material_set_fv(size_t face_direction, size_t type, float* res)
  {
    if (!res)
      return;

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

  inline void material_set_fv_intensity_rgb(int face_direction, int type, float* res, float intensity_rgb)
  {
    if (!res)
      return;

    material_colors[type][face_direction][0] = res[0] * intensity_rgb;
    material_colors[type][face_direction][1] = res[1] * intensity_rgb;
    material_colors[type][face_direction][2] = res[2] * intensity_rgb;
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
  inline void material_get_fv(int face_direction, int type, float* res)
  {
    if (!res)
      return;

    res[0] = material_colors[type][face_direction][0];
    res[1] = material_colors[type][face_direction][1];
    res[2] = material_colors[type][face_direction][2];
    res[3] = material_colors[type][face_direction][3];
  }

  inline void get_material_fv_all( float* res)
  {
    if (!res)
      return;

    memcpy(res, &material_colors[0][0][0], sizeof(material_colors));
  }

  inline void material_set_fv_all(float* res)
  {
    if (!res)
      return;

    memcpy(&material_colors[0][0][0], res, sizeof(material_colors));
    for (size_t mat = 0; mat < 5; mat++)
    {
      for (size_t face = 0; face < 2; face++)
      {
        material_set_fv(face, mat, material_colors[mat][face]);
      }
    }
  }

  inline void _material_init_values_default()
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

  inline void material_set_default()
  {
    _material_init_values_default();
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

  float material_colors[5][2][4] =
    {
      // ambient
      {
        {0.2f, 0.2f, 0.2f, 1.0f},
        {0.2f, 0.2f, 0.2f, 1.0f}
      },
      // diffuse
      {
        {0.8f, 0.8f, 0.8f, 1.0f},
        {0.8f, 0.8f, 0.8f, 1.0f}
      },
      // specular
      {
        {0.0f, 0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
      },
      // emission
      {
        {0.0f, 0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
      },
      // shininess
      {
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f}
      },

    }
  ;

//***************************************************************************
//*** VIEWPORT **************************************************************
//***************************************************************************

public:

  inline int viewport_get_width()
  {
    return _viewport_size[2];
  }

  inline int viewport_get_height()
  {
    return _viewport_size[3];
  }

  // called from window change event
  inline void viewport_change
  (
    int x0,
    int y0,
    int w,
    int h
  )
  {
    _viewport_size[0] = x0;
    _viewport_size[1] = y0;
    _viewport_size[2] = w;
    _viewport_size[3] = h;
    _viewport_w_d_h = (float)w / (float)h;
  }

  // substitute for glGetIntegerv(GL_VIEWPORT)
  inline void viewport_get(int* res)
  {
    res[0] = _viewport_size[0];
    res[1] = _viewport_size[1];
    res[2] = _viewport_size[2];
    res[3] = _viewport_size[3];
  }

  inline float viewport_width_div_height_get()
  {
    return _viewport_w_d_h;
  }

  inline float viewport_normalize_from_16_9(float v)
  {
    return v / (16.0f / 9.0f) * _viewport_w_d_h;
  }

  inline vsx_vector3f viewport_normalize_from_16_9(vsx_vector3f v)
  {
    return vsx_vector3f(
      viewport_normalize_from_16_9(v.x),
      v.y,
      v.z
    );
  }

  // substitute for glViewport()
  inline void viewport_set
  (
    const int x0,
    const int y0,
    const int w,
    const int h
  )
  {
    _viewport_size[0] = x0;
    _viewport_size[1] = y0;
    _viewport_size[2] = w;
    _viewport_size[3] = h;
    _viewport_w_d_h = (float)w / (float)h;
#ifndef VSX_NO_GL
    glViewport(x0,y0,w,h);
#endif
  }


  // substitute for glViewport()
  inline void viewport_set
  (
    int* values
  )
  {
    _viewport_size[0] = values[0];
    _viewport_size[1] = values[1];
    _viewport_size[2] = values[2];
    _viewport_size[3] = values[3];
    _viewport_w_d_h = (float)values[2] / (float)values[3];
#ifndef VSX_NO_GL
    glViewport(values[0],values[1],values[2],values[3]);
#endif
  }


private:
  int _viewport_size[4] = {0,0,0,0};
  float _viewport_w_d_h = 1.0f; // width divided by height


//***************************************************************************
//*** MATRIX OPS ************************************************************
//***************************************************************************

  vsx_matrix<float> core_matrix[3];
  vsx_matrix<float> matrix_stack[3][VSX_GL_STATE_STACK_DEPTH];
  int i_matrix_stack_pointer[3] = {0, 0, 0};
  int i_matrix_mode = 0;
  vsx_matrix<float> m_temp;
  vsx_matrix<float> m_temp_2;

public:

  inline void matrix_load_identities()
  {
    core_matrix[0].load_identity();
    core_matrix[1].load_identity();
    core_matrix[2].load_identity();
    #ifndef VSX_NO_GL
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    #endif
  }

  inline void matrix_get_v(int mode, float* res)
  {
    memcpy(res, &core_matrix[mode].m[0], sizeof(vsx_matrix<float>) );
  }

  inline vsx_matrix<float>& matrix_get()
  {
    return core_matrix[i_matrix_mode];
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

  // push and pop
  inline void matrix_push()
  {
    if (i_matrix_stack_pointer[i_matrix_mode] + 1 > VSX_GL_STATE_STACK_DEPTH)
      VSX_ERROR_RETURN("Trying to push to a full matrix stack!\n");

    matrix_stack[i_matrix_mode][i_matrix_stack_pointer[i_matrix_mode]] = core_matrix[i_matrix_mode];
    i_matrix_stack_pointer[i_matrix_mode]++;
    #ifndef VSX_NO_GL
      glPushMatrix();
    #endif
  }

  inline void matrix_pop()
  {
    if (!i_matrix_stack_pointer[i_matrix_mode])
      VSX_ERROR_RETURN("Error, popping off of an empty matrix stack\n");

    i_matrix_stack_pointer[i_matrix_mode]--;
    core_matrix[i_matrix_mode] = matrix_stack[i_matrix_mode][i_matrix_stack_pointer[i_matrix_mode]];
    #ifndef VSX_NO_GL
      glPopMatrix();
    #endif
  }



  inline void matrix_scale_f(float x, float y, float z, bool gl = true)
  {
    /*
    x 0 0 0
    0 y 0 0
    0 0 z 0
    0 0 0 1
    */
    m_temp.m[0] = x;     m_temp.m[4] = 0.0f;   m_temp.m[8] = 0.0f;   m_temp.m[12] = 0.0f;
    m_temp.m[1] = 0.0f;  m_temp.m[5] = y;      m_temp.m[9] = 0.0f;   m_temp.m[13] = 0.0f;
    m_temp.m[2] = 0.0f;  m_temp.m[6] = 0.0f;   m_temp.m[10] = z;     m_temp.m[14] = 0.0f;
    m_temp.m[3] = 0.0f;  m_temp.m[7] = 0.0f;   m_temp.m[11] = 0.0f;  m_temp.m[15] = 1.0f;

    memcpy(m_temp_2.m, core_matrix[i_matrix_mode].m, sizeof(vsx_matrix<float>));
    core_matrix[i_matrix_mode].multiply( &m_temp, &m_temp_2);

    #ifndef VSX_NO_GL
    if (gl)
    {
      glLoadIdentity();
      glMultMatrixf(core_matrix[i_matrix_mode].m);
    }
    #else
    (void)gl;
    #endif
  }

  inline void matrix_translate_d(double x, double y, double z, bool gl = true)
  {
    matrix_translate_f((float)x, (float)y, (float)z, gl);
  }



  inline void matrix_translate_f(float x, float y, float z, bool gl = true)
  {
    /*
    1 0 0 x
    0 1 0 y
    0 0 1 z
    0 0 0 1
    */
    m_temp.m[0] = 1.0f;  m_temp.m[4] = 0.0f;   m_temp.m[8] = 0.0f;   m_temp.m[12] = x;
    m_temp.m[1] = 0.0f;  m_temp.m[5] = 1.0f;   m_temp.m[9] = 0.0f;   m_temp.m[13] = y;
    m_temp.m[2] = 0.0f;  m_temp.m[6] = 0.0f;   m_temp.m[10] = 1.0f;  m_temp.m[14] = z;
    m_temp.m[3] = 0.0f;  m_temp.m[7] = 0.0f;   m_temp.m[11] = 0.0f;  m_temp.m[15] = 1.0f;

    memcpy(m_temp_2.m, core_matrix[i_matrix_mode].m, sizeof(vsx_matrix<float>));
    core_matrix[i_matrix_mode].multiply( &m_temp, &m_temp_2);

    #ifndef VSX_NO_GL
    if (gl)
    {
      glLoadIdentity();
      glMultMatrixf(core_matrix[i_matrix_mode].m);
    }
    #else
    (void)gl;
    #endif
  }

  inline void matrix_rotate_f(float angle, float x, float y, float z)
  {
    vsx_matrix<float> m_rotate;
    /*
      Rotation matrix:
      xx(1-c)+c   xy(1-c)-zs  xz(1-c)+ys   0
      yx(1-c)+zs  yy(1-c)+c   yz(1-c)-xs   0
      xz(1-c)-ys  yz(1-c)+xs  zz(1-c)+c    0
      0           0           0            1

      c = cos(angle), s = sin(angle), and ||( x,y,z )|| = 1
    */
    float radians;
    radians = (angle * (float)PI) / 180.0f;
    float c = cosf(radians);
    float s = sinf(radians);
    float c1 = 1.0f - c;

    float xx = x*x;
    float yy = y*y;
    float zz = z*z;

    //normalize vector
    float length = (float)sqrt(xx + yy + zz);
    if (FLOAT_EQUALS(length, 1.0f))
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

    memcpy(&m_temp.m[0], &core_matrix[i_matrix_mode].m[0], sizeof(vsx_matrix<float>) );
    core_matrix[i_matrix_mode].multiply( &m_rotate, &m_temp);

    #ifndef VSX_NO_GL
      // TODO: use real OpenGL call here
      glLoadIdentity();
      glMultMatrixf(core_matrix[i_matrix_mode].m);
    #endif
  }

  inline void matrix_mult_f(float* res)
  {
    memcpy(&m_temp.m[0], res, sizeof(vsx_matrix<float>) );
    memcpy(&m_temp_2.m[0], &core_matrix[i_matrix_mode].m[0], sizeof(vsx_matrix<float>));
    core_matrix[i_matrix_mode].multiply( &m_temp, &m_temp_2 );
    #ifndef VSX_NO_GL
      glLoadIdentity();
      glMultMatrixf( core_matrix[i_matrix_mode].m );
    #endif

  }

  inline void matrix_ortho(double left, double right, double bottom, double top, double near_limit, double far_limit)
  {

    #define TX (float)((right + left) / (right - left))
    #define TY (float)((top + bottom) / (top - bottom))
    #define TZ (float)((far_limit + near_limit) / (far_limit - near_limit))
    #define N0 (float)(2.0f / (right - left))
    #define N1 (float)(2.0f / (top - bottom))
    #define N2 (float)(-2.0f / (far_limit - near_limit))

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
    memcpy(&m_temp_2.m[0], &core_matrix[i_matrix_mode].m[0], sizeof(vsx_matrix<float>));

    // multiply
    core_matrix[i_matrix_mode].multiply(&m_temp, &m_temp_2);

    // implement in OpenGL
    #ifndef VSX_NO_GL
      glMultMatrixf(m_temp.m);
    #endif
  }

  inline void matrix_frustum(double left, double right, double bottom, double top, double z_near, double z_far) {
    #define N0 (float)((2.0 * z_near) / (right - left))
    #define N1 (float)((2.0 * z_near) / (top - bottom))
    #define A (float)((right + left) / (right - left))
    #define B (float)((top + bottom) / (top - bottom))
    #define C (float)(-(z_far + z_near) / (z_far - z_near))
    #define D (float)(-(2.0 * z_far * z_near) / (z_far - z_near))

    #define m m_temp.m

    // set up the matrix
    m[0] = N0;   m[4] = 0;    m[8]  =  A;    m[12]  = 0;
    m[1] = 0;    m[5] = N1;   m[9]  =  B;    m[13]  = 0;
    m[2] = 0;    m[6] = 0;    m[10] =  C;    m[14]  = D;
    m[3] = 0;    m[7] = 0;    m[11] = -1;    m[15]  = 0;

    #undef m
    #undef N0
    #undef N1
    #undef A
    #undef B
    #undef C
    #undef D

    // prepare for multiplication
    memcpy(&m_temp_2.m[0], &core_matrix[i_matrix_mode].m[0], sizeof(vsx_matrix<float>));

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

    #define N0 (float)((zFar + zNear) / (zNear - zFar))
    #define N1 (float)((2.0 * zFar * zNear) / (zNear - zFar))
    double f = tan( HALF_PI - (fovy * (PI / 180.0)) * 0.5 );
    double fdiva = f / aspect;
    #define m m_temp.m
    m[0] = (float)fdiva; m[4] = 0.0;      m[8] = 0.0;     m[12] = 0.0;
    m[1] = 0.0;          m[5] = (float)f; m[9] = 0.0;     m[13] = 0.0;
    m[2] = 0.0;          m[6] = 0.0;      m[10] = N0;     m[14] = N1;
    m[3] = 0.0;          m[7] = 0.0;      m[11] = -1.0;   m[15] = 0.0;
    #undef m
    #undef N0
    #undef N1

    memcpy(&m_temp_2.m[0], &core_matrix[i_matrix_mode].m[0], sizeof(vsx_matrix<float>));
    core_matrix[i_matrix_mode].multiply(&m_temp, &m_temp_2);
    #ifndef VSX_NO_GL
      glLoadIdentity();
      glMultMatrixf( core_matrix[i_matrix_mode].m );
    #endif

    matrix_mode(VSX_GL_MODELVIEW_MATRIX);
  }

  inline void matrix_glu_lookatd(
      double eyex, double eyey, double eyez,
      double centerx, double centery, double centerz,
      double upx, double upy, double upz
  )
  {
    matrix_glu_lookatf(
      (float)eyex,
      (float)eyey,
      (float)eyez,
      (float)centerx,
      (float)centery,
      (float)centerz,
      (float)upx,
      (float)upy,
      (float)upz
    );
  }


  inline void matrix_glu_lookatf(
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


    mag = sqrtf(z[0] * z[0] + z[1] * z[1] + z[2] * z[2]);
    if (mag > 0.0f)
    {
      z[0] /= mag;
      z[1] /= mag;
      z[2] /= mag;
    }


    y[0] = upx;
    y[1] = upy;
    y[2] = upz;

    x[0] =  y[1] * z[2] - y[2] * z[1];
    x[1] = -y[0] * z[2] + y[2] * z[0];
    x[2] =  y[0] * z[1] - y[1] * z[0];

    y[0] =  z[1] * x[2] - z[2] * x[1];
    y[1] = -z[0] * x[2] + z[2] * x[0];
    y[2] =  z[0] * x[1] - z[1] * x[0];


    mag = sqrtf(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
    if (mag > 0.0f)
    {
      x[0] /= mag;
      x[1] /= mag;
      x[2] /= mag;
    }

    mag = sqrtf(y[0] * y[0] + y[1] * y[1] + y[2] * y[2]);
    if (mag > 0.0f)
    {
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

    memcpy(&m_temp_2.m[0], &core_matrix[i_matrix_mode].m[0], sizeof(vsx_matrix<float>));
    core_matrix[i_matrix_mode].multiply(&m_temp,&m_temp_2);
    matrix_translate_f(-eyex, -eyey, -eyez, false);
    #ifndef VSX_NO_GL
      glLoadIdentity();
      glMultMatrixf(core_matrix[i_matrix_mode].m);
    #endif
  }

private:
  vsx_string<> gl_errors;

public:

  inline void clear_errors()
  {
    accumulate_errors();
    gl_errors = "";
  }

  inline vsx_string<>& get_errors()
  {
    return gl_errors;
  }

  inline void accumulate_errors()
  {
#ifndef VSX_NO_GL
    GLenum errCode;
    const GLubyte *errString;

    if ((errCode = glGetError()) != GL_NO_ERROR) {
        errString = gluErrorString(errCode);
        gl_errors += vsx_string<>((char*)errString) + "\n";
        vsx_printf(L"error string: %hs\n", errString);
    }
#endif
  }

private:
  VSX_ENGINE_GRAPHICS_DLLIMPORT static vsx_gl_state instance;
public:
  inline static vsx_gl_state* get_instance()
  {
    return &instance;
  }

};



#endif
