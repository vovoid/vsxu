#pragma once

#include <inttypes.h>

// 4 parts:

// 1. header
// 2. file name table
//    - null-terminated names of all files in the archive in order
//    - all characters are encoded in 4 bits
//    - padded up to the
// 3. file info table
//    - array of vsxz_header_file_info
// 4. data stream
//

namespace vsx
{
struct vsxz_header
{
   /* 4 */uint8_t identifier[4] = {'V','S', 'X', 'Z'}; // "VSXZ"
   /* 8 */uint32_t version = 1; // 1
   /* 12 */uint32_t file_count = 0; // number of files stored in this archive
   /* 16 */uint32_t file_name_table_size = 0; // size of the file name stream in bytes
   /* 20 */uint32_t file_info_table_size = 0; // size of the file info directory in bytes
   /* 24 */uint32_t data_size = 0; // size of the file directory in bytes
   /* 32 */uint32_t reserved[2] = {0, 0};
} __attribute__((packed));

struct vsxz_header_file_info
{
  uint8_t compression_type = 0; // 0 for none, 1 for lzma, 2 for lzham
  uint32_t compressed_size = 0;
  uint32_t uncompressed_size = 0;
} __attribute__((packed));

}
