
#include <vsx_platform.h>

#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
  #include "vsx_dlopen_windows.cpp"
#endif

#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
  #include "vsx_dlopen_linux.cpp"
#endif

