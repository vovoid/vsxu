#pragma once

#include <vsx_common_dllimport.h>
#include <container/vsx_nw_vector.h>
#include <string/vsx_string.h>
#include <tools/vsx_lock.h>

#include <filesystem/vsx_file.h>
#include <filesystem/archive/vsx_filesystem_archive_reader_base.h>

#define VSXF_NUM_ADD_THREADS 8
#define VSXF_WORK_CHUNK_MAX_SIZE 1024*1024*5

namespace vsx
{
  class COMMON_DLLIMPORT filesystem_archive_reader
  {
  public:
    enum archive_type_t
    {
      archive_none,
      archive_vsx,
      archive_vsxz
    };
  private:

    filesystem_archive_reader_base* archive = 0x0;
    archive_type_t archive_type = archive_none;

  public:

    bool load(const char* archive_filename, bool load_data_multithreaded);
    void file_open(const char* filename, file* &handle);
    void close();

    bool is_archive();
    bool is_archive_populated();
    bool is_file(vsx_string<> filename);

    void files_get(vsx_nw_vector<filesystem_archive_file_read>& files);
  };
}
