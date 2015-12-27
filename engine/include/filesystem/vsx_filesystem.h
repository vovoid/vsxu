#pragma once

#include <vsx_engine_dllimport.h>
#include <stdio.h>
#include <string/vsx_string.h>

#include "vsx_filesystem_archive_info.h"
#include "vsx_filesystem_handle.h"

#define VSXF_NUM_ADD_THREADS 8
#define VSXF_WORK_CHUNK_MAX_SIZE 1024*1024*5

#define VSXF_TYPE_FILESYSTEM 0
#define VSXF_TYPE_ARCHIVE 1


namespace vsx_filesystem
{
  class ENGINE_DLLIMPORT filesystem
  {
    // filesystem functions
    vsx_nw_vector<archive_info> archive_files;
    vsx_nw_vector<archive_info*> archive_files_p;
    int type; // 0 = regular filesystem, 1 = archive
    FILE* archive_handle;
    vsx_string<> archive_name;

    // base path for opening file system files
    vsx_string<> base_path;

    pthread_mutex_t mutex1;

    void aquire_lock()
    {
      pthread_mutex_lock( &mutex1 );
    }

    void release_lock()
    {
      pthread_mutex_unlock( &mutex1 );
    }

    vsx_nw_vector<archive_info*> add_work_pool[VSXF_NUM_ADD_THREADS];
    size_t add_work_pool_iterator;
    size_t work_chunk_current_size;
    void archive_close_handle_workers();

  public:

    filesystem();
    filesystem(const filesystem& f);
    void set_base_path(vsx_string<> new_base_path);
    vsx_string<> get_base_path();
    vsx_nw_vector<archive_info>* get_archive_files();
    int archive_load(const char* filename, bool preload_compressed_data = true);
    void archive_load_all_mt(const char* filename);
    void archive_create(const char* filename);
    void archive_close();
    int archive_add_file(vsx_string<> filename, char* data = 0, uint32_t data_size = 0, vsx_string<> disk_filename = "");
    int archive_add_file_mt(vsx_string<> filename);
    bool is_archive();
    bool is_archive_populated();

    static void* archive_add_file_worker(void* p);
    static void* archive_load_worker(void* p);

    bool          is_file(const char* filename);
    bool          is_file(const vsx_string<> filename);
    file_handle*  f_open(const char* filename, const char* mode);
    void          f_close(file_handle* &handle);
    int           f_puts(const char* buf, file_handle* handle);
    char*         f_gets(char* buf, unsigned long max_buf_size, file_handle* handle);
    wchar_t*      f_getws(wchar_t* buf, unsigned long max_buf_size, file_handle* handle);
    char*         f_gets_entire(file_handle* handle);
    int           f_read(void* buf, unsigned long num_bytes, file_handle* handle);
    unsigned long f_get_size(file_handle* handle);

    static filesystem* get_instance()
    {
      static filesystem f;
      return &f;
    }
  };

}

