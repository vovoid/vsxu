#pragma once

#include <vsx_platform.h>

namespace vsx
{


/**
 * Inherit this class to get aligned new / delete operators.
 * Example for 64 bit system:
 *
 *   public vsx::aligned_new_delete<64>
 *
 */
template<int bit_alignment = PLATFORM_BITS>
class aligned_new_delete
{
public:
  void* operator new(size_t i)
  {
      return _mm_malloc(i,bit_alignment / 8);
  }

  void operator delete(void* p)
  {
      _mm_free(p);
  }

};

}
