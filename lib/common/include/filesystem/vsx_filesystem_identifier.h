#pragma once

#include <container/vsx_ma_vector.h>

namespace vsx
{
class filesystem_identifier
{
public:
  static bool is_text_file(vsx_ma_vector<unsigned char>& data)
  {
    size_t char_count = 0;
    size_t other_count = 0;

    char* data_p = (char*)data.get_pointer();

    foreach (data, i)
    {
      if (i > 50)
        break;

      if (
          (data_p[i] >= 'a' && data_p[i] <= 'z')
          ||
          (data_p[i] >= 'A' && data_p[i] <= 'Z')
          ||
          (data_p[i] >= '0' && data_p[i] <= '9')
        )
      {
        char_count++;
        continue;
      }
      other_count++;
    }

    if (char_count > other_count * 5)
      return true;
    return false;
  }
};
}
