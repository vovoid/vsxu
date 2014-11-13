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
 *   VSX_PROFILER_BASE_PATH
 *     Default: undefined
 *
 *     By default, profiler uses VSXu engine's directory structure to place profiler files in
 *     the user's "vsxu/{version}/data/profiles/" directory. If you want this someplace else,
 *     set this define to your directory of choice.
 */

#ifndef VSX_PROFILER_MANAGER_H
#define VSX_PROFILER_MANAGER_H

#include <errno.h>
#include <vsx_profiler.h>
#include <vsx_data_path.h>
#include <vsx_timer.h>
#include <vsxfst.h>
#include <vsx_string_helper.h>
#include <debug/vsx_error.h>

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

  volatile __attribute__((aligned(64))) int64_t run_threads;
	volatile __attribute__((aligned(64))) int64_t enabled;

  pthread_mutex_t profiler_thread_lock;
  pthread_t consumer_pthread;
  pthread_t io_pthread;
  vsx_profiler profiler_list[VSX_PROFILER_MAX_THREADS];
  pid_t thread_list [VSX_PROFILER_MAX_THREADS];

  vsx_fifo<vsx_profile_chunk*,VSX_PROFILER_RECIEVE_BUFFER_PAGES> io_pool;


  vsx_profiler_manager()
		:
			run_threads(1),
			enabled(0)
  {
		vsx_printf("VSX PROFILER:  Initializing: ");
    pthread_mutex_init(&profiler_thread_lock, NULL);

    for (size_t i = 0; i < VSX_PROFILER_MAX_THREADS; i++)
    {
      thread_list[i] = 0;
    }

		vsx_printf("[consumer thread: ");

    pthread_create(
      &consumer_pthread,
      NULL,
      vsx_profiler_manager::consumer_thread,
      NULL
    );

		vsx_printf("[io thread: ");

    pthread_create(
      &io_pthread,
      NULL,
      vsx_profiler_manager::io_thread,
      NULL
    );

		vsx_printf("[initialization completed]\n");
  }

  ~vsx_profiler_manager()
  {
		vsx_printf("VSX PROFILER:  Shutting down:");

		if (run_threads)
			__sync_fetch_and_sub(&run_threads, 1);

		vsx_printf("[io thread] ");
		pthread_join(io_pthread, NULL);

		vsx_printf("[consumer thread] ");
		pthread_join(consumer_pthread, NULL);

		vsx_printf("[destruction complete]\n");
	}

  static vsx_string<>profiler_directory_get()
  {
		#ifdef VSX_PROFILER_BASE_PATH
			return vsx_string<>( VSX_PROFILER_BASE_PATH );
		#else
			return vsx_data_path::get_instance()->data_path_get()+"profiler";
		#endif
  }

	void enable()
	{
		if ( __sync_fetch_and_add(&enabled, 0) )
			return;

		vsx_printf("VSX PROFILER:  request consumer to data collection...\n");
		__sync_fetch_and_add(&enabled, 1);
	}

	void disable()
	{
		if ( !__sync_fetch_and_add(&enabled, 0) )
			return;

		vsx_printf("VSX PROFILER:  request consumer to disable data collection...\n");
		__sync_fetch_and_sub(&enabled, 1);
	}

  static void* io_thread(void* arg)
  {
    VSX_UNUSED(arg);
		vsx_printf("done] ");

    vsx_profiler_manager* pm = vsx_profiler_manager::get_instance();

    vsx_string<>profiler_directory = profiler_directory_get();

    if (access(profiler_directory.c_str(),0) != 0)
      mkdir( (profiler_directory).c_str(), 0700);

    vsx_string<>filename = profiler_directory + "/" + vsx_string<>(program_invocation_short_name) +
        "_" +vsx_string_helper::i2s(time(0x0)) + ".dat";

    vsx_timer timer;

    FILE* fp = fopen( filename.c_str() , "wb");

		if (!fp)
      VSX_ERROR_EXIT("VSX PROFILER: ***ERROR*** I/O thread can not open file. Aborting...", 900);

    timer.start();
    double accumulated_time = 0.0;
    while ( __sync_fetch_and_add( &pm->run_threads, 0) )
    {
      vsx_profile_chunk* r;
      while ( pm->io_pool.consume(r) )
      {
        fwrite(r,sizeof(vsx_profile_chunk) * VSX_PROFILER_RECIEVE_BUFFER_ITEMS,1,fp);
      }
      double d1 = timer.dtime();
      if (pm->enabled)
        accumulated_time += d1;
    }

    vsx_profile_chunk c;
    c.cycles = vsx_profiler_rdtsc();
    c.flags = VSX_PROFILE_CHUNK_FLAG_TIMESTAMP;
    memset(c.tag,0,32);
    sprintf(c.tag,"%f", accumulated_time );
    fwrite(&c,sizeof(vsx_profile_chunk),1,fp);

    fclose(fp);
    pthread_exit(0);
    return NULL;
  }

  static void* consumer_thread(void* profiler)
  {
    VSX_UNUSED(profiler);

		vsx_printf("done] ");

		vsx_profiler_manager* pm = vsx_profiler_manager::get_instance();

    vsx_profiler* profilers = &pm->profiler_list[0];
    pid_t* producer_threads = &pm->thread_list[0];

    vsx_profile_chunk recieve_buffer[VSX_PROFILER_RECIEVE_BUFFER_PAGES][VSX_PROFILER_RECIEVE_BUFFER_ITEMS];
    size_t recieve_buffer_iterator = 0;

    size_t current_buffer_page = 0;

    int64_t current_depth = 0;
    int64_t max_depth = 0;

		int64_t current_enabled = __sync_fetch_and_add(&pm->enabled, 0);

    while ( __sync_fetch_and_add( &pm->run_threads, 0) )
    {
      // collect from all threads
      for ( size_t i = 0; i < VSX_PROFILER_MAX_THREADS; i++)
      {
        if (producer_threads[i] == 0)
          break;

        vsx_profile_chunk recieve_chunk;

        // write the data to disk
        size_t max_iterations = 0;
        while (1)
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
							vsx_printf("VSX PROFILER:  Stack depth new maximum: %ld\n", max_depth);
            }
						if (current_depth > VSX_PROFILER_STACK_DEPTH_WARNING)
              vsx_printf("VSX PROFILER:  ***WARNING*** Stack depth: %d\n", VSX_PROFILER_STACK_DEPTH_WARNING);
					}

          if (
              recieve_chunk.flags == VSX_PROFILE_CHUNK_FLAG_SECTION_END
              ||
              recieve_chunk.flags == VSX_PROFILE_CHUNK_FLAG_END
              )
          {
						if ( !current_depth )
							vsx_printf("VSX PROFILER:  ***WARNING*** Got stack depth below zero. Fix your code!\n");
						current_depth--;
          }

					// handle enabling switch, only toggle when at zero depth
					if (
							current_enabled != __sync_fetch_and_add( &pm->enabled, 0)
							&&
							!current_depth
							)
					{
						current_enabled = __sync_fetch_and_add( &pm->enabled, 0);
						if (current_enabled)
            {
              vsx_printf("VSX PROFILER:  [enabled data collector]\n");
            }
						else
            {
							vsx_printf("VSX PROFILER:  [disabled data collector]\n");
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
								vsx_printf("VSX PROFILER:  ***PERFORMANCE WARNING*** spinning while i/o fifo is full...\n");
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
    pthread_exit(0);
    return 0x0;
  }


  static pid_t gettid( void )
  {
    return syscall( __NR_gettid );
  }

  // Call this once per thread
  // - make sure to call it from inside the thread
  //
  // It does lookups per thread to give you the correct pointer
  void init_profiler()
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
				vsx_printf("VSX PROFILER: ***WARNING*** Profiler already initialized for thread id %d. Did you call init() twice by mistake?\n",local_thread_id);
        return;
      }

      if (thread_list[i] == 0)
        break;
    }

    thread_list[i] = local_thread_id;
    profiler_list[i].set_thread_id( local_thread_id );

    pthread_mutex_unlock(&profiler_thread_lock);
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

  static vsx_profiler_manager* get_instance() __attribute__((always_inline))
  {
    static vsx_profiler_manager pm;
    return &pm;
  }

};

#undef VSX_PROFILER_STACK_DEPTH_WARNING

#endif
