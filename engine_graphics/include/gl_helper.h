#ifndef VSX_GL_HELPER_H
#define VSX_GL_HELPER_H


#include <vsx_platform.h>

#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
#define VSX_GL_HELPER_DLLIMPORT
#else
  #if defined(VSX_ENG_DLL)
    #define VSX_GL_HELPER_DLLIMPORT __declspec (dllexport)
  #else 
    #define VSX_GL_HELPER_DLLIMPORT __declspec (dllimport)
  #endif
#endif

VSX_GL_HELPER_DLLIMPORT void draw_box(vsx_vector pos, float width, float height);
VSX_GL_HELPER_DLLIMPORT void draw_box_c(vsx_vector pos, float width, float height);
VSX_GL_HELPER_DLLIMPORT void draw_box_tex(vsx_vector pos, float width, float height);
VSX_GL_HELPER_DLLIMPORT void draw_box_tex_c(vsx_vector pos, float width, float height);
VSX_GL_HELPER_DLLIMPORT void draw_box_tex(float pos_x, float pos_y, float pos_z, float width, float height);
VSX_GL_HELPER_DLLIMPORT void draw_box_gradient(vsx_vector pos, float width, float height, vsx_color a, vsx_color b, vsx_color c, vsx_color d);
VSX_GL_HELPER_DLLIMPORT void draw_box_border(vsx_vector pos, vsx_vector size, float dragborder);
VSX_GL_HELPER_DLLIMPORT vsx_vector vsx_vec_viewport();



#endif
