
#include <vsx_platform.h>


#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
  #include <io.h>
#endif

#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
  #include <dlfcn.h>
  #include <syslog.h>
#endif


#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
  #include <stdio.h>
  #include <stdlib.h>
  #include <string>
  #include <sys/stat.h>
  #include <unistd.h>
#endif

#include "vsx_dlopen.h"

void* vsx_dlopen::open(const char *filename)
{
#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
  return LoadLibrary( filename );
#endif
#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
  return dlopen( filename, RTLD_NOW );
#endif
}

int   vsx_dlopen::close(void* handle)
{
#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
  return FreeLibrary( handle );
#endif
#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
  return dlclose( handle );
#endif
}

char*  vsx_dlopen::error()
{
#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
  DWORD dw = GetLastError();
  char* message = (char*)malloc(sizeof(char)*64);
  sprintf(message, "windows error code %d", dw);
  return message;
#endif
#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
  return dlerror();
#endif
}

void*  vsx_dlopen::sym(void *handle, const char *symbol)
{
#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
  return GetProcAddress( handle, symbol );
#endif
#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
  return dlsym( handle, symbol );
#endif

}
