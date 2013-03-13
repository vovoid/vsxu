#include <io.h>
#include <windows.h>
#include <stdio.h>
#include "vsx_dlopen.h"


HMODULE vsx_dlopen::open(const char *filename)
{
  HMODULE winlibrary = LoadLibrary( filename );
  return winlibrary;
}

int  vsx_dlopen::close(HMODULE handle)
{
  return FreeLibrary( handle );
}


char*  vsx_dlopen::error()
{
  DWORD dw = GetLastError();
  char* message = (char*)malloc(sizeof(char)*64);
  sprintf(message, "windows error code %d", dw);
  return message;
}


void* vsx_dlopen::sym(HMODULE handle, const char *symbol)
{
  return GetProcAddress( handle, symbol );
}
