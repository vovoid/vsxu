#ifndef TIME_SCALE_H
#define TIME_SCALE_H

#include <vsx_platform.h>

class time_scale
{
public:
  double time_scale_x;
  double time_offset;

  double chunk_time_end;
  double one_div_chunk_time_end;

  double time_size_x;
  double one_div_time_size_x;

  time_scale()
    :
      time_scale_x (1.0),
      time_offset (0.0),
      chunk_time_end(0.0),
      one_div_chunk_time_end(1.0),
      time_size_x(0.0),
      one_div_time_size_x(1.0)
  {}

  double world_to_time_factor(double x)
  {
    return (x - time_offset) / time_size_x;
  }

  inline static time_scale* get_instance() VSX_ALWAYS_INLINE
  {
    static time_scale ts;
    return &ts;
  }
};

#endif
