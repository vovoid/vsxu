#ifndef VSX_SINGLETON_COUNTER_H
#define VSX_SINGLETON_COUNTER_H

#include <inttypes.h>

// Atomic, unique counter across all cores
class vsx_singleton_counter
{
  uint64_t counter = 0;

public:

  static uint64_t get()
  {
    static vsx_singleton_counter vsc;
    return __sync_fetch_and_add( &vsc.counter, 1);
  }
};

#endif
