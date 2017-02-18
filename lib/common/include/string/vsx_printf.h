
#pragma once

#ifdef VSX_PRINTF_TO_FILE

class vsx_pf_file_holder
{
  FILE* fp;
public:
  vsx_pf_file_holder()
    :
      fp(0x0)
  {
    fp = fopen( (vsx_string<>("debug")+ DIRECTORY_SEPARATOR +vsx_string<>("debug_log.txt")).c_str(), "w");
  }

  ~vsx_pf_file_holder()
  {
    if (fp)
      fclose(fp);
  }

  FILE* get_fp()
  {
    return fp;
  }

  void flush()
  {
    fflush(fp);
  }

public:
  static vsx_pf_file_holder* get_instance()
  {
    static vsx_pf_file_holder instance;
    return &instance;
  }
};


#define vsx_printf(...) \
  fwprintf(vsx_pf_file_holder::get_instance()->get_fp(), __VA_ARGS__); \
  vsx_pf_file_holder::get_instance()->flush();

#else

  #define vsx_printf(...) \
    wprintf(__VA_ARGS__); \
    fflush(stdout)

  #endif
#
