#pragma once

#include <vsx_common_dllimport.h>
#include <container/vsx_nw_vector.h>
#include <string/vsx_string.h>
#include <tools/vsx_lock.h>

#include <filesystem/vsx_file.h>
#include <filesystem/archive/vsx_filesystem_archive_file_write.h>
#include <filesystem/archive/vsx_filesystem_archive_writer_base.h>

namespace vsx
{
  class COMMON_DLLIMPORT filesystem_archive_vsx_writer
    : public filesystem_archive_writer_base
  {
    const size_t work_chunk_size = 1024*1024 * 5;

    FILE* archive_handle = 0x0;
    vsx_string<> archive_filename;
    vsx_nw_vector<filesystem_archive_file_write> archive_files;

    void file_add_all_worker(vsx_nw_vector<filesystem_archive_file_write*>* work_list);
    void file_add_all();
    void archive_files_saturate_all();
    void file_compress_and_add_to_archive(vsx_string<> filename, vsx_ma_vector<unsigned char>& uncompressed_data);

  public:

    void create(const char* filename);
    void add_file(vsx_string<> filename, vsx_string<> disk_filename, bool deferred_multithreaded);
    void add_string(vsx_string<> filename, vsx_string<> payload, bool deferred_multithreaded);
    void close();

    ~filesystem_archive_vsx_writer()
    {}
  };
}
