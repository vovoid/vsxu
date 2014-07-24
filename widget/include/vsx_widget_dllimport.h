#include <vsx_platform.h>

#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
  #if VSXU_STATIC
    #define WIDGET_DLLIMPORT
  #else
    #if BUILDING_DLL
      # define WIDGET_DLLIMPORT __declspec (dllexport)
    #else /* Not BUILDING_DLL */
      # define WIDGET_DLLIMPORT __declspec (dllimport)
    #endif /* Not BUILDING_DLL */
  #endif
#else
  #define WIDGET_DLLIMPORT
#endif
