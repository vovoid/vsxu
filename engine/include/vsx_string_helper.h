#ifndef VSX_STRING_HELPER_H
#define VSX_STRING_HELPER_H

// helper functions for vsx_string

#include <stdlib.h>
#include <vsx_string.h>

namespace vsx_string_helper
{
  inline int s2i(const vsx_string<>& in) VSX_ALWAYS_INLINE
  {
    return atoi( in.c_str() );
  }

  inline vsx_string<>i2s(int in)
  {
    char string_res[256] = "";
    sprintf(string_res,"%d",in);
    return vsx_string<>(string_res);
  }

  inline float s2f(const vsx_string<>& in)
  {
    return atof(in.c_str());
  }

  inline vsx_string<>f2s(float in)
  {
    char string_res[256] = "";
    sprintf(string_res,"%f",in);
    return vsx_string<>(string_res);
  }

  inline vsx_string<>f2s(float in, int decimals)
  {
    char string_res[256] = "";
    sprintf(string_res,vsx_string<>("%."+vsx_string_helper::i2s(decimals)+"f").c_str(),in);
    return vsx_string<>(string_res);
  }

  inline void write_to_file(vsx_string<>filename, vsx_string<>* payload)
  {
    FILE* fp = fopen(filename.c_str(), "w");
    if (!fp)
      return;
    fputs(payload->c_str(), fp);
    fclose(fp);
  }
}



#endif
