#ifndef VSX_ENGINE_DLLIMPORT_H
#define VSX_ENGINE_DLLIMPORT_H

#include <vsx_platform.h>

#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
  #if VSXU_STATIC
    #define ENGINE_DLLIMPORT
  #else
    #if BUILDING_DLL
      # define ENGINE_DLLIMPORT __declspec (dllexport)
    #else /* Not BUILDING_DLL */
      # define ENGINE_DLLIMPORT __declspec (dllimport)
    #endif /* Not BUILDING_DLL */
  #endif
#else
  #define ENGINE_DLLIMPORT
#endif

#endif
