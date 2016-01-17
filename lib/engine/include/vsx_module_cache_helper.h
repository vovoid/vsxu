#pragma once
#define cache_check(n) \
  if ( n##_in->get() != n##_cache) { \
    n##_cache = n##_in->get(); \
    return true; \
  }

#define cache_check_f(n, precision) \
  if ( fabs(n##_in->get() - n##_cache) > precision) { \
    n##_cache = n##_in->get(); \
    return true; \
  }

