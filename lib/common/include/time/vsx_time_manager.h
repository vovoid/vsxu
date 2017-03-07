#pragma once

#include <tools/vsx_singleton.h>

namespace vsx
{
namespace common
{
namespace time
{
class manager
    : public vsx::singleton<manager>
{
public:
  float dt = 0.0f;
  float vt = 0.0f;

  void update(float dt)
  {
    this->dt = dt;
    vt += dt;
  }
};
}
}
}
