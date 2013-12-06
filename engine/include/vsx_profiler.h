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

// You can set VSX_PROFILER_MAX_THREADS in your make script as well

#ifndef VSX_PROFILER_MAX_THREADS
#define VSX_PROFILER_MAX_THREADS 8
#endif

// VSXu Profiler:
/*
 * Example use in a thread (works from main thread as well):
 * ---
 * void thread_worker(void* ptr)
 * {
 *   vsx_profiler* profiler = vsx_profiler::get_instance();
 *   // set up
 *   profiler->begin("calculating stuff");
 *   //
 *   // do your work here
 *   //
 *   profiler->end();
 * }
 * ---
 **/




#ifdef __i386
__inline__ uint64_t vsx_profiler_rdtsc() __attribute__((always_inline))
{
  uint64_t x;
  __asm__ volatile ("rdtsc" : "=A" (x));
  return x;
}
#elif __amd64
__inline__ uint64_t vsx_profiler_rdtsc() __attribute__((always_inline))
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
 /*4 */ pid_t      tid;        // thread id
 /*44*/ char       tag[44];    // text describing the section
};


// Profiler Class
class vsx_profiler
{
  vsx_fifo<vsx_profile_chunk,4096> queue;

  vsx_profiler* get_profilers()
  {
    static vsx_profiler profiler_list[VSX_PROFILER_MAX_THREADS];
    return &profiler_list[0];
  }

  pid_t thread_id;

public:

  inline void set_thread_id(pid_t new_id)
  {
    thread_id = new_id;
  }

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
    chunk.tid = thread_id;
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
      // collect from all threads
      for ( size_t i = 0; i < VSX_PROFILER_MAX_THREADS; i++)
      {
        // write the data to disk

      }
      // Flush to disk
    }
  }



  // Call this once per thread
  // - make sure to call it from inside the thread
  //
  // It does lookups per thread to give you the correct pointer
  static vsx_profiler* get_instance()
  {
    static pid_t thread_list [VSX_PROFILER_MAX_THREADS] = {0,0,0,0,0,0,0,0};
    vsx_profiler* profilers = get_profilers();

    // identify thread, if reaching 0 = first time, cache the value
    pid_t local_thread_id = gettid();

    // is consumer thread running?
    //    if not: start up consumer thread
  }

  // Call this when exiting a thread.
  // Ensures the profiler slot can be re-used.
  void recycle_instance( vsx_profiler* p )
  {
    // find the pointer and set the corresponding thread id to 0
  }

};



#endif
