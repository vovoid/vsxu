#pragma once

class vsx_texture_transform_scale : public vsx_texture_transform_base
{
  float x, y, z;
public:
  vsx_texture_transform_scale(float x1, float y1, float z1, vsx_texture_transform_base* prev_trans = 0)
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
    glScalef(x, y, z);
  }

  void update(float nx = 1, float ny = 1, float nz = 1)
  {
    x = nx; y = ny; z = nz;
  }
};
