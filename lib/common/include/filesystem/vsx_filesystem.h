#pragma once

#include <vsx_common_dllimport.h>
#include <stdio.h>
#include <map>
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
    vsx_string<> prefix_remove;

    bool archive_prohibited = false;
    static size_t num_open_files;

    std::map< vsx_string<>, bool> open_files;

    vsx_string<> remove_prefix(vsx_string<> i_filename)
    {
      if (prefix_remove.size())
      {
        int res = i_filename.find(prefix_remove);
        if (res != -1)
        {
          i_filename = i_filename.substr((int)prefix_remove.size());
        }
      }
      return i_filename;
    }

  public:

    void set_base_path(vsx_string<> new_base_path);
    void set_prefix_remove(vsx_string<> prefix_remove)
    {
      this->prefix_remove = prefix_remove;
    }

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
    /**
     * @brief f_close_io Closes underlying operating system handle but preserving
     * memory read from file.
     * @param handle
     */
    void f_close_io(file* &handle);

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

