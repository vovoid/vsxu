#ifndef VSX_FACE3_H
#define VSX_FACE3_H

#ifndef __gl_h_
typedef unsigned int GLuint;
#endif

class vsx_face3
{
public:

  GLuint a,b,c;

  vsx_face3()
  {
  }

  vsx_face3(GLuint ia, GLuint ib, GLuint ic)
    :
      a(ia),
      b(ib),
      c(ic)
  {}

  static size_t arity()
  {
    return 3;
  }

};


#endif
