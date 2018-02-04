#pragma once

#include <inttypes.h>

namespace vsx
{
template <size_t start, size_t max, bool wrap>
class count_up
{
  size_t value = start;

public:

  size_t get_and_increase()
  {
    size_t cur = value;
    value++;
    if (wrap)
      if (value > max)
        value = start;
    return cur;
  }

};
}
