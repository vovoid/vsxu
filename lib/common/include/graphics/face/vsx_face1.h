#ifndef VSX_FACE1_H
#define VSX_FACE1_H

#ifndef __gl_h_
typedef unsigned int GLuint;
#endif

class vsx_face1
{
public:

  GLuint a;

  vsx_face1()
  {
  }

  vsx_face1(GLuint ia)
    :
      a(ia)
  {}


  static size_t arity()
  {
    return 1;
  }
};



#endif
