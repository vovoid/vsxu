#include <stdio.h>
#include <stdint.h>
#include <vsx_compression_lzham.h>
#include <lib/compression/thirdparty/lzham-sdk/include/lzham.h>

namespace vsx
{

vsx_ma_vector<unsigned char> compression_lzham::compress(vsx_ma_vector<unsigned char> &uncompressed_data)
{
  vsx_ma_vector<unsigned char> compressed_data;
  size_t destLen = uncompressed_data.get_sizeof();
  compressed_data.allocate(destLen);

  lzham_compress_params params;
  params.m_struct_size = sizeof(lzham_compress_params);
  params.m_dict_size_log2 = LZHAM_MAX_DICT_SIZE_LOG2_X64;
  params.m_level = LZHAM_COMP_LEVEL_UBER;
  params.m_table_update_rate = 0;
  params.m_max_helper_threads = -1;
  params.m_compress_flags = LZHAM_COMP_FLAG_EXTREME_PARSING;
  params.m_num_seed_bytes = 0;
  params.m_pSeed_bytes = 0x0;

  lzham_uint32 adler_32;
  lzham_compress_memory(
    &params,
    compressed_data.get_pointer(),
    &destLen,
    uncompressed_data.get_pointer(),
    uncompressed_data.get_sizeof(),
    &adler_32
  );
  compressed_data.reset_used(destLen);
  compressed_data.trim();
  return compressed_data;
}

void compression_lzham::uncompress(
    vsx_ma_vector<unsigned char> &uncompressed_data,
    vsx_ma_vector<unsigned char> &compressed_data
)
{
  lzham_uint32 adler_32;
  lzham_decompress_params params;
  params.m_struct_size = sizeof(lzham_decompress_params);
  params.m_dict_size_log2 = LZHAM_MAX_DICT_SIZE_LOG2_X64;
  params.m_table_update_rate = 0;
  params.m_decompress_flags = 0;
  params.m_num_seed_bytes = 0;
  params.m_pSeed_bytes = 0x0;

  size_t destination_length;
  lzham_decompress_memory(
    &params,
    uncompressed_data.get_pointer(),
    &destination_length,
    compressed_data.get_pointer(),
    compressed_data.get_sizeof(),
    &adler_32
  );
}

vsx_ma_vector<unsigned char> compression_lzham::uncompress(vsx_ma_vector<unsigned char> &compressed_data, size_t original_size)
{
  vsx_ma_vector<unsigned char> uncompressed_data;
  uncompressed_data.allocate(original_size);
  uncompress(uncompressed_data, compressed_data);
  return uncompressed_data;
}



}
