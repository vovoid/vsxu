#ifndef VSX_VECTOR_2_HELPER_H
#define VSX_VECTOR_2_HELPER_H


// helper functions for vsx_vector

#include <math/vector/vsx_vector3.h>
#include <string/vsx_string.h>
#include <string/vsx_string_helper.h>
#include <filesystem/vsx_filesystem.h>

namespace vsx_vector2_helper
{
  template<typename T>
  inline vsx_vector2<T> from_string(vsx_string<>&val) VSX_ALWAYS_INLINE
  {
    vsx_vector2<T> res;
    vsx_string<> deli = ",";
    vsx_nw_vector< vsx_string<> > pp;
    vsx_string_helper::explode(val,deli,pp);
    if (pp.size() >= 1)
    res.x = vsx_string_helper::s2f(pp[0]);
    if (pp.size() >= 2)
    res.y = vsx_string_helper::s2f(pp[1]);
    return res;
  }

  template<typename T>
  inline vsx_string<>to_string(const vsx_vector2<T> &v, const int num_values = 2) VSX_ALWAYS_INLINE
  {
    vsx_string<>local = vsx_string_helper::f2s((float)v.x);

    if (num_values > 1)
      local += vsx_string<>(",")+vsx_string_helper::f2s((float)v.y);

    return local;
  }

  template<typename T>
  inline void dump(const vsx_vector2<T> &v, char* label="n/a")
  {
    vsx_printf( L"Name: %s. x: %f y: %f\n",label, v.x, v.y );
  }

}


#endif
