#pragma once

#include <vsx_platform.h>

#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
#include "vsx_filesystem_mmap_windows.h"
#endif

#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
#include "vsx_filesystem_mmap_linux.h"
#endif
