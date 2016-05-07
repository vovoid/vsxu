#pragma once

#include <color/vsx_color.h>
#include <string/vsx_string.h>
#include <filesystem/vsx_filesystem.h>


namespace vsx_color_aux
{
  inline vsx_color<> from_string(vsx_string<char> &val) VSX_ALWAYS_INLINE
  {
    vsx_color<> res;
    vsx_string<>deli = ",";
    vsx_nw_vector< vsx_string<> > pp;
    vsx_string_helper::explode(val,deli,pp);

    if (pp.size() >= 1)
    res.r = vsx_string_helper::s2f(pp[0]);

    if (pp.size() >= 2)
    res.g = vsx_string_helper::s2f(pp[1]);

    if (pp.size() >= 3)
    res.b = vsx_string_helper::s2f(pp[2]);

    if (pp.size() == 4)
    res.a = vsx_string_helper::s2f(pp[3]);

    return res;
  }

  inline vsx_string<>to_string(const vsx_color<> &v, const int num_values = 4) VSX_ALWAYS_INLINE
  {
    vsx_string<>local = vsx_string_helper::f2s(v.r);

    if (num_values > 1)
      local += vsx_string<>(",")+vsx_string_helper::f2s(v.g);

    if (num_values > 2)
      local += vsx_string<>(",")+vsx_string_helper::f2s(v.b);

    if (num_values > 2)
      local += vsx_string<>(",")+vsx_string_helper::f2s(v.a);

    return local;
  }

  inline void dump(const vsx_color<> &v, const char* label="n/a")
  {
    printf( "Name: %s. r: %f b: %f b: %f a: %f\n",label, v.r, v.g, v.b, v.a );
  }
}


