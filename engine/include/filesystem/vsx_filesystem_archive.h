#pragma once

#include <vsx_engine_dllimport.h>
#include <container/vsx_nw_vector.h>
#include <string/vsx_string.h>
#include <tools/vsx_lock.h>

#include <filesystem/vsx_filesystem_handle.h>
#include <filesystem/vsx_filesystem_archive.h>
#include <filesystem/vsx_filesystem_archive_info.h>

#define VSXF_NUM_ADD_THREADS 8
#define VSXF_WORK_CHUNK_MAX_SIZE 1024*1024*5

namespace vsx_filesystem
{
  class ENGINE_DLLIMPORT filesystem_archive
  {
    vsx_nw_vector<archive_info> archive_files;
    vsx_nw_vector<archive_info*> archive_files_p;
    FILE* archive_handle = 0x0;
    vsx_string<> archive_name;

    vsx_nw_vector<archive_info*> add_work_pool[VSXF_NUM_ADD_THREADS];
    size_t add_work_pool_iterator = 0;
    size_t work_chunk_current_size = 0;

    void archive_close_handle_workers();

    vsx_lock lock;

    void file_open_read(const char* filename, file_handle* &handle);
    void file_open_write(const char* filename, file_handle* &handle);

    static void* add_file_worker(void* p);
    static void* load_worker(void* p);

  public:

    void create(const char* filename);
    void close();

    bool is_archive();
    bool is_archive_populated();

    int load(const char* filename, bool preload_compressed_data = true);
    void load_all_mt(const char* filename);

    int add_file(vsx_string<> filename, char* data = 0, uint32_t data_size = 0, vsx_string<> disk_filename = "");
    int add_file_mt(vsx_string<> filename);

    vsx_nw_vector<archive_info>* get_files();

    bool file_exists(vsx_string<> filename);
    void file_open(const char* filename, const char* mode, file_handle* &handle);
    void file_close(file_handle* handle);
  };
}
