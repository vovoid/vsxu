#ifndef VSX_FACE_H
#define VSX_FACE_H

#ifndef __gl_h_
typedef unsigned int GLuint;
#endif

class vsx_face {
public:
  vsx_face()
  {
  }

  vsx_face(GLuint ia, GLuint ib, GLuint ic)
    :    a(ia),    b(ib),    c(ic)
  {

  }

  GLuint a,b,c;
};

#endif
