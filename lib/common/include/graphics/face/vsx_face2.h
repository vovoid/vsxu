#ifndef VSX_FACE2_H
#define VSX_FACE2_H

#ifndef __gl_h_
typedef unsigned int GLuint;
#endif

class vsx_face2
{
public:

  GLuint a,b;

  vsx_face2()
  {
  }

  vsx_face2(GLuint ia, GLuint ib)
    :
      a(ia),
      b(ib)
  {}

  static size_t arity()
  {
    return 2;
  }
};


#endif
