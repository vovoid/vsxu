
#ifndef VSX_DLOPEN_H
#define VSX_DLOPEN_H

#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
  typedef HMODULE vsx_dynamic_object_handle;
#endif
#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
  typedef void* vsx_dynamic_object_handle;
#endif


class vsx_dlopen
{
public:
  static vsx_dynamic_object_handle  open(const char *filename);
  static int                        close(vsx_dynamic_object_handle handle);
  static void*                      sym(vsx_dynamic_object_handle handle, const char *symbol);
  static char*                      error();
};

#endif
