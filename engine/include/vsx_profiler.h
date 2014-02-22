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
#include <vsx_string.h>

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

pid_t gettid( void )
{
  return syscall( __NR_gettid );
}



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

// Profiler data chunk
// Occupies exactly one cache line
class vsx_profile_chunk
{
public:
 /*8 */ uint64_t   cycles;     // cpu cycles
 /*8 */ uint64_t   flags;      // bit-mask
 /*4 */ pid_t      tid;        // thread id
 /*8 */ uint64_t   spin_waste; // cycles wasted spinning waiting for a full buffer
 /*36*/ char       tag[36];    // text describing the section
};


// Profiler Class
class vsx_profiler
{
public:
  vsx_fifo<vsx_profile_chunk,4096> queue;
  pid_t thread_id;

  inline void set_thread_id(pid_t new_id)
  {
    thread_id = new_id;
  }

  inline void sub_begin( const char* tag ) __attribute__((always_inline))
  {
    vsx_profile_chunk chunk;
    chunk.flags = VSX_PROFILE_CHUNK_FLAG_START;
    chunk.spin_waste = 0;
    for (size_t i = 0; i < 44; i++)
    {
      if (i && tag[i-1] == 0)
        break;
      chunk.tag[i] = tag[i];
    }
    chunk.tid = thread_id;
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
    vsx_profile_chunk chunk;
    chunk.spin_waste = 0;
    chunk.tid = thread_id;
    chunk.cycles = t;
    chunk.flags = VSX_PROFILE_CHUNK_FLAG_END;
    while (!queue.produce(chunk))
    {
      chunk.spin_waste++;
    }
  }

  inline void maj_begin() __attribute__((always_inline))
  {
    vsx_profile_chunk chunk;
    chunk.spin_waste = 0;
    chunk.tid = thread_id;
    chunk.flags = VSX_PROFILE_CHUNK_FLAG_SECTION_START;
    asm volatile("": : :"memory");
    chunk.cycles = vsx_profiler_rdtsc();
    while (!queue.produce(chunk))
    {
      chunk.spin_waste++;
      chunk.cycles = vsx_profiler_rdtsc();
    }
  }

  inline void maj_end() __attribute__((always_inline))
  {
    uint64_t t = vsx_profiler_rdtsc();
    vsx_profile_chunk chunk;
    chunk.spin_waste = 0;
    chunk.tid = thread_id;
    chunk.cycles = t;
    chunk.flags = VSX_PROFILE_CHUNK_FLAG_SECTION_END;
    while (!queue.produce(chunk))
    {
      chunk.spin_waste++;
    }
  }
};

#define RECIEVE_BUFFER_PAGES 32
#define RECIEVE_BUFFER_ITEMS 32

class vsx_profiler_manager
{
public:

  volatile __attribute__((aligned(64))) int64_t run_threads;

  pthread_mutex_t profiler_thread_lock;
  pthread_t consumer_pthread;
  pthread_t io_pthread;
  vsx_profiler profiler_list[VSX_PROFILER_MAX_THREADS];
  pid_t thread_list [VSX_PROFILER_MAX_THREADS];

  vsx_fifo<vsx_profile_chunk*,RECIEVE_BUFFER_PAGES> io_pool;

  vsx_profiler_manager()
  {
    run_threads = 1;

    vsx_printf("vsx_profiler_manager constructor\n");
    pthread_mutex_init(&profiler_thread_lock, NULL);

    for (size_t i = 0; i < VSX_PROFILER_MAX_THREADS; i++)
    {
      thread_list[i] = 0;
    }

    vsx_printf("starting consumer thread\n");

    pthread_create(
      &consumer_pthread,
      NULL,
      vsx_profiler_manager::consumer_thread,
      NULL
    );

    vsx_printf("starting io thread\n");

    pthread_create(
      &io_pthread,
      NULL,
      vsx_profiler_manager::io_thread,
      NULL
    );
  }

  ~vsx_profiler_manager()
  {
    vsx_printf("setting run threads to false in profiler manager\n");
    run_threads = false;
    pthread_join(io_pthread, NULL);
    pthread_join(consumer_pthread, NULL);
  }


  static void* io_thread(void* arg)
  {
    VSX_UNUSED(arg);

    vsx_profiler_manager* pm = vsx_profiler_manager::get_instance();

    FILE* fp = fopen("/tmp/vsx_profile.dat", "wb");
    while ( __sync_fetch_and_add( &pm->run_threads, 0) )
    {
      vsx_profile_chunk* r;
      while ( pm->io_pool.consume(r) )
      {
        fwrite(r,sizeof(vsx_profile_chunk)*RECIEVE_BUFFER_ITEMS,1,fp);
      }
    }
    fclose(fp);
    pthread_exit(0);
    return NULL;
  }

  static void* consumer_thread(void* profiler)
  {
    VSX_UNUSED(profiler);
    vsx_profiler_manager* pm = vsx_profiler_manager::get_instance();

    vsx_profiler* profilers = &pm->profiler_list[0];
    pid_t* producer_threads = &pm->thread_list[0];

    vsx_profile_chunk recieve_buffer[RECIEVE_BUFFER_PAGES][RECIEVE_BUFFER_ITEMS];
    size_t recieve_buffer_iterator = 0;

    size_t current_buffer_page = 0;

    while ( __sync_fetch_and_add( &pm->run_threads, 0) )
    {
      // collect from all threads
      for ( size_t i = 0; i < VSX_PROFILER_MAX_THREADS; i++)
      {
        if (producer_threads[i] == 0)
          break;

        // write the data to disk
        size_t max_iterations = 0;
        while (
          profilers[i].queue.consume( recieve_buffer[current_buffer_page][recieve_buffer_iterator] )
          &&
          max_iterations++ < 4
        )
        {
          recieve_buffer_iterator++;

          if (recieve_buffer_iterator != RECIEVE_BUFFER_ITEMS)
            continue;

          // send to io
          pm->io_pool.produce(&recieve_buffer[current_buffer_page][0]);

          recieve_buffer_iterator = 0;
          current_buffer_page++;

          if (current_buffer_page == RECIEVE_BUFFER_PAGES)
            current_buffer_page = 0;
        }
      }
    }
    pthread_exit(0);
    return 0x0;
  }


  // Call this once per thread
  // - make sure to call it from inside the thread
  //
  // It does lookups per thread to give you the correct pointer
  vsx_profiler* get_profiler()
  {
    // identify thread, if reaching 0 = first time, cache the value
    pid_t local_thread_id = gettid();

    pthread_mutex_lock(&profiler_thread_lock);
    size_t i = 0;
    for (i = 0; i < VSX_PROFILER_MAX_THREADS; i++)
    {
      if (thread_list[i] != 0 && thread_list[i] != local_thread_id)
        continue;
      if (thread_list[i] == local_thread_id)
      {
        pthread_mutex_unlock(&profiler_thread_lock);
        return &profiler_list[i];
      }
      if (thread_list[i] == 0)
        break;
    }

    thread_list[i] = local_thread_id;
    profiler_list[i].set_thread_id( local_thread_id );

    pthread_mutex_unlock(&profiler_thread_lock);
    return &profiler_list[i];
  }

  // Call this when exiting a thread.
  // Ensures the profiler slot can be re-used.
  void recycle_instance( vsx_profiler* p )
  {
    VSX_UNUSED(p);
    // find the pointer and set the corresponding thread id to 0
  }

  static vsx_profiler_manager* get_instance()
  {
    static vsx_profiler_manager pm;
    return &pm;
  }

};


#endif
