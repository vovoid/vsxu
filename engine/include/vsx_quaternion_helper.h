#ifndef VSX_QUATERNION_HELPER_H
#define VSX_QUATERNION_HELPER_H


#include <vsx_platform.h>
#include <vsx_quaternion.h>

namespace vsx_quaternion_helper
{

  template<typename T>
  inline vsx_quaternion<T> from_string(vsx_string &str) VSX_ALWAYS_INLINE
  {
    vsx_quaternion<T> q;
    vsx_avector<vsx_string> parts;
    vsx_string deli = ",";
    explode(str, deli, parts);
    if (parts.size() == 4) {
      q.x = vsx_string_helper::s2f(parts[0]);
      q.y = vsx_string_helper::s2f(parts[1]);
      q.z = vsx_string_helper::s2f(parts[2]);
      q.w = vsx_string_helper::s2f(parts[3]);
    }
    return q;
  }

}

#endif
