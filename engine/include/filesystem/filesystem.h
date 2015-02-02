#ifndef VSX_FILESYSTEM_FILESYSTEM_H
#define VSX_FILESYSTEM_FILESYSTEM_H

namespace vsx_filesystem
{
    class ENGINE_DLLIMPORT filesystem {
    private:
      // filesystem functions
      vsx_nw_vector<vsxf_archive_info> archive_files;
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

    public:
      filesystem();
      filesystem(const filesystem& f);
      void set_base_path(vsx_string<> new_base_path);
      vsx_string<> get_base_path();
      vsx_nw_vector<vsxf_archive_info>* get_archive_files();
      int archive_load(const char* filename, bool preload_compressed_data = true);
      void archive_load_all_mt(const char* filename);
      void archive_create(const char* filename);
      void archive_close();
      int archive_add_file(vsx_string<> filename, char* data = 0, uint32_t data_size = 0, vsx_string<> disk_filename = "");
      bool is_archive();
      bool is_archive_populated();

      static void* worker(void* p);

      bool          is_file(const char* filename);
      bool          is_file(const vsx_string<> filename);
      vsxf_handle*  f_open(const char* filename, const char* mode);
      void          f_close(vsxf_handle* handle);
      int           f_puts(const char* buf, vsxf_handle* handle);
      char*         f_gets(char* buf, unsigned long max_buf_size, vsxf_handle* handle);
      wchar_t*      f_getws(wchar_t* buf, unsigned long max_buf_size, vsxf_handle* handle);
      char*         f_gets_entire(vsxf_handle* handle);
      int           f_read(void* buf, unsigned long num_bytes, vsxf_handle* handle);
      unsigned long f_get_size(vsxf_handle* handle);
    };

}

#endif