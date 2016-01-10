#pragma once

#include <inttypes.h>

// 4 parts:

// 1. header
// 2. file name table
//    [ vsxz_header_file_info ][ filename ][ vsxz_header_file_info ][ filename ]...
// 3. compressed data

namespace vsx
{
struct vsxz_header
{
   uint8_t identifier[4] = {'V','S', 'X', 'Z'}; // "VSXZ"
   uint32_t version = 1; // 1
   uint32_t file_info_table_size = 0; // size of the file name stream in bytes
   uint32_t file_count = 0; // number of files stored in this archive
   uint32_t file_count_compressed = 0; // number of compressed files stored in this archive
   uint32_t compressed_uncompressed_data_size = 0; // of the data which is compressed, how large is it when uncompressed
   uint32_t reserved[4] = {0xEFBEADDE, 0xEFBEADDE, 0xEFBEADDE, 0xEFBEADDE};
} __attribute__((packed));

struct vsxz_header_file_info
{
  uint16_t compression_type = 0; // 0 for none, 1 for lzma, 2 for lzham
  uint16_t filename_size;
  uint32_t compressed_size = 0;
  uint32_t uncompressed_size = 0;
} __attribute__((packed));

}
