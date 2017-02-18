#pragma once

#include <inttypes.h>

/**
 * A VSXZ file consists of 5 parts:
 * 1. header
 * 2. file tree
 * 3. file info table
 *    [ vsxz_header_file_info ][ vsxz_header_file_info ]...
 * 4. compression chunk table
 *    [ vsxz_header_compression_chunk][ vsxz_header_compression_chunk]
 * 5. compressed data
 *
 */
namespace vsx
{
VSX_PACK_BEGIN
struct vsxz_header
{
   uint8_t identifier[4] = {'V','S', 'X', 'Z'}; // "VSXZ"
   uint32_t version = 1; // 1
   uint32_t tree_size = 0; // size of the file tree
   uint32_t file_count = 0; // number of files stored in this archive
   uint32_t chunk_count = 0; // number of compression chunks in this archive
   uint32_t compression_uncompressed_memory_size = 0; // the chunk of memory needed for uncompressing the entire archive
   uint32_t reserved[4] = {0xEFBEADDE, 0xEFBEADDE, 0xEFBEADDE, 0xEFBEADDE};
}
VSX_PACK_END

VSX_PACK_BEGIN
struct vsxz_header_chunk_info
{
  uint16_t compression_type = 0; // 0 for none, 1 for lzma, 2 for lzham
  uint32_t compressed_size = 0;
  uint32_t uncompressed_size = 0;
}
VSX_PACK_END

VSX_PACK_BEGIN
struct vsxz_header_file_info
{
  uint8_t chunk = 0; // chunk index, starts with 0
  uint32_t offset = 0; // offset within the chunk
  uint32_t size = 0; // uncompressed size
}
VSX_PACK_END

}
