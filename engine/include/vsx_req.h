#ifndef VSX_REQUIRE_H
#define VSX_REQUIRE_H

#define VSX_REQ_TRUE(t) \
  if (!(t)) \
    return

#define VSX_REQ_FALSE(t) \
  if ((t)) \
    return

#define VSX_REQ_TRUE_F(t) \
  if (!(t)) \
    return false

#define VSX_REQ_FALSE_F(t) \
  if ((t)) \
    return false

#endif

