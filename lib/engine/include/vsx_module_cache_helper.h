#pragma once
#define cache_check(n) \
  if ( n##_in->get() != n##_cache) { \
    n##_cache = n##_in->get(); \
    return true; \
  }

#define cache_check_f(n) \
  if ( fabs(n##_in->get() - n##_cache) > 0.001f) { \
    n##_cache = n##_in->get(); \
    return true; \
  }

