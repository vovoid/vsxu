#include <vsx_platform.h>

#if (PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS)

#include <vsx_gl_global.h>
#include "vsx_gl_state.h"

vsx_gl_state* vsx_gl_state::get_instance()
{
  static vsx_gl_state state;
  return &state;
}

#endif
