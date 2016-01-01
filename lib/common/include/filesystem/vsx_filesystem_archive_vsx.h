#pragma once

#include <vsx_common_dllimport.h>
#include <container/vsx_nw_vector.h>
#include <string/vsx_string.h>
#include <tools/vsx_lock.h>

#include <filesystem/vsx_file.h>
#include <filesystem/vsx_filesystem_archive.h>
#include <filesystem/vsx_filesystem_archive_info.h>
#include <filesystem/vsx_filesystem_archive_base.h>

namespace vsx
{
  class COMMON_DLLIMPORT filesystem_archive_vsx
      : public filesystem_archive_base
  {
    const size_t work_chunk_size = 1024*1024 * 5;

    FILE* archive_handle = 0x0;
    vsx_string<> archive_name;
    vsx_nw_vector<filesystem_archive_info> archive_files;
    vsx_lock lock;

    void file_open_read(const char* filename, file* &handle);
    void file_open_write(const char* filename, file* &handle);

    void file_add_all_worker(vsx_nw_vector<filesystem_archive_info*>* work_list);
    void file_add_all();

    void load_all_worker(vsx_nw_vector<filesystem_archive_info*>* work_list);
    void load_all();

  public:

    void create(const char* filename);
    int load(const char* archive_filename, bool load_data_multithreaded);
    void close();

    int file_add(vsx_string<> filename, vsx_string<> disk_filename, bool deferred_multithreaded);

    bool is_archive();
    bool is_archive_populated();
    bool is_file(vsx_string<> filename);

    vsx_nw_vector<filesystem_archive_info>* files_get();

    void file_open(const char* filename, const char* mode, file* &handle);
    void file_close(file* handle);
  };
}
