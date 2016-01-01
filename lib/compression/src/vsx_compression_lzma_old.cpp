#include <lib/compression/third-party/lzma-old/Common/MyInitGuid.h>

#if defined(_WIN32)
#include <io.h>
#include <fcntl.h>
#define MY_SET_BINARY_MODE(file) setmode(fileno(file),O_BINARY)
#else
#define MY_SET_BINARY_MODE(file)
#endif

#include <lib/compression/third-party/lzma-old/7zip/Compress/LZMA/LZMADecoder.h>
#include <lib/compression/third-party/lzma-old/7zip/Compress/LZMA/LZMAEncoder.h>
#include <lib/compression/third-party/lzma-old/LzmaRam.h>
extern "C"
{
  #include <lib/compression/third-party/lzma-old/LzmaRamDecode.h>
}


#include <stdio.h>
#include <stdint.h>
#include <vsx_compression_lzma_old.h>

namespace vsx
{

vsx_ma_vector<unsigned char> compression_lzma_old::compress(vsx_ma_vector<unsigned char> &uncompressed_data)
{
  vsx_ma_vector<unsigned char> compressed_data;

  UInt32 dictionary = 1 << 21;
  size_t outSize = (size_t)uncompressed_data.get_sizeof() / 20 * 21 + (1 << 16);
  size_t outSizeProcessed;

  compressed_data.allocate(outSize);

  LzmaRamEncode(
    (Byte*)uncompressed_data.get_pointer(),
    uncompressed_data.get_sizeof(),
    compressed_data.get_pointer(),
    compressed_data.get_sizeof(),
    &outSizeProcessed,
    dictionary,
    SZ_FILTER_AUTO
  );

  compressed_data.reset_used(outSizeProcessed);
  compressed_data.trim();
  return compressed_data;
}

vsx_ma_vector<unsigned char> compression_lzma_old::uncompress(vsx_ma_vector<unsigned char> &compressed_data)
{
  vsx_ma_vector<unsigned char> uncompressed_data;

  size_t outSize;
  if (LzmaRamGetUncompressedSize(compressed_data.get_pointer(), compressed_data.size(), &outSize) != 0)
    return uncompressed_data;

  uncompressed_data.allocate(outSize);

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
