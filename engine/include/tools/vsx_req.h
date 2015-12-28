#pragma once

#define VSX_REQ_TRUE(t) \
  if (!(t)) \
    return

#define VSX_REQ_TRUE_V(t, v) \
  if (!(t)) \
    return v

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

#define ret(t) \
  return (void)(t)

#define req_v(t, v) \
  VSX_REQ_TRUE_V(t, v)

#define req_continue(t) \
  if (!(t)) \
    continue

