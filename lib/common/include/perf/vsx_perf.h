#pragma once

#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
  #include "vsx_perf_win.h"
#else
  #include "vsx_perf_linux.h"
#endif
