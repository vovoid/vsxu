#pragma once


#include <filesystem/archive/vsx_filesystem_archive_file_write.h>
#include <filesystem/archive/vsx_filesystem_archive_writer_base.h>

namespace vsx
{

class filesystem_archive_vsxz_writer
  : public filesystem_archive_writer_base
{
  const size_t work_chunk_size = 1024*1024 * 5;
  const size_t max_chunks = 9;

  bool do_compress = true;

  vsx_string<> archive_filename;
  vsx_nw_vector<filesystem_archive_file_write> archive_files;

  void archive_files_saturate_all();
  void file_add_all_worker(vsx_nw_vector<filesystem_archive_file_write*>* work_list);

public:

  void create(const char* filename);

  void disable_compression()
  {
    do_compress = false;
  }

  void add_file(vsx_string<> filename, vsx_string<> disk_filename, bool deferred_multithreaded);
  void add_string(vsx_string<> filename, vsx_string<> payload, bool deferred_multithreaded);
  void close();

  ~filesystem_archive_vsxz_writer()
  {}

};

}
