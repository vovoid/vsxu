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

/*
 * VSX Profiler Manager
 *
 * This is the wrapper class which you can use to implement the profiler in your program.
 *
 * Defines:
 *   VSX_PROFILER_MAX_THREADS
 *     Default: undefined
 *
 *     With this you can increase or decrease the number of threads supported by the profiler.
 *     Make sure this is more or equal to your peak thread count in the program in which you
 *     make calls to the profiler.
 *
 *     By default, profiler uses VSXu engine's directory structure to place profiler files in
 *     the user's "vsxu/{version}/data/profiles/" directory. If you want this someplace else,
 *     set this define to your directory of choice.
 */

#pragma once

#include <thread>
#include <errno.h>
#include "vsx_profiler.h"
#include <vsx_data_path.h>
#include <time/vsx_timer.h>
#include <string/vsx_string_helper.h>
#include <debug/vsx_error.h>

#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS && COMPILER == COMPILER_VISUAL_STUDIO
#include <io.h>
#endif

#if (PLATFORM == PLATFORM_LINUX)
  #include <sys/prctl.h>
#endif

// You can set VSX_PROFILER_MAX_THREADS in your make script as well

#ifndef VSX_PROFILER_MAX_THREADS
  #define VSX_PROFILER_MAX_THREADS 8
#endif

#define VSX_PROFILER_RECIEVE_BUFFER_PAGES 32
#define VSX_PROFILER_RECIEVE_BUFFER_ITEMS 32
#define VSX_PROFILER_STACK_DEPTH_WARNING 64

class vsx_profiler_manager
{
public:

  std::atomic_uint_fast64_t thread_run_control;
  std::atomic_uint_fast64_t enable_data_collection;

  bool started = false;

  vsx_string<> output_path;

  vsx_lock thread_lock;
  std::thread consumer_thread;
  std::thread io_thread;

  vsx_profiler profiler_list[VSX_PROFILER_MAX_THREADS];
  pid_t thread_list [VSX_PROFILER_MAX_THREADS];

  vsx_fifo_mt<vsx_profile_chunk*,VSX_PROFILER_RECIEVE_BUFFER_PAGES> io_pool;

  vsx_profiler_manager()
  {
    thread_run_control = 1;
    enable_data_collection = 0;
  }

  ~vsx_profiler_manager()
  {

    if (thread_run_control)
    {
      vsx_printf(L"VSX PROFILER:  Shutting down:");
      thread_run_control.fetch_sub(1);

      vsx_printf(L"[io thread] ");
      if (io_thread.joinable())
        io_thread.join();

      vsx_printf(L"[consumer thread] ");
      if (consumer_thread.joinable())
        consumer_thread.join();
      vsx_printf(L"[destruction complete]\n");
    }
  }

  void start()
  {
    vsx_printf(L"VSX PROFILER:\n  Creating: ");

    vsx_printf(L"[consumer thread]\n");

    consumer_thread = std::thread(vsx_profiler_manager::consumer_worker);

    vsx_printf(L"[io thread]\n");

    io_thread = std::thread(vsx_profiler_manager::io_worker);

    for (size_t i = 0; i < VSX_PROFILER_MAX_THREADS; i++)
      profiler_list[i].enabled = true;
  }

  static vsx_string<> profiler_directory_get()
  {
    if (vsx_profiler_manager::get_instance()->output_path.size())
      return vsx_profiler_manager::get_instance()->output_path;
    return vsx_data_path::get_instance()->data_path_get()+"profiler";
  }

  void enable()
  {
    req(!enable_data_collection.load());

    vsx_printf(L"VSX PROFILER:  request consumer to data collection...\n");
    enable_data_collection.fetch_add(1);
  }

  void disable()
  {
    req(enable_data_collection.load());

    vsx_printf(L"VSX PROFILER:  request consumer to disable data collection...\n");
    enable_data_collection.fetch_sub(1);
  }

  static void* io_worker()
  {
    vsx_printf(L"[Profiler IO running]\n");

    #if (PLATFORM == PLATFORM_LINUX)
      const char* cal = "profiler::io_thread";
      prctl(PR_SET_NAME,cal);
    #endif

    vsx_profiler_manager* pm = vsx_profiler_manager::get_instance();

    vsx_string<>profiler_directory = profiler_directory_get();

#if COMPILER == COMPILER_VISUAL_STUDIO
    if (_access(profiler_directory.c_str(), 0) != 0)
#else
    if (access(profiler_directory.c_str(), 0) != 0)
#endif
#if (PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX)
      mkdir( (profiler_directory).c_str(), 0700);
#else
      CreateDirectory( (profiler_directory).c_str(), nullptr );
#endif

#if (PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX)
    vsx_string<>filename = profiler_directory + "/" + vsx_string<>(program_invocation_short_name) +
        "_" +vsx_string_helper::i2s((int)time(0x0)) + ".dat";
#else
    vsx_string<>filename = profiler_directory + "/" + "_" +vsx_string_helper::i2s((int)time(0x0)) + ".dat";
#endif
    vsx_timer timer;

    FILE* fp = fopen( filename.c_str() , "wb");

    if (!fp)
      VSX_ERROR_EXIT("VSX PROFILER: ***ERROR*** I/O thread can not open file. Aborting...", 900);


    vsx_printf(L"[Profiler writing to %hs]\n", filename.c_str());
    timer.start();
    double accumulated_time = 0.0;
    while ( pm->thread_run_control.load() )
    {
      vsx_profile_chunk* r;
      while ( pm->io_pool.consume(r) )
      {
        fwrite(r,sizeof(vsx_profile_chunk) * VSX_PROFILER_RECIEVE_BUFFER_ITEMS,1,fp);
      }
      double d1 = timer.dtime();
      if (pm->enable_data_collection)
        accumulated_time += d1;
    }

    vsx_profile_chunk c;
    c.cycles = vsx_rdtsc();
    c.flags = VSX_PROFILE_CHUNK_FLAG_TIMESTAMP;
    memset(c.tag,0,32);
    sprintf(c.tag,"%f", accumulated_time );
    fwrite(&c,sizeof(vsx_profile_chunk),1,fp);

    fclose(fp);
    return NULL;
  }

  static void* consumer_worker()
  {
    vsx_printf(L"[Profiler CONSUMER running]\n");

    #if (PLATFORM == PLATFORM_LINUX)
      const char* cal = "profiler::manager";
      prctl(PR_SET_NAME,cal);
    #endif

    vsx_profiler_manager* pm = vsx_profiler_manager::get_instance();

    vsx_profiler* profilers = &pm->profiler_list[0];
    pid_t* producer_threads = &pm->thread_list[0];

    vsx_profile_chunk recieve_buffer[VSX_PROFILER_RECIEVE_BUFFER_PAGES][VSX_PROFILER_RECIEVE_BUFFER_ITEMS];
    size_t recieve_buffer_iterator = 0;

    size_t current_buffer_page = 0;

    int64_t current_depth = 0;
    int64_t max_depth = 0;

    uint64_t current_enabled = pm->enable_data_collection.load();

    while ( pm->thread_run_control.load() )
    {
      // collect from all threads
      for ( size_t i = 0; i < VSX_PROFILER_MAX_THREADS; i++)
      {
        if (producer_threads[i] == 0)
          break;

        vsx_profile_chunk recieve_chunk;

        // write the data to disk
        size_t max_iterations = 0;
        forever
        {
          if (!profilers[i].queue.consume( recieve_chunk ))
            break;

          if (
              recieve_chunk.flags == VSX_PROFILE_CHUNK_FLAG_SECTION_START
              ||
              recieve_chunk.flags == VSX_PROFILE_CHUNK_FLAG_START
              )
          {
            current_depth++;
            if (current_depth > max_depth)
            {
              max_depth = current_depth;
              vsx_printf(L"VSX PROFILER:  Stack depth new maximum: %zu\n", max_depth);
            }
            if (current_depth > VSX_PROFILER_STACK_DEPTH_WARNING)
              vsx_printf(L"VSX PROFILER:  ***WARNING*** Stack depth: %d\n", VSX_PROFILER_STACK_DEPTH_WARNING);
          }

          if (
              recieve_chunk.flags == VSX_PROFILE_CHUNK_FLAG_SECTION_END
              ||
              recieve_chunk.flags == VSX_PROFILE_CHUNK_FLAG_END
              )
          {
            if ( !current_depth )
              vsx_printf(L"VSX PROFILER:  ***WARNING*** Got stack depth below zero. Fix your code!\n");
            current_depth--;
          }

          // handle enabling switch, only toggle when at zero depth
          if (
              current_enabled != pm->enable_data_collection.load()
              &&
              !current_depth
              )
          {
            current_enabled = pm->enable_data_collection.load();
            if (current_enabled)
            {
              vsx_printf(L"VSX PROFILER:  [enabled data collector]\n");
            }
            else
            {
              vsx_printf(L"VSX PROFILER:  [disabled data collector]\n");
            }

            continue;
          }

          if (current_enabled)
          {
            recieve_buffer[current_buffer_page][recieve_buffer_iterator] = recieve_chunk;

            recieve_buffer_iterator++;

            if (recieve_buffer_iterator == VSX_PROFILER_RECIEVE_BUFFER_ITEMS)
            {
              recieve_buffer_iterator = 0;

              // send to io
              while (!pm->io_pool.produce(&recieve_buffer[current_buffer_page][0]))
              {
                vsx_printf(L"VSX PROFILER:  ***PERFORMANCE WARNING*** spinning while i/o fifo is full...\n");
              }

              current_buffer_page++;

              if (current_buffer_page == VSX_PROFILER_RECIEVE_BUFFER_PAGES)
                current_buffer_page = 0;
            }
          }

          // read max 4 packets per thread
          if ( max_iterations++ > 4)
            break;
        }
      }
    }
    return 0x0;
  }


  static pid_t gettid( void )
  {
#if (PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS)
    return GetCurrentThreadId();
#else
    return (pid_t)syscall( __NR_gettid );
#endif
  }

  // Call this once per thread
  // - make sure to call it from inside the thread
  //
  // It does lookups per thread to give you the correct pointer
  void init_profiler()
  {
    // identify thread, if reaching 0 = first time, cache the value
    pid_t local_thread_id = gettid();

    thread_lock.aquire();
    size_t i = 0;
    for (i = 0; i < VSX_PROFILER_MAX_THREADS; i++)
    {
      if (thread_list[i] != 0 && thread_list[i] != local_thread_id)
        continue;

      if (thread_list[i] == local_thread_id)
      {
        thread_lock.release();
        vsx_printf(L"VSX PROFILER: ***WARNING*** Profiler already initialized for thread id %d. Did you call init() twice by mistake?\n",local_thread_id);
        return;
      }

      if (thread_list[i] == 0)
        break;
    }

    thread_list[i] = local_thread_id;
    profiler_list[i].set_thread_id( local_thread_id );

    thread_lock.release();
  }

  // Call as many times per thread as needed to get local pointers
  // However try to minimize it.
  vsx_profiler* get_profiler(bool try_to_initialize = true)
  {
    // identify thread, if reaching 0 = first time, cache the value
    pid_t local_thread_id = gettid();

    size_t i = 0;
    for (i = 0; i < VSX_PROFILER_MAX_THREADS; i++)
    {
      if (thread_list[i] != 0 && thread_list[i] != local_thread_id)
        continue;

      if (thread_list[i] == local_thread_id)
        return &profiler_list[i];

      if (thread_list[i] == 0 && try_to_initialize)
      {
        init_profiler();
        return get_profiler(false);
      }

      if (thread_list[i] == 0)
        VSX_ERROR_EXIT("VSX PROFILER: Trying to get profiler without initializing it first. Reached end of profiler array.", 1);
    }
    return 0x0;
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

#undef VSX_PROFILER_STACK_DEPTH_WARNING


