#include <filesystem/mmap/vsx_filesystem_mmap.h>
#include <string/vsx_string_helper.h>
#include <tools/vsx_foreach.h>
#include <vsx_compression_lzma.h>


#define LZMA_PROPS_SIZE 14
#define LZMA_SIZE_OFFSET 6


int main(int argc, char *argv[])
{
  VSX_UNUSED(argc);
  VSX_UNUSED(argv);

  srand(time(0x0));

  vsx_ma_vector<unsigned char> data;

  for_n(512,i)
    data[i] = (unsigned char)rand();

  vsx_ma_vector<unsigned char> compressed = vsx::compression_lzma::compress(data);

  vsx_ma_vector<unsigned char> uncompressed = vsx::compression_lzma::uncompress(compressed, 512);

  foreach(data, i)
    if (data[i] != uncompressed[i])
      vsx_printf(L"data differs on index %d\n", i);

  return 0;
}
