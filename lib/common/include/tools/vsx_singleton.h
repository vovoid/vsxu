#pragma once

#include <vsx_platform.h>

namespace vsx
{

template<class T>
class singleton
{
public:

  static inline VSX_ALWAYS_INLINE T* get() {
    static T t;
    return &t;
  }
};

}
