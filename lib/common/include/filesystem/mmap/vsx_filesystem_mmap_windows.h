#pragma once

#include <stdio.h>
#include <fcntl.h>
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

    size_t file_size = filesystem_helper::file_get_size(filename);
    reqrv(file_size, 0x0);

    void* handle =
      CreateFile(
        filename,
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0
      );
    req_error_v(handle != INVALID_HANDLE_VALUE, "Invalid file handle", 0x0);


    HANDLE map_handle =
      CreateFileMapping(
        handle,
        0,
        PAGE_READONLY,
        file_size >> 32,
        file_size & 0xFFFFFFFF,
        0
      );

    req_error_v(map_handle != INVALID_HANDLE_VALUE, "Invalid mapping handle", 0x0);

    unsigned char* map_ptr = (unsigned char*)
      (
        MapViewOfFile(
          map_handle,
          FILE_MAP_READ,
          0,
          0,
          file_size
        )
      );

    req_error_v(map_ptr, "Invalid map view of file", 0x0);

    file_mmap* file_handle = new file_mmap();
    file_handle->filename = filename;
    file_handle->size = file_size;
    file_handle->data = map_ptr;
    file_handle->handle = handle;
    file_handle->map_handle = map_handle;
    return file_handle;
  }

  static void destroy(file_mmap* &file_handle)
  {
    req(file_handle);
    req(file_handle->data);
    req(file_handle->size);

    UnmapViewOfFile(file_handle->data);
    CloseHandle(file_handle->map_handle);
    delete file_handle;
    file_handle = 0x0;
  }
};

}
