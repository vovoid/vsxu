#ifndef VSX_TEXCOORD_H
#define VSX_TEXCOORD_H

class vsx_tex_coord
{
public:
  GLfloat s,t;
  vsx_tex_coord(float ss = 0.0f, float tt = 0.0f)
    :
      s(ss),
      t(tt)
  {
  }
};



#endif
