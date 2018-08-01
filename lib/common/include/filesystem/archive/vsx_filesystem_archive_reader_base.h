#pragma once

#include <string/vsx_string.h>
#include <filesystem/vsx_file.h>
#include <container/vsx_nw_vector.h>
#include "vsx_filesystem_archive_file_read.h"

#define VSX_FILESYSTEM_ARCHIVE_LOADING_FLAG_DO_NOT_COPY_MMAP_TO_RAM 2

namespace vsx
{

  class filesystem_archive_reader_base
  {
  public:
    virtual bool load(const char* archive_filename, bool load_data_multithreaded, uint64_t loading_flags) = 0;

    virtual void files_get(vsx_nw_vector<filesystem_archive_file_read>& files) = 0;
    virtual void file_open(const char* filename, file* &handle) = 0;

    virtual void close() = 0;

    virtual bool is_archive() = 0;
    virtual bool is_archive_populated() = 0;
    virtual bool is_file(vsx_string<> filename) = 0;

    virtual ~filesystem_archive_reader_base()
    {}
  };
}
