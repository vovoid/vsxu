#ifndef VSX_REQUIRE_H
#define VSX_REQUIRE_H

#include <debug/vsx_error.h>

#define VSX_REQ_TRUE(t) \
  if (!(t)) \
    return

#define VSX_REQ_TRUE_ERROR(t, error) \
  if (!(t)) \
    VSX_ERROR_RETURN(error)

#define VSX_REQ_TRUE_ERROR_V(t, v, error) \
  if (!(t)) \
    VSX_ERROR_RETURN_V(error, v)

#define VSX_REQ_FALSE(t) \
  if ((t)) \
    return

#define VSX_REQ_TRUE_F(t) \
  if (!(t)) \
    return false

#define VSX_REQ_FALSE_F(t) \
  if ((t)) \
    return false

#define req(t) \
  VSX_REQ_TRUE(t)

#define req_error(t, error) \
  VSX_REQ_TRUE_ERROR(t, error)

#define req_error_v(t, v, error) \
  VSX_REQ_TRUE_ERROR_V(t, v, error)

#endif

