#pragma once

#include <vsx_engine_dllimport.h>
#include <stdio.h>
#include <string/vsx_string.h>

#include "vsx_filesystem_archive_info.h"
#include "vsx_filesystem_handle.h"
#include "vsx_filesystem_archive.h"


#define VSXF_TYPE_FILESYSTEM 0
#define VSXF_TYPE_ARCHIVE 1

namespace vsx_filesystem
{
  class ENGINE_DLLIMPORT filesystem
  {
    filesystem_archive archive;

    vsx_string<> base_path;

  public:

    void set_base_path(vsx_string<> new_base_path);
    vsx_string<> get_base_path();

    filesystem_archive& get_archive()
    {
      return archive;
    }

    bool is_file(const char* filename);
    bool is_file(const vsx_string<> filename);

    file_handle* f_open(const char* filename, const char* mode);
    void f_close(file_handle* &handle);

    char* f_gets(char* buf, unsigned long max_buf_size, file_handle* handle);
    wchar_t* f_getws(wchar_t* buf, unsigned long max_buf_size, file_handle* handle);
    char* f_gets_entire(file_handle* handle);
    int f_read(void* buf, unsigned long num_bytes, file_handle* handle);

    int f_puts(const char* buf, file_handle* handle);

    unsigned long f_get_size(file_handle* handle);

    static filesystem* get_instance()
    {
      static filesystem f;
      return &f;
    }
  };

}

