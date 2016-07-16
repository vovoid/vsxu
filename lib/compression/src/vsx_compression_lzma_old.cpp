#include <lib/compression/thirdparty/lzma-old/Common/MyInitGuid.h>

#if defined(_WIN32)
#include <io.h>
#include <fcntl.h>
#define MY_SET_BINARY_MODE(file) setmode(fileno(file),O_BINARY)
#else
#define MY_SET_BINARY_MODE(file)
#endif

#include <lib/compression/thirdparty/lzma-old/7zip/Compress/LZMA/LZMADecoder.h>
#include <lib/compression/thirdparty/lzma-old/7zip/Compress/LZMA/LZMAEncoder.h>
#include <lib/compression/thirdparty/lzma-old/LzmaRam.h>
extern "C"
{
  #include <lib/compression/thirdparty/lzma-old/LzmaRamDecode.h>
}


#include <stdio.h>
#include <stdint.h>
#include <vsx_compression_lzma_old.h>
#include <string/vsx_string.h>

namespace vsx
{

vsx_ma_vector<unsigned char> compression_lzma_old::compress(vsx_ma_vector<unsigned char> &uncompressed_data)
{
  vsx_ma_vector<unsigned char> compressed_data;

  UInt32 dictionary = (UInt32)(uncompressed_data.size() << 1);
  size_t uncompressed_data_size = uncompressed_data.get_sizeof();
  size_t outSize = (size_t)uncompressed_data_size / 20 * 21 + (1 << 16);
  size_t outSizeProcessed;

  compressed_data.allocate(outSize);

  LzmaRamEncode(
    (Byte*)uncompressed_data.get_pointer(),
    uncompressed_data_size,
    compressed_data.get_pointer(),
    compressed_data.get_sizeof(),
    &outSizeProcessed,
    dictionary,
    SZ_FILTER_AUTO
  );

  compressed_data.reset_used(outSizeProcessed);
  return compressed_data;
}

vsx_ma_vector<unsigned char> compression_lzma_old::uncompress(vsx_ma_vector<unsigned char> &compressed_data)
{
  vsx_ma_vector<unsigned char> uncompressed_data;

  size_t outSize;
  if (LzmaRamGetUncompressedSize(compressed_data.get_pointer(), compressed_data.size(), &outSize) != 0)
    return uncompressed_data;

  reqrv(outSize, uncompressed_data);

  uncompressed_data.allocate(outSize - 1);

  size_t outSizeProcessed;
  LzmaRamDecompress(
    compressed_data.get_pointer(),
    compressed_data.get_sizeof(),
    uncompressed_data.get_pointer(),
    outSize,
    &outSizeProcessed,
    malloc,
    free
  );

  return uncompressed_data;
}



}
