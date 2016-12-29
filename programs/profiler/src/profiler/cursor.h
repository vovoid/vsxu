#pragma once

#include <vsx_platform.h>

class cursor
{
public:
  vsx_vector3<> pos;

  inline static cursor* get_instance() VSX_ALWAYS_INLINE
  {
    static cursor c;
    return &c;
  }
};
