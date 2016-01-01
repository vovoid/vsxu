#pragma once

#include <container/vsx_ma_vector.h>

namespace vsx
{

class compression_lzma_old
{
public:
  static vsx_ma_vector<unsigned char> compress( vsx_ma_vector<unsigned char> &uncompressed_data );
  static vsx_ma_vector<unsigned char> uncompress( vsx_ma_vector<unsigned char> &compressed_data );
};

}
