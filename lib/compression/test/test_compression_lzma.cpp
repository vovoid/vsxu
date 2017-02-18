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

  srand((unsigned int)time(0x0));

  vsx_ma_vector<unsigned char> data;

  data[0] = 'h';
  data[1] = 'e';
  data[2] = 'l';
  data[3] = 'l';
  data[4] = 'o';
  data[5] = ' ';
  data[6] = 'w';
  data[7] = 'o';
  data[8] = 'r';
  data[9] = 'l';
  data[10] = 'd';
  data[11] = ' ';
  data[12] = '1';

  vsx_ma_vector<unsigned char> compressed = vsx::compression_lzma::compress(data);

  vsx_ma_vector<unsigned char> uncompressed = vsx::compression_lzma::uncompress(compressed, 13);

  foreach(data, i)
    if (data[i] != uncompressed[i])
      vsx_printf(L"data differs on index %zu\n", i);

  return 0;
}
