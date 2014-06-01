#ifndef VSX_FACE_H
#define VSX_FACE_H

#ifndef __gl_h_
typedef unsigned int GLuint;
#endif

class vsx_face1
{
public:
  vsx_face1()
  {
  }

  vsx_face1(GLuint ia)
    :    a(ia)
  {

  }

  GLuint a;

  static size_t arity()
  {
    return 1;
  }
};


class vsx_face2
{
public:
  vsx_face2()
  {
  }

  vsx_face2(GLuint ia, GLuint ib)
    :    a(ia),    b(ib)
  {

  }

  GLuint a,b;

  static size_t arity()
  {
    return 2;
  }
};


class vsx_face3
{
public:

  GLuint a,b,c;

  vsx_face3()
  {
  }

  vsx_face3(GLuint ia, GLuint ib, GLuint ic)
    :    a(ia),    b(ib),    c(ic)
  {
  }

  static size_t arity()
  {
    return 3;
  }

};

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
