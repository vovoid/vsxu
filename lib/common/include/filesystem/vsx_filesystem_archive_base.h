#pragma once

#include <string/vsx_string.h>
#include <filesystem/vsx_filesystem_archive_info.h>
#include <filesystem/vsx_file.h>
#include <container/vsx_nw_vector.h>

namespace vsx
{

  class filesystem_archive_base
  {
  public:
    virtual void create(const char* filename) = 0;
    virtual int load(const char* archive_filename, bool load_data_multithreaded) = 0;
    virtual void close() = 0;

    virtual bool is_archive() = 0;
    virtual bool is_archive_populated() = 0;
    virtual bool is_file(vsx_string<> filename) = 0;

    virtual vsx_nw_vector<filesystem_archive_info>* files_get() = 0;

    virtual void file_open(const char* filename, const char* mode, file* &handle) = 0;
    virtual void file_close(file* handle) = 0;

    virtual int file_add(vsx_string<> filename, vsx_string<> disk_filename, bool deferred_multithreaded) = 0;

    virtual ~filesystem_archive_base()
    {}
  };
}
