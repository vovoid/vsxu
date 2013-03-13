#include <dlfcn.h>
#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include "vsx_dlopen.h"



void* vsx_dlopen::open(const char *filename)
{
  return dlopen( filename, RTLD_NOW );
}


  int  vsx_dlopen::close(void* handle)
{
  return dlclose( handle );
}


char*  vsx_dlopen::error()
{
  return dlerror();
}


void*  vsx_dlopen::sym(void *handle, const char *symbol)
{
  return dlsym( handle, symbol );
}

