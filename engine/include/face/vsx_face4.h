#ifndef VSX_FACE4_H
#define VSX_FACE4_H

#ifndef __gl_h_
typedef unsigned int GLuint;
#endif

class vsx_face4
{
public:
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

  GLuint a,b,c,d;

  static size_t arity()
  {
    return 4;
  }
};



#endif
