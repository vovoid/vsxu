#pragma once

#include <vsx_common_dllimport.h>
#include <stdio.h>
#include <string/vsx_string.h>

#include "vsx_file.h"
#include "archive/vsx_filesystem_archive_reader.h"

#define VSXF_TYPE_FILESYSTEM 0
#define VSXF_TYPE_ARCHIVE 1

namespace vsx
{
  class COMMON_DLLIMPORT filesystem
  {
    filesystem_archive_reader archive;
    vsx_string<> base_path;

    bool archive_prohibited = false;
    static size_t num_open_files;

  public:

    void set_base_path(vsx_string<> new_base_path);
    vsx_string<> get_base_path();

    filesystem_archive_reader* get_archive()
    {
      if (archive_prohibited)
        return 0x0;

      return &archive;
    }

    bool is_file(const char* filename);
    bool is_file(const vsx_string<> filename);

    file* f_open(const char* filename);
    void f_close(file* &handle);

    char* f_gets(char* buf, unsigned long max_buf_size, file* handle);

    /**
     * @brief f_gets_entire
     * @param handle
     * @return 0-terminated sgring
     */
    char* f_gets_entire(file* handle);

    /**
     * @brief f_data_get
     * @param handle
     * @return pointer to raw data, try to use this when reading from an archive
     */
    unsigned char* f_data_get(file* handle);


    size_t f_read(void* buf, size_t num_bytes, file* handle);

    size_t f_get_size(file* handle);

    static filesystem* get_instance()
    {
      static filesystem f;
      f.archive_prohibited = true;
      return &f;
    }
  };
}

