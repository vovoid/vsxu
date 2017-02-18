#ifndef VSX_FACE4_H
#define VSX_FACE4_H

#ifndef __gl_h_
typedef unsigned int GLuint;
#endif

class vsx_face4
{
public:

  GLuint a,b,c,d;

  vsx_face4()
  {
  }

  vsx_face4(GLuint ia, GLuint ib, GLuint ic, GLuint id)
    :
      a(ia),
      b(ib),
      c(ic),
      d(id)
  {}

  static size_t arity()
  {
    return 4;
  }
};



#endif
