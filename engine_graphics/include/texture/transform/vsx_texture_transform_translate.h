#pragma once

class vsx_texture_transform_translate : public vsx_texture_transform_base
{
  float x, y, z;
public:

  vsx_texture_transform_translate(float x1, float y1, float z1, vsx_texture_transform_base* prev_trans = 0)
    :
      vsx_texture_transform_base(prev_trans),
      x(x1),
      y(y1),
      z(z1)
  {
    valid = 1;
  }

  void transform()
  {
    vsx_texture_transform_base::transform();
    glTranslatef(x, y, z);
  }

  void update(float nx = 0, float ny = 0, float nz = 0)
  {
    x = nx; y = ny; z = nz;
  }
};

