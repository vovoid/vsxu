#include <vsx_platform.h>

#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
  #if BUILDING_DLL
    # define WIDGET_DLLIMPORT __declspec (dllexport)
  #else /* Not BUILDING_DLL */
    # define WIDGET_DLLIMPORT __declspec (dllimport)
  #endif /* Not BUILDING_DLL */
#else
  #define WIDGET_DLLIMPORT
#endif
