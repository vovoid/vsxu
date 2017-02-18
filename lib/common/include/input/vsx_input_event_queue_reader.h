#pragma once

#include "vsx_input_event_queue.h"

// This should be created on the stack, not the heap.
class vsx_input_event_queue_reader
{
  size_t read_pointer = 0;
  vsx_ma_vector< vsx_input_event >* queue;

public:

  vsx_input_event* consume()
  {
    if (read_pointer == queue->size() )
      return 0x0;

    return &(*queue)[read_pointer++];

    return 0x0;
  }

  vsx_input_event* consume(vsx_input_event::event_type_list type)
  {
    while (1)
    {
      vsx_input_event* event = consume();

      if (!event)
        return 0x0;

      if (event->type == type)
        return event;
    }
    return 0x0;
  }

  vsx_input_event_queue_reader(vsx_input_event_queue* queue_n)
  {
    queue = &queue_n->queue;
  }

  // to disallow creating this class on the heap, i.e. with new
  void* operator new(size_t) throw()
  {
    return 0x0;
  }

  void* operator new(size_t, void*) throw()
  {
    return 0x0;
  }

  void* operator new[](size_t) throw()
  {
    return 0x0;
  }

  void* operator new[](size_t, void*) throw()
  {
    return 0x0;
  }

};

#define vsx_input_event_queue_reader_consume(source_queue, target) \
  vsx_input_event_queue_reader reader(source_queue); \
  while (1) \
  { \
    vsx_input_event* event = reader.consume(); \
    if (!event) \
      break; \
    target.consume(*event); \
  }
