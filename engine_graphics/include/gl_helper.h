#ifndef VSX_GL_HELPER_H
#define VSX_GL_HELPER_H


#include <vsx_platform.h>

#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
#define VSX_GL_HELPER_DLLIMPORT
#else
  #ifdef VSX_ENG_DLL
    #define VSX_GL_HELPER_DLLIMPORT __declspec (dllexport) 
  #else 
    #define VSX_GL_HELPER_DLLIMPORT __declspec (dllimport)
  #endif
#endif

VSX_GL_HELPER_DLLIMPORT void draw_box(vsx_vector pos, float width, float height);
VSX_GL_HELPER_DLLIMPORT void draw_box_c(vsx_vector pos, float width, float height);
VSX_GL_HELPER_DLLIMPORT void draw_box_tex(vsx_vector pos, float width, float height);
VSX_GL_HELPER_DLLIMPORT void draw_box_tex_c(vsx_vector pos, float width, float height);
//VSX_GL_HELPER_DLLIMPORT void draw_box_texf(float pos_x, float pos_y, float pos_z, float width, float height);


#define draw_box_texf(pos_x, pos_y, pos_z, width, height) {\
\
  const GLshort squareTexcoords[] = {\
    0, 0,\
    1, 0,\
    0, 1,\
    1, 1\
  };\
\
  const GLfloat squareVertices[] = {\
    pos_x-width,   pos_y-width,\
    pos_x+width,   pos_y-width,\
    pos_x-width,   pos_y+height,\
    pos_x+width,   pos_y+height,\
  };\
\
  glVertexPointer(2, GL_FLOAT, 0, squareVertices);\
  glTexCoordPointer(2, GL_SHORT, 0, squareTexcoords);\
  glEnableClientState(GL_VERTEX_ARRAY);\
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);\
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);\
}






VSX_GL_HELPER_DLLIMPORT void draw_box_gradient(vsx_vector pos, float width, float height, vsx_color a, vsx_color b, vsx_color c, vsx_color d);
VSX_GL_HELPER_DLLIMPORT void draw_box_border(vsx_vector pos, vsx_vector size, float dragborder);
VSX_GL_HELPER_DLLIMPORT vsx_vector vsx_vec_viewport();



#endif
