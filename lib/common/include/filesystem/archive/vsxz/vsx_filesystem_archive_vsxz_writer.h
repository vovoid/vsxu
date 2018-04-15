#pragma once


#include <filesystem/archive/vsx_filesystem_archive_file_write.h>
#include <filesystem/archive/vsx_filesystem_archive_writer_base.h>
#include <filesystem/archive/vsxz/vsx_filesystem_archive_chunk_write.h>

namespace vsx
{

#define VSX_FILESYSTEM_ARCHIVE_VSXZ_WRITER_MAX_CHUNKS 9
class filesystem_archive_vsxz_writer
  : public filesystem_archive_writer_base
{
  const size_t work_chunk_size = 1024*1024 * 5;
  const size_t max_chunks = VSX_FILESYSTEM_ARCHIVE_VSXZ_WRITER_MAX_CHUNKS;

  bool do_compress = true;
  bool do_calculate_ratios = true;

  vsx_string<> archive_filename;
  vsx_nw_vector<filesystem_archive_file_write> archive_files;

  filesystem_archive_chunk_write chunks[VSX_FILESYSTEM_ARCHIVE_VSXZ_WRITER_MAX_CHUNKS];

  vsx_ma_vector<float> compression_ratios;


  void archive_files_saturate_all();
  void calculate_ratios();
  void add_files_to_chunk_space_evenly();
  void file_add_all_worker(vsx_nw_vector<filesystem_archive_file_write*>* work_list);

public:

  void create(const char* filename);

  void disable_compression()
  {
    do_compress = false;
  }

  void set_no_ratio_calculation()
  {
    do_calculate_ratios = false;
  }

  void add_file(vsx_string<> filename, vsx_string<> disk_filename, bool deferred_multithreaded);
  void add_string(vsx_string<> filename, vsx_string<> payload, bool deferred_multithreaded);
  void close();

  ~filesystem_archive_vsxz_writer()
  {}

};

}
