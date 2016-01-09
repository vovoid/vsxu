#include <lib/compression/thirdparty/lzma-sdk/C/LzmaLib.h>
#include <stdio.h>
#include <stdint.h>
#include <vsx_compression_lzma.h>

namespace vsx
{

vsx_ma_vector<unsigned char> compression_lzma::compress(vsx_ma_vector<unsigned char> &uncompressed_data)
{
  vsx_ma_vector<unsigned char> compressed_data;
  size_t propsSize = LZMA_PROPS_SIZE;
  size_t destLen = uncompressed_data.get_sizeof() + uncompressed_data.get_sizeof() / 3 + 128;
  compressed_data.allocate(propsSize + destLen);

  LzmaCompress(
    &compressed_data[LZMA_PROPS_SIZE],
    &destLen,
    uncompressed_data.get_pointer(),
    uncompressed_data.get_sizeof(),
    compressed_data.get_pointer(),
    &propsSize, /* *outPropsSize must be = 5 */
    9,      /* 0 <= level <= 9, default = 5 */
    1 << 24,  /* default = (1 << 24) */
    3,        /* 0 <= lc <= 8, default = 3  */
    0,        /* 0 <= lp <= 4, default = 0  */
    2,        /* 0 <= pb <= 4, default = 2  */
    32,        /* 5 <= fb <= 273, default = 32 */
    1 /* 1 or 2, default = 2 */
  );
  compressed_data.reset_used(destLen + LZMA_PROPS_SIZE);
  return compressed_data;
}

void compression_lzma::uncompress(
    vsx_ma_vector<unsigned char> &uncompressed_data,
    vsx_ma_vector<unsigned char> &compressed_data
)
{
  size_t dest_len = uncompressed_data.size();
  size_t compressed_size = compressed_data.get_sizeof();
  LzmaUncompress(
    uncompressed_data.get_pointer(),
    &dest_len,
    compressed_data.get_pointer() + LZMA_PROPS_SIZE,
    &compressed_size,
    &compressed_data[0],
    LZMA_PROPS_SIZE
  );
}

vsx_ma_vector<unsigned char> compression_lzma::uncompress(vsx_ma_vector<unsigned char> &compressed_data, size_t original_size)
{
  vsx_ma_vector<unsigned char> uncompressed_data;
  uncompressed_data.allocate(original_size);
  uncompress(uncompressed_data, compressed_data);
  return uncompressed_data;
}



}
