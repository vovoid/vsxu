/**
* Project: VSXu Engine: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu Engine.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Lesser General Public License (LGPL)
*
* VSXu Engine is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU Lesser General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef VSX_PROFILER_H
#define VSX_PROFILER_H

#include <vsx_fifo.h>
#include <sys/types.h>

// configuring options
#define VSX_PROFILER_MAX_THREADS 8

// -------------------



#ifdef __i386
__inline__ uint64_t vsx_profiler_rdtsc()
{
  uint64_t x;
  __asm__ volatile ("rdtsc" : "=A" (x));
  return x;
}
#elif __amd64
__inline__ uint64_t vsx_profiler_rdtsc()
{
  uint64_t a, d;
  __asm__ volatile ("rdtsc" : "=a" (a), "=d" (d));
  return (d<<32) | a;
}
#endif




#define VSX_PROFILE_CHUNK_FLAG_END 1


// Profiler data chunk
// Occupies exactly one cache line
class vsx_profile_chunk
{
public:
 /*8 */ uint64_t   rdtsc_cycles;       // cpu cycles
 /*8 */ uint64_t   flags;      // bit-mask
 /*4 */ pid_t      tid;
 /*44*/ char       tag[44];    // text describing the section
};


// Profiler Class
class vsx_profiler
{
  vsx_fifo<vsx_profile_chunk,4096> queue;
public:

  inline void begin( const char* tag ) __attribute__((always_inline))
  {
    vsx_profile_chunk chunk;
    chunk.flags = 0;
    for (size_t i = 0; i < 44; i++)
    {
      if (tag[i-1] == 0)
        break;
      chunk.tag[i] = tag[i];
    }
    chunk.tid = gettid();
    chunk.rdtsc_cycles = vsx_profiler_rdtsc();
    while (!queue.produce(chunk))
    {
      // try to get as close to our target code as possible
      chunk.rdtsc_cycles = vsx_profiler_rdtsc();
    }
  }

  inline void end() __attribute__((always_inline))
  {
    uint64_t t = vsx_profiler_rdtsc();
    vsx_profile_chunk chunk;
    chunk.tid = gettid();
    chunk.rdtsc_cycles = t;
    chunk.flags = VSX_PROFILE_CHUNK_FLAG_END;
    while (!queue.produce(chunk))
    {
    }
  }

  static void* consumer_thread(void* profiler)
  {
    vsx_profiler* my_profiler = (vsx_profiler*) profiler;
    vsx_profile_chunk recieve_buffer[16];
    while (1)
    {
      // collect, write to disk
    }
  }

  // do not call this often, cache the value!
  // it does lookups per thread to give you the correct
  static vsx_profiler* get_instance()
  {
    static pid_t thread_list [VSX_PROFILER_MAX_THREADS] = {0,0,0,0,0,0,0,0};
    static vsx_profiler profiler_list[VSX_PROFILER_MAX_THREADS];
    // identify thread, if reaching 0 = first time
    // first time: start up consumer thread
  }

};



#endif
