#pragma once

#include <atomic>

// Atomic, unique counter across all cores
class vsx_singleton_counter
{
  std::atomic_uint_fast64_t counter;

public:

  vsx_singleton_counter()
  {
    counter = 0;
  }

  static uint64_t get()
  {
    static vsx_singleton_counter vsc;
    return vsc.counter.fetch_add(1);
  }
};
