#include <vsx_platform.h>

#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
  #if VSXU_STATIC
    #define VSX_ENGINE_GRAPHICS_DLLIMPORT
  #else
    #if BUILDING_DLL
      # define VSX_ENGINE_GRAPHICS_DLLIMPORT __declspec (dllexport)
    #else /* Not BUILDING_DLL */
      # define VSX_ENGINE_GRAPHICS_DLLIMPORT __declspec (dllimport)
    #endif /* Not BUILDING_DLL */
  #endif
#else
  #define VSX_ENGINE_GRAPHICS_DLLIMPORT
#endif

