#pragma once

#include <vsx_platform.h>

#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
  #if VSXU_STATIC
    #define COMMON_DLLIMPORT
  #else
    #if BUILDING_DLL
      # define COMMON_DLLIMPORT __declspec (dllexport)
    #else /* Not BUILDING_DLL */
      # define COMMON_DLLIMPORT __declspec (dllimport)
    #endif /* Not BUILDING_DLL */
  #endif
#else
  #define COMMON_DLLIMPORT
#endif

