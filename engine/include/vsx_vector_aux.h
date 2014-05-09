#ifndef VSX_VECTOR_AUX_H
#define VSX_VECTOR_AUX_H


// helper functions for vsx_vector

#include <vsx_vector.h>
#include <vsx_string.h>
#include <vsxfst.h>

namespace vsx_vector_aux
{
  template<typename T>
  inline vsx_vector<T> from_string(vsx_string &val) VSX_ALWAYS_INLINE
  {
    vsx_vector<T> res;
    vsx_string deli = ",";
    vsx_avector<vsx_string> pp;
    explode(val,deli,pp);
    if (pp.size() >= 1)
    res.x = s2f(pp[0]);
    if (pp.size() >= 2)
    res.y = s2f(pp[1]);
    if (pp.size() == 3)
    res.z = s2f(pp[2]);
    return res;
  }

  template<typename T>
  inline vsx_string to_string(const vsx_vector<T> &v, const int num_values = 3) VSX_ALWAYS_INLINE
  {
    vsx_string local = f2s(v.x);

    if (num_values > 1)
      local += vsx_string(",")+f2s(v.y);

    if (num_values > 2)
      local += vsx_string(",")+f2s(v.z);

    return local;
  }

  template<typename T>
  inline void dump(const vsx_vector<T> &v, char* label="n/a")
  {
    vsx_printf( "Name: %s. x: %f y: %f z: %f\n",label, v.x, v.y, v.z );
  }

}


#endif
