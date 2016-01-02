#pragma once

#include <inttypes.h>

// 3 parts:

// 1. header
// 2. file list
//   which consists of:
//   header_file, filename (nullterminated), header_file, filename
// 3. data stream
//

namespace vsx
{
struct vsxz_header
{
   uint8_t identifier[4]; // "VSXZ"
   uint32_t version; // 1
   uint32_t directory_size; // size of the file directory in bytes
   uint32_t directory_count; // number of files stored in this archive
   uint32_t data_size; // size of the file directory in bytes
   uint32_t reserved[4];
} __attribute__((packed));

struct vsxz_header_file
{
  uint8_t compression_type; // 0 for none, 1 for lzma, 2 for lzham
  uint32_t file_name_size;
  uint32_t compressed_size;
  uint32_t uncompressed_size;
} __attribute__((packed));

}
