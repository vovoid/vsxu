#pragma once

#define VSX_REQ_TRUE(t) \
  if (!(t)) \
    return

#define VSX_REQ_TRUE_V(t, v) \
  if (!(t)) \
    return v

#define VSX_REQ_FALSE_V(t, v) \
  if ((t)) \
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

#define req(t) VSX_REQ_TRUE(t)
#define reqrv(t, v) VSX_REQ_TRUE_V(t, v)
#define reqrf(t) VSX_REQ_TRUE_V(t, false)
#define freq(t) VSX_REQ_FALSE(t)
#define freqrv(t, v) VSX_REQ_FALSE_V(t, v)
#define freqrf(t) VSX_REQ_FALSE_V(t, false)



#define ret(t) return (void)(t)

#define req_continue(t) \
  if (!(t)) \
    continue

#define req_break(t) \
  if (!(t)) \
    break
