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

#ifndef VSX_PROFILER_CONSUMER_H
#define VSX_PROFILER_CONSUMER_H

#include <list>

#include <filesystem/vsx_filesystem.h>
#include <container/vsx_nw_vector.h>
#include <math/vector/vsx_vector4.h>
#include <profiler/vsx_profiler_manager.h>
#include <filesystem/vsx_filesystem.h>
#include <filesystem/vsx_filesystem_helper.h>

const size_t compute_stack_depth = 128;

class vsx_profiler_consumer_chunk
{
public:
  double time_start;
  double time_end;
  size_t depth;
  uint64_t   flags;
  uint64_t   cycles_start;
  uint64_t   cycles_end;
  vsx_string<>tag;

  vsx_profiler_consumer_chunk()
    :
      time_start(0.0),
      time_end(0.0),
      depth(0),
      flags(0),
      cycles_start(0),
      cycles_end(0),
      tag("")
  {}
};

class vsx_profiler_consumer_plot
{
public:

  double time;
  size_t plot_type;
  vsx_vector4<double> v;

  vsx_profiler_consumer_plot()
    :
      time(0.0),
      plot_type (1)
  {}
};


class vsx_profiler_consumer
{
  vsx_nw_vector< vsx_string<> > filenames;


  vsx_nw_vector<vsx_profile_chunk> current_profile;

  vsx_nw_vector<u_int64_t> current_threads;
  vsx_nw_vector<u_int64_t> current_plots;

  vsx::filesystem filesystem;

  double current_max_time;
  double cycles_per_second;
  double one_div_cycles_per_second;

  double cycles_begin_time;

  uint64_t cpu_clock_start;
  uint64_t cpu_clock_end;


  vsx_profiler_consumer_chunk compute_stack[compute_stack_depth];
  size_t compute_stack_pointer;

public:

  vsx_profiler_consumer()
  {
    std::list< vsx_string<> > local_filenames;

    vsx::filesystem_helper::get_files_recursive(
      vsx_profiler_manager::profiler_directory_get(),
      &local_filenames
    );

    size_t i = 0;
    for (std::list< vsx_string<> >::iterator it = local_filenames.begin(); it != local_filenames.end(); it++)
    {
      vsx_string<>n = *it;
      vsx_string<>deli = DIRECTORY_SEPARATOR;
      vsx_nw_vector< vsx_string<> > parts;
      vsx_string_helper::explode(n, deli, parts);

      if (parts.size() < 2)
        continue;

      filenames[i] = parts[parts.size()-1];
      i++;
    }
  }

  vsx_string<>get_filenames_list()
  {
    vsx_string<>deli = "\n";
    return vsx_string_helper::implode(filenames, deli);
  }

  vsx_string<>get_items_list()
  {
    vsx_string<>res;
    for (size_t i = 0; i < current_threads.size(); i++)
    {
      res += "thread " + vsx_string_helper::i2s((int)current_threads[i]) + "\n";
    }
    for (size_t i = 0; i < current_plots.size(); i++)
    {
      res += "plot " + vsx_string_helper::i2s((int)current_plots[i]) + "\n";
    }
    return res;
  }

  void load_profile(size_t index)
  {
    current_max_time = 0.0;
    cycles_per_second = 0.0;
    one_div_cycles_per_second = 0.0;

    cycles_begin_time = 0.0;

    cpu_clock_start = 0;
    cpu_clock_end = 0;

    current_profile.reset_used();
    current_threads.reset_used();
    current_plots.reset_used();

    vsx_string<>filename = vsx_profiler_manager::profiler_directory_get() + DIRECTORY_SEPARATOR + filenames[index];
    vsx::file* fp = filesystem.f_open( filename.c_str());

    size_t file_size = filesystem.f_get_size(fp);

    // Some sanity checks
    if ( file_size % sizeof(vsx_profile_chunk) )
      VSX_ERROR_RETURN("Profile data file is not a multiple of chunk size.");

    size_t chunks_in_file = file_size / sizeof(vsx_profile_chunk);

    current_profile.allocate( chunks_in_file );

    filesystem.f_read( current_profile.get_pointer(), file_size, fp );
    vsx_printf(L"VSX PROFILER: loaded profile with %ld chunks\n", (long)chunks_in_file);

    if (current_profile.size() < 3)
      VSX_ERROR_RETURN("not enough data in profile");

    vsx_profile_chunk& first_profile = current_profile[0];
    vsx_profile_chunk& last_profile = current_profile[current_profile.size()-2];

    cpu_clock_start = first_profile.cycles;

    if (last_profile.flags != VSX_PROFILE_CHUNK_FLAG_TIMESTAMP)
      VSX_ERROR_RETURN("No timestamp in data file, can't analyze it.");

    current_max_time = vsx_string_helper::s2f( vsx_string<>( last_profile.tag, 32) );
    cpu_clock_end = last_profile.cycles;

    cycles_per_second = ((double)cpu_clock_end - (double)cpu_clock_start) / current_max_time;
    one_div_cycles_per_second = 1.0 / cycles_per_second;

    for (size_t i = 0; i < current_profile.size()-2; i++)
    {
      vsx_profile_chunk& chunk = current_profile[i];

      if (chunk.flags < 100) // regular
      {
        bool found = false;
        size_t j = 0;
        while (!found && j < current_threads.size())
        {
          if (current_threads[j] == chunk.id)
            found = true;
          j++;
        }

        if (!found)
          current_threads.push_back(chunk.id);
      }
      if (chunk.flags > 100) // plot
      {
        bool found = false;
        size_t j = 0;
        while (!found && j < current_plots.size())
        {
          if (current_plots[j] == chunk.id)
            found = true;
          j++;
        }

        if (!found)
          current_plots.push_back(chunk.id);
      }
    }

    for (size_t i = 0; i < current_threads.size(); i++)
    {
      vsx_printf(L"Current thread: %ld\n", (long)current_threads[i]);
    }
    for (size_t i = 0; i < current_plots.size(); i++)
    {
      vsx_printf(L"Current plot: %ld\n", (long)current_plots[i]);
    }


    vsx_printf(L"max time: %f\n", current_max_time);
    vsx_printf(L"clock frequency: %f\n", (double)(cpu_clock_end-cpu_clock_start) / current_max_time );
  }

  double cycles_to_time(uint64_t cycles)
  {
    return (double)(cycles - cpu_clock_start) * one_div_cycles_per_second;
  }

  /**
   * @brief get_chunks
   * @param t_start
   * @param t_end
   * @param chunks_result
   */
  void get_thread(double t_start, double t_end, uint64_t thread_id, vsx_nw_vector<vsx_profiler_consumer_chunk> &chunks_result)
  {
    VSX_UNUSED(t_start);
    VSX_UNUSED(t_end);

    if (current_profile.size() < 2)
      VSX_ERROR_RETURN("Not enough chunks in loaded profile data");

//    uint64_t cycles_begin_time = cpu_clock_start + (uint64_t)(t_start * cycles_per_second);
//    uint64_t cycles_end_time = cpu_clock_start + (uint64_t)(t_end * cycles_per_second);

    // now find the first major chunk
    for (size_t i = 0; i < current_profile.size(); i++)
    {
      vsx_profile_chunk& chunk = current_profile[i];

      if (chunk.id != thread_id)
      {
        vsx_printf(L"chunk id: %ld\n", chunk.id);
        continue;
      }

      if (chunk.flags == VSX_PROFILE_CHUNK_FLAG_SECTION_START /* && chunk.cycles > cycles_begin_time*/)
      {
        compute_stack[compute_stack_pointer].time_start = cycles_to_time( chunk.cycles );
        compute_stack[compute_stack_pointer].cycles_start = chunk.cycles;
//        vsx_printf(L"starting time: %f\n", compute_stack[compute_stack_pointer].time_start);
        compute_stack_pointer++;
        if (compute_stack_pointer == compute_stack_depth)
          compute_stack_pointer--;
      }

      if (chunk.flags == VSX_PROFILE_CHUNK_FLAG_SECTION_END /*&& chunk.cycles > cycles_begin_time*/)
      {
        if (compute_stack_pointer != 0)
          compute_stack_pointer--;

        compute_stack[compute_stack_pointer].time_end = cycles_to_time( chunk.cycles );
        compute_stack[compute_stack_pointer].depth = compute_stack_pointer;
        compute_stack[compute_stack_pointer].cycles_end = chunk.cycles;
        chunks_result.push_back( compute_stack[compute_stack_pointer] );
//        vsx_printf(L"ending time: %f\n", compute_stack[compute_stack_pointer].time_end);
      }

      if (chunk.flags == VSX_PROFILE_CHUNK_FLAG_START /*&& chunk.cycles > cycles_begin_time*/)
      {
        compute_stack[compute_stack_pointer].time_start = cycles_to_time( chunk.cycles );
        vsx_printf(L"tag: %hs\n", chunk.tag);
        compute_stack[compute_stack_pointer].tag = chunk.tag;
        compute_stack[compute_stack_pointer].cycles_start = chunk.cycles;
//        vsx_printf(L"starting time inner: %f\n", compute_stack[compute_stack_pointer].time_start);
        compute_stack_pointer++;
        if (compute_stack_pointer == compute_stack_depth)
          compute_stack_pointer--;
      }

      if (chunk.flags == VSX_PROFILE_CHUNK_FLAG_END /*&& chunk.cycles > cycles_begin_time*/)
      {
        if (compute_stack_pointer != 0)
          compute_stack_pointer--;

        compute_stack[compute_stack_pointer].time_end = cycles_to_time( chunk.cycles );
        compute_stack[compute_stack_pointer].depth = compute_stack_pointer;
        compute_stack[compute_stack_pointer].cycles_end = chunk.cycles;
        chunks_result.push_back( compute_stack[compute_stack_pointer] );
//        vsx_printf(L"ending time inner: %f\n", compute_stack[compute_stack_pointer].time_end);
      }
//      vsx_printf(L"stack pointer: %d\n", compute_stack_pointer);

    }

  }

  void get_plot(u_int64_t index, vsx_nw_vector<vsx_profiler_consumer_plot> &chunks_result)
  {
    for (size_t i = 0; i < current_profile.size(); i++)
    {
      vsx_profile_chunk& chunk = current_profile[i];

      if (chunk.flags < 100)
        continue;

      if (chunk.flags > 200)
        continue;

      vsx_profiler_consumer_plot plot;

      if (chunk.id == index)
      {
        plot.plot_type = chunk.flags - 100; // 101, 102, 103, 104
        plot.time = cycles_to_time( chunk.cycles );

        double a;
        memcpy(&a, &chunk.tag[0], sizeof(double));
        plot.v.x = a;
        if (plot.plot_type > 1)
        {
          memcpy(&a, &chunk.tag[8], sizeof(double));
          plot.v.y = a;
        }
        if (plot.plot_type > 2)
        {
          memcpy(&a, &chunk.tag[16], sizeof(double));
          plot.v.z = a;
        }
        if (plot.plot_type > 3)
        {
          memcpy(&a, &chunk.tag[24], sizeof(double));
          plot.v.w = a;
        }

        chunks_result.push_back( plot );
      }
    }

  }



  static vsx_profiler_consumer* get_instance()
  {
    static vsx_profiler_consumer vpc;
    return &vpc;
  }
};


#endif
