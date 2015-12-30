#pragma once

#include <debug/vsx_error.h>
#include "vsx_req.h"

#define VSX_REQ_TRUE_ERROR(t, error) \
  if (!(t)) \
    VSX_ERROR_RETURN(error)

#define VSX_REQ_TRUE_ERROR_V(t, error, v) \
  if (!(t)) \
    VSX_ERROR_RETURN_V(error, v)


#define req_error(t, error) \
  VSX_REQ_TRUE_ERROR(t, error)

#define req_error_goto(t, error, handle) \
  if (!(t)) \
  { \
    VSX_ERROR_INFO(error) \
    goto handle; \
  }

#define req_error_v(t, error, v) \
  VSX_REQ_TRUE_ERROR_V(t, error, v)
