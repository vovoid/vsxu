#pragma once

class vsx_texture_transform_rotate : public vsx_texture_transform_base{
  float angle, x, y, z;
public:
  vsx_texture_transform_rotate(float angle1, float x1, float y1, float z1, vsx_texture_transform_base* prev_trans = 0)
    :
      vsx_texture_transform_base(prev_trans),
      angle(angle1),
      x(x1),
      y(y1),
      z(z1)
  {
    valid = 1;
  }

  void transform()
  {
    vsx_texture_transform_base::transform();
    glRotatef(angle, x, y, z);
  }

  void update(float nangle = 0, float nx = 1, float ny = 0, float nz = 0) {
    angle = nangle; x = nx; y = ny; z = nz;
  }
};
