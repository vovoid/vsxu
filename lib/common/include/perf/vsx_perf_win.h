#pragma once


#include <tools/vsx_rdtsc.h>
#include "windows.h"
#include "psapi.h"

class vsx_perf
{
  uint64_t rdtsc_prev = 0;
public:

  // cache misses
  void cache_misses_start()
  {
  }

  void cache_misses_begin()
  {
  }

  long long cache_misses_end()
  {
    return 0;
  }

  void cache_misses_stop()
  {
  }

  // cpu instructions
  void cpu_instructions_start()
  {
  }

  void cpu_instructions_begin()
  {
    rdtsc_prev = vsx_rdtsc();
  }

  long long cpu_instructions_end()
  {
    return vsx_rdtsc() - rdtsc_prev;
  }

  void cpu_instructions_stop()
  {
  }

  int memory_currently_used()
  {
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    return (int)pmc.WorkingSetSize / (1024 * 1024);
  }

};
