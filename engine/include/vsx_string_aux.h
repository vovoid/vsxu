#ifndef VSX_STRING_AUX_H
#define VSX_STRING_AUX_H

// helper functions for vsx_string

#include <stdlib.h>
#include <vsx_string.h>

class vsx_string_aux
{
public:

  static inline int s2i(const vsx_string& in) VSX_ALWAYS_INLINE
  {
    return atoi( in.c_str() );
  }

};



#endif
