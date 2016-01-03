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
   uint8_t identifier[4] = {'V','S', 'X', 'Z'}; // "VSXZ"
   uint32_t version = 1; // 1
   uint32_t file_count = 0; // number of files stored in this archive
   uint32_t file_table_size = 0; // size of the file directory in bytes
   uint32_t data_size = 0; // size of the file directory in bytes
   uint32_t reserved[4] = {0, 0, 0, 0};
} __attribute__((packed));

struct vsxz_header_file
{
  uint8_t compression_type = 0; // 0 for none, 1 for lzma, 2 for lzham
  uint32_t file_name_size = 0;
  uint32_t compressed_size = 0;
  uint32_t uncompressed_size = 0;
} __attribute__((packed));

}
