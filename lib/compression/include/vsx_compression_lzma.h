#pragma once

#include <container/vsx_ma_vector.h>

namespace vsx
{

class compression_lzma
{
public:
  static vsx_ma_vector<unsigned char> compress( vsx_ma_vector<unsigned char> &uncompressed_data);
  static void uncompress( vsx_ma_vector<unsigned char> &uncompressed_data, vsx_ma_vector<unsigned char> &compressed_data);
  static vsx_ma_vector<unsigned char> uncompress( vsx_ma_vector<unsigned char> &compressed_data, size_t original_size);
};

}
