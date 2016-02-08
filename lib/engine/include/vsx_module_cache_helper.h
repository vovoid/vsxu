#pragma once

#define cache_check(n) \
  if ( n##_in->get() != n##_cache) { \
    n##_cache = n##_in->get(); \
    return true; \
  }

#define cache_check_arity(n, a) \
  if ( n##_in->get(a) != n##_cache[a]) { \
    n##_cache[a] = n##_in->get(a); \
    return true; \
  }

#define cache_check_f(n, precision) \
  if ( fabs(n##_in->get() - n##_cache) > precision) { \
    n##_cache = n##_in->get(); \
    return true; \
  }

#define cache_check_f_arity(n, a, precision) \
  if ( fabs(n##_in->get(a) - n##_cache[a]) > precision) { \
    n##_cache[a] = n##_in->get(a); \
    return true; \
  }

#define cache_check_float2(n, precision) \
  cache_check_f_arity(n, 0, precision) \
  cache_check_f_arity(n, 1, precision)

#define cache_check_float3(n, precision) \
  cache_check_f_arity(n, 0, precision) \
  cache_check_f_arity(n, 1, precision) \
  cache_check_f_arity(n, 2, precision)

#define cache_check_float4(n, precision) \
  cache_check_f_arity(n, 0, precision) \
  cache_check_f_arity(n, 1, precision) \
  cache_check_f_arity(n, 2, precision) \
  cache_check_f_arity(n, 3, precision)

#define cache_set(n) \
  n##_cache = n##_in->get();

#define cache_set_float2(n) \
  n##_cache[0] = n##_in->get(0); \
  n##_cache[1] = n##_in->get(1);

#define cache_set_float3(n) \
  n##_cache[0] = n##_in->get(0); \
  n##_cache[1] = n##_in->get(1); \
  n##_cache[2] = n##_in->get(2);

#define cache_set_float4(n) \
  n##_cache[0] = n##_in->get(0); \
  n##_cache[1] = n##_in->get(1); \
  n##_cache[2] = n##_in->get(2); \
  n##_cache[3] = n##_in->get(3);
