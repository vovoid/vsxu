#pragma once

#include <tools/vsx_singleton.h>
#include <stdio.h>

class vsx_printf_file_holder
  : public vsx::singleton<vsx_printf_file_holder>
{
  FILE* fp = 0x0;

public:

  vsx_printf_file_holder()
  {
  }

  ~vsx_printf_file_holder()
  {
    if (fp)
      fclose(fp);
  }

  FILE* get_fp()
  {
    return fp;
  }

  void output_to_file(const char* filename)
  {
    fp = fopen(filename, "w");
  }

  void flush()
  {
    fflush(fp);
  }
};

#define vsx_printf(...) \
{ \
  wprintf(__VA_ARGS__); \
  fflush(stdout); \
  if (vsx_printf_file_holder::get()->get_fp()) { \
    fwprintf(vsx_printf_file_holder::get()->get_fp(), __VA_ARGS__); \
    vsx_printf_file_holder::get()->flush(); \
  } \
}
