#pragma once

#include <vsx_engine_dllimport.h>
#include <container/vsx_nw_vector.h>
#include <string/vsx_string.h>
#include <tools/vsx_lock.h>

#include <filesystem/vsx_file.h>
#include <filesystem/vsx_filesystem_archive.h>
#include <filesystem/vsx_filesystem_archive_info.h>
#include <filesystem/vsx_filesystem_archive_base.h>

namespace vsx
{
  class ENGINE_DLLIMPORT filesystem_archive_vsxz
      : public filesystem_archive_base
  {
  public:

    void create(const char* filename);
    int load(const char* archive_filename, bool load_data_multithreaded);
    void close();

    int file_add(vsx_string<> filename, char* data, uint32_t data_size, vsx_string<> disk_filename, bool deferred_multithreaded);

    bool is_archive();
    bool is_archive_populated();
    bool is_file(vsx_string<> filename);

    vsx_nw_vector<filesystem_archive_info>* files_get();

    void file_open(const char* filename, const char* mode, file* &handle);
    void file_close(file* handle);
  };
}
