/**
* Project: VSXu: Realtime modular visual programming language, music/audio visualizer.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Vovoid Media Technologies AB Copyright (C) 2014
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef VSX_PROFILER_H
#define VSX_PROFILER_H

#include <vsx_fifo.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <sys/syscall.h>
#include <sys/stat.h>

#include <inttypes.h>


/*
 * Example use in a thread (works from main thread as well):
 * ---
 * void thread_worker(void* ptr)
 * {
 *
 *   vsx_profiler* profiler = vsx_profiler::get_instance();
 *   profiler->thread_name("main thread");
 *
 *   // set up
 *
 *   profiler->maj_begin();
 *   profiler->sub_begin("calculating stuff");
 *   // do your work here
 *   profiler->sub_end();
 *   profiler->sub_begin("calculating more");
 *     profiler->sub_begin("calculating inner");
 *       // do more complicated work here
 *     profiler->sub_end();
 *   profiler->sub_end();
 *   profiler->maj_end();
 * }
 * ---
 **/

#ifdef __i386
__inline__ uint64_t vsx_profiler_rdtsc()
{
  uint64_t x;
  __asm__ volatile ("rdtsc" : "=A" (x));
  return x;
}
#elif __amd64
inline uint64_t vsx_profiler_rdtsc()
{
  uint64_t a, d;
  __asm__ volatile ("rdtsc" : "=a" (a), "=d" (d));
  return (d<<32) | a;
}
#endif



#define VSX_PROFILE_CHUNK_FLAG_START 0
#define VSX_PROFILE_CHUNK_FLAG_END 1
#define VSX_PROFILE_CHUNK_FLAG_SECTION_START 2
#define VSX_PROFILE_CHUNK_FLAG_SECTION_END 3
#define VSX_PROFILE_CHUNK_FLAG_TIMESTAMP 4  // written by I/O thread at the end of the run to calculate frequency
#define VSX_PROFILE_CHUNK_FLAG_THREAD_NAME 10

#define VSX_PROFILE_CHUNK_FLAG_PLOT_NAME 100 // plot name
#define VSX_PROFILE_CHUNK_FLAG_PLOT_1_DOUBLE 101 // plot 1 double
#define VSX_PROFILE_CHUNK_FLAG_PLOT_2_DOUBLE 102 // plot 2 doubles
#define VSX_PROFILE_CHUNK_FLAG_PLOT_3_DOUBLE 103 // plot 3 doubles
#define VSX_PROFILE_CHUNK_FLAG_PLOT_4_DOUBLE 104 // plot 4 doubles

// Profiler data chunk
// Occupies exactly one cache line
typedef struct
{
 /*8 */ u_int64_t   cycles;     // cpu cycles
 /*8 */ u_int64_t   flags;      // bit-mask
 /*8 */ u_int64_t   id;         // thread or plot/other id
 /*8 */ u_int64_t   spin_waste; // cycles wasted spinning waiting for a full buffer
 /*32*/ char        tag[32];    // text describing the section
} vsx_profile_chunk;


// Profiler Data Logger Class (end-user interface)
class vsx_profiler
{
public:
  vsx_fifo<vsx_profile_chunk,4096> queue;
  pid_t thread_id;



  inline void set_thread_id(pid_t new_id)
  {
    thread_id = new_id;
  }

  /**
   * @brief thread_name Optional thread name
   * @param tag
   */
  inline void thread_name( const char* tag ) __attribute__((always_inline))
  {
    vsx_profile_chunk chunk;
    chunk.flags = VSX_PROFILE_CHUNK_FLAG_THREAD_NAME;
    chunk.spin_waste = 0;
    size_t i;
    for (i = 0; i < 32; i++)
    {
      if (i && tag[i-1] == 0)
        break;
      chunk.tag[i] = tag[i];
    }
    chunk.tag[i-1] = 0;
    chunk.id = thread_id;
    // get dizzy
    while (!queue.produce(chunk)) {}
  }

  /**
   * @brief sub_begin
   * Call this (paired with sub_end) around anything you want to measure.
   *
   * @param tag
   */
  inline void sub_begin( const char* tag ) __attribute__((always_inline))
  {
    vsx_profile_chunk chunk;
    chunk.flags = VSX_PROFILE_CHUNK_FLAG_START;
    chunk.spin_waste = 0;
    size_t i;
    for (i = 0; i < 32; i++)
    {
      if (i && tag[i-1] == 0)
        break;
      chunk.tag[i] = tag[i];
    }
    chunk.tag[i-1] = 0;
    chunk.id = thread_id;
    asm volatile("": : :"memory");
    chunk.cycles = vsx_profiler_rdtsc();
    while (!queue.produce(chunk))
    {
      chunk.spin_waste++;
      // try to get as close to our target code as possible
      chunk.cycles = vsx_profiler_rdtsc();
    }
  }

  inline void sub_end() __attribute__((always_inline))
  {
    uint64_t t = vsx_profiler_rdtsc();
    asm volatile("": : :"memory");
    vsx_profile_chunk chunk;
    chunk.spin_waste = 0;
    chunk.id = thread_id;
    chunk.cycles = t;
    chunk.flags = VSX_PROFILE_CHUNK_FLAG_END;
    while (!queue.produce(chunk))
    {
      chunk.spin_waste++;
    }
  }

  /**
   * @brief maj_begin
   * Call this once per major cycle (graphics frame, sound buffer turnover etc)
   * Should not be nested!
   */
  inline void maj_begin()
  {
    vsx_profile_chunk chunk;
    chunk.spin_waste = 0;
    chunk.id = thread_id;
    chunk.flags = VSX_PROFILE_CHUNK_FLAG_SECTION_START;
    asm volatile("": : :"memory");
    chunk.cycles = vsx_profiler_rdtsc();
    while (!queue.produce(chunk))
    {
      chunk.spin_waste++;
      chunk.cycles = vsx_profiler_rdtsc();
    }
  }

  /**
   * @brief maj_end Call this once per major cycle (graphics frame, sound buffer turnover etc)
   */
  inline void maj_end()
  {
    uint64_t t = vsx_profiler_rdtsc();
    vsx_profile_chunk chunk;
    chunk.spin_waste = 0;
    chunk.id = thread_id;
    chunk.cycles = t;
    chunk.flags = VSX_PROFILE_CHUNK_FLAG_SECTION_END;
    while (!queue.produce(chunk))
    {
      chunk.spin_waste++;
    }
  }

  /**
   * @brief plot_name Optional plot name
   * @param tag
   */
  inline void plot_name(uint64_t id, const char* tag ) __attribute__((always_inline))
  {
    vsx_profile_chunk chunk;
    chunk.flags = VSX_PROFILE_CHUNK_FLAG_THREAD_NAME;
    chunk.spin_waste = 0;
    size_t i;
    for (i = 0; i < 32; i++)
    {
      if (i && tag[i-1] == 0)
        break;
      chunk.tag[i] = tag[i];
    }
    chunk.tag[i-1] = 0;
    chunk.id = id;

    // get dizzy
    while (!queue.produce(chunk)) {}
  }



  inline void plot_1(uint64_t id, double a)
  {
    vsx_profile_chunk chunk;
    memcpy(&chunk.tag[0],&a, sizeof(double));
    chunk.flags = VSX_PROFILE_CHUNK_FLAG_PLOT_1_DOUBLE;
    chunk.id = id;
    chunk.cycles = vsx_profiler_rdtsc();
    while (!queue.produce(chunk))
    {
      chunk.spin_waste++;
      chunk.cycles = vsx_profiler_rdtsc();
    }
  }

  inline void plot_2(uint64_t id, double a, double b)
  {
    vsx_profile_chunk chunk;
    memcpy(&chunk.tag[0],&a, sizeof(double));
    memcpy(&chunk.tag[8],&b, sizeof(double));
    chunk.flags = VSX_PROFILE_CHUNK_FLAG_PLOT_2_DOUBLE;
    chunk.id = id;
    chunk.cycles = vsx_profiler_rdtsc();
    while (!queue.produce(chunk))
    {
      chunk.spin_waste++;
      chunk.cycles = vsx_profiler_rdtsc();
    }
  }

  inline void plot_3(uint64_t id, double a, double b, double c)
  {
    vsx_profile_chunk chunk;
    memcpy(&chunk.tag[0],&a, sizeof(double));
    memcpy(&chunk.tag[8],&b, sizeof(double));
    memcpy(&chunk.tag[16],&c, sizeof(double));
    chunk.flags = VSX_PROFILE_CHUNK_FLAG_PLOT_3_DOUBLE;
    chunk.id = id;
    chunk.cycles = vsx_profiler_rdtsc();
    while (!queue.produce(chunk))
    {
      chunk.spin_waste++;
      chunk.cycles = vsx_profiler_rdtsc();
    }
  }

  inline void plot_4(uint64_t id, double a, double b, double c, double d)
  {
    vsx_profile_chunk chunk;

    memcpy(&chunk.tag[0],&a, sizeof(double));
    memcpy(&chunk.tag[8],&b, sizeof(double));
    memcpy(&chunk.tag[16],&c, sizeof(double));
    memcpy(&chunk.tag[24],&d, sizeof(double));
    chunk.flags = VSX_PROFILE_CHUNK_FLAG_PLOT_4_DOUBLE;
    chunk.id = id;
    chunk.cycles = vsx_profiler_rdtsc();
    while (!queue.produce(chunk))
    {
      chunk.spin_waste++;
      chunk.cycles = vsx_profiler_rdtsc();
    }
  }
};



#endif
