#ifndef VSX_TEXCOORD_H
#define VSX_TEXCOORD_H

class vsx_tex_coord2f
{
public:
  GLfloat s,t;
  vsx_tex_coord2f(float ss = 0.0f, float tt = 0.0f)
    :
      s(ss),
      t(tt)
  {
  }

  static size_t arity()
  {
    return 2;
  }
};

class vsx_tex_coord3f
{
public:
  GLfloat s,t,r;
  vsx_tex_coord3f(float ss = 0.0f, float tt = 0.0f, float rr = 0.0f)
    :
      s(ss),
      t(tt),
      r(rr)
  {
  }

  static size_t arity()
  {
    return 3;
  }
};

class vsx_tex_coord4f
{
public:
  GLfloat s,t,r,q;
  vsx_tex_coord4f(float ss = 0.0f, float tt = 0.0f, float rr = 0.0f, float qq = 0.0f)
    :
      s(ss),
      t(tt),
      r(rr),
      q(qq)
  {
  }

  static size_t arity()
  {
    return 4;
  }
};


#endif
