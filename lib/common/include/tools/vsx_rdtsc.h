#pragma once

#ifdef __GNUC__

#ifdef __i386
__inline__ uint64_t vsx_rdtsc()
{
  uint64_t x;
  __asm__ volatile ("rdtsc" : "=A" (x));
  return x;
}
#elif __amd64
inline uint64_t vsx_rdtsc()
{
  uint64_t a, d;
  __asm__ volatile ("rdtsc" : "=a" (a), "=d" (d));
  return (d<<32) | a;
}
#endif
#else

#define vsx_rdtsc __rdtsc

#endif