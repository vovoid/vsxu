#include <filesystem/mmap/vsx_filesystem_mmap.h>
#include <string/vsx_string_helper.h>
#include <tools/vsx_foreach.h>
#include <vsx_compression_lzma_old.h>


#define LZMA_PROPS_SIZE 14
#define LZMA_SIZE_OFFSET 6


int main(int argc, char *argv[])
{
  VSX_UNUSED(argc);
  VSX_UNUSED(argv);

  srand((unsigned int)time(0x0));

  vsx_ma_vector<unsigned char> data;

  for_n(i, 0, 512)
    data[i] = (unsigned char)rand();

  vsx_ma_vector<unsigned char> compressed = vsx::compression_lzma_old::compress(data);

  vsx_ma_vector<unsigned char> uncompressed = vsx::compression_lzma_old::uncompress(compressed);

  foreach(data, i)
    if (data[i] != uncompressed[i])
      vsx_printf(L"data differs on index %zu\n", i);

  return 0;
}
