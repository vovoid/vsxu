#pragma once

#include <vsx_common_dllimport.h>
#include <container/vsx_nw_vector.h>
#include <string/vsx_string.h>
#include <tools/vsx_lock.h>

#include <filesystem/vsx_file.h>
#include <filesystem/archive/vsx_filesystem_archive_file_read.h>
#include <filesystem/archive/vsx_filesystem_archive_reader_base.h>

namespace vsx
{
  class COMMON_DLLIMPORT filesystem_archive_vsx_reader
      : public filesystem_archive_reader_base
  {
    const size_t work_chunk_size = 1024*1024 * 5;

    FILE* archive_handle = 0x0;
    vsx_string<> archive_name;
    vsx_nw_vector<filesystem_archive_file_read> archive_files;
    vsx_lock lock;

    void load_all_worker(vsx_nw_vector<filesystem_archive_file_read*>* work_list);
    void load_all();

  public:

    bool load(const char* archive_filename, bool load_data_multithreaded);

    void files_get(vsx_nw_vector<filesystem_archive_file_read>& files);
    void file_open(const char* filename, file* &handle);

    void close();

    bool is_archive();
    bool is_archive_populated();
    bool is_file(vsx_string<> filename);

    ~filesystem_archive_vsx_reader()
    {}
  };
}
