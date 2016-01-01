#ifndef VSX_QUATERNION_HELPER_H
#define VSX_QUATERNION_HELPER_H


#include <vsx_platform.h>
#include <math/quaternion/vsx_quaternion.h>

namespace vsx_quaternion_helper
{
  template<typename T>
  inline vsx_quaternion<T> from_string(vsx_string<>&str) VSX_ALWAYS_INLINE
  {
    vsx_quaternion<T> q;
    vsx_nw_vector< vsx_string<> > parts;
    vsx_string<>deli = ",";
    vsx_string_helper::explode(str, deli, parts);
    if (parts.size() == 4) {
      q.x = vsx_string_helper::s2f(parts[0]);
      q.y = vsx_string_helper::s2f(parts[1]);
      q.z = vsx_string_helper::s2f(parts[2]);
      q.w = vsx_string_helper::s2f(parts[3]);
    }
    return q;
  }

  template<typename T>
  inline vsx_string<>to_string(const vsx_quaternion<T> &q) VSX_ALWAYS_INLINE
  {
    vsx_string<>local;
    local += vsx_string_helper::f2s(q.x);
    local += vsx_string<>(",")+vsx_string_helper::f2s(q.y);
    local += vsx_string<>(",")+vsx_string_helper::f2s(q.z);
    local += vsx_string<>(",")+vsx_string_helper::f2s(q.w);
    return local;
  }

  template<typename T>
  inline void dump(const vsx_quaternion<T> &q, char* label="n/a")
  {
    vsx_printf( "Name: %s. x: %f y: %f z: %f w: %f\n",label, q.x, q.y, q.z, q.w );
  }
}

#endif
