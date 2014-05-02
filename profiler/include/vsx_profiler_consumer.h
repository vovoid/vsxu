#include <list>

#include <vsxfst.h>
#include <vsx_avector.h>
#include <vsx_profiler.h>
#include <vsxfst.h>


class vsx_profiler_consumer
{
  vsx_avector<vsx_string> filenames;

  vsx_avector<vsx_profile_chunk> current_profile;
  vsxf filesystem;

  double current_max_time;
  uint64_t cpu_clock_start;
  uint64_t cpu_clock_end;

public:

  vsx_profiler_consumer()
  {
    std::list<vsx_string> local_filenames;

    get_files_recursive(
      vsx_profiler_manager::profiler_directory_get(),
      &local_filenames
    );

    size_t i = 0;
    for (std::list<vsx_string>::iterator it = local_filenames.begin(); it != local_filenames.end(); it++)
    {
      vsx_string n = *it;
      vsx_string deli = DIRECTORY_SEPARATOR;
      vsx_avector<vsx_string> parts;
      explode(n, deli, parts);

      if (parts.size() < 2)
        continue;

      filenames[i] = parts[parts.size()-1];
      i++;
    }
  }

  vsx_string get_filenames()
  {
    vsx_string deli = "\n";
    return implode(filenames, deli);
  }

  void load_profile(size_t index)
  {
    vsx_string filename = vsx_profiler_manager::profiler_directory_get() + DIRECTORY_SEPARATOR + filenames[index];
    vsxf_handle* fp = filesystem.f_open( filename.c_str() , "r" );

    unsigned long file_size = filesystem.f_get_size(fp);

    // Some sanity checks
    if ( file_size % sizeof(vsx_profile_chunk) )
      ERROR_RETURN("Profile data file is not a multiple of chunk size.");

    size_t chunks_in_file = file_size / sizeof(vsx_profile_chunk);

    current_profile.allocate( chunks_in_file );

    filesystem.f_read( current_profile.get_pointer(), file_size, fp );
    vsx_printf("VSX PROFILER: loaded profile with %ld chunks\n", chunks_in_file);

    if (current_profile.size() < 3)
      ERROR_RETURN("not enough data in profile");

    vsx_profile_chunk& first_profile = current_profile[0];
    vsx_profile_chunk& last_profile = current_profile[current_profile.size()-2];

    cpu_clock_start = first_profile.cycles;

    if (last_profile.flags != VSX_PROFILE_CHUNK_FLAG_TIMESTAMP)
      ERROR_RETURN("No timestamp in data file, can't analyze it.");

    current_max_time = s2f( vsx_string( last_profile.tag, 32) );
    cpu_clock_end = last_profile.cycles;

    vsx_printf("max time: %f\n", current_max_time);

    vsx_printf("clock frequency: %f\n", (double)(cpu_clock_end-cpu_clock_start) / current_max_time );

  }


  static vsx_profiler_consumer* get_instance()
  {
    static vsx_profiler_consumer vpc;
    return &vpc;
  }
};
