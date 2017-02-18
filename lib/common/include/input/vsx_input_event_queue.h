#pragma once

#include <container/vsx_ma_vector.h>
#include "vsx_input_event.h"

class vsx_input_event_queue
{
  friend class vsx_input_event_queue_reader;

  vsx_ma_vector< vsx_input_event > queue;

  size_t write_pointer = 0;

public:

  void reset()
  {
    queue.reset_used();
    write_pointer = 0;
  }

  void add(const vsx_input_event& event)
  {
    queue[write_pointer] = event;
    write_pointer++;
  }

};
