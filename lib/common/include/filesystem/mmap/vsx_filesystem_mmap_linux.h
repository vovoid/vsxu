#pragma once

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <vsx_platform.h>
#include <tools/vsx_req_error.h>
#include <filesystem/mmap/vsx_file_mmap.h>
#include <filesystem/vsx_filesystem_helper.h>

namespace vsx
{

class filesystem_mmap
{
public:

  static file_mmap* create(const char* filename)
  {
    reqrv(filename, 0x0);
    reqrv(strlen(filename), 0x0);

    int handle = open(filename, O_RDONLY);

    req_error_v(handle != -1, "Invalid file handle", 0x0);

    size_t file_size = filesystem_helper::file_get_size( filename );

    reqrv(file_size, 0x0);

    unsigned char* map_ptr =
      (unsigned char*)mmap(
        0,
        file_size,
        PROT_READ,
        MAP_SHARED,
        handle,
        0
      );

    if (map_ptr == MAP_FAILED)
      return 0x0;

    file_mmap* file_handle = new file_mmap();
    file_handle->filename = filename;
    file_handle->size = file_size;
    file_handle->data = map_ptr;
    return file_handle;
  }

  static void destroy(file_mmap* &file_handle)
  {
    req(file_handle);
    req(file_handle->data);
    req(file_handle->size);

    munmap(file_handle->data, file_handle->size);
    delete file_handle;
    file_handle = 0x0;
  }
};

}
