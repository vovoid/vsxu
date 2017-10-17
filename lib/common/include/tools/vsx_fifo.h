/**
* Project: VSXu Engine: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu Engine.
*
* @author Jonatan Wallmander, Vovoid Media Technologies AB Copyright (C) 2013
* @see The GNU Lesser General Public License (LGPL)
*
* VSXu Engine is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU Lesser General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#pragma once

#include <atomic>
#include <vsx_platform.h>

/**
 * Single threaded FIFO buffer for communication inside the same thread.
 */
template<
    typename T = int64_t,   // your data type
    int buffer_size = 4096  // MUST be: >=2 and power of two;
    >
class alignas(64) vsx_fifo
{
private:
  // this is the only variable shared between the threads
  size_t live_count;

  // this variable is owned by the producer thread; keep it on its own cache page
  size_t write_pointer;

  // this variable is owned by consumer thread; keep it on its own cache page
  size_t read_pointer;

  // actual data storage
  T buffer[buffer_size];

public:

  vsx_fifo()
  {
    live_count = 0;
    write_pointer = 0;
    read_pointer = 0;
  }

  uint64_t live_count_get()
  {
    return live_count;
  }

  // producer - if there is room in the queue, write to it
  // returns:
  //   true - value written successfully
  //   false - no more room in the queue, try later
  inline bool produce(const T &value)
  {
    // if the buffer is full, we can't do anything
    if
    (
      buffer_size
      ==
      live_count
    )
      return false;

    // advance the cyclic write pointer
    write_pointer++;

    // write value
    buffer[write_pointer & (buffer_size-1) ] = value;

    // now make this data available to the consumer
    live_count++;
    return true;
  }


  // consumer - if a value is available, reads it
  // returns:
  //    true  - value fetched successfully
  //    false - queue is empty, try later
  inline bool consume(T &result)
  {
    // is there something to read?
    // if not, return false
    if
    (
      0
      ==
      live_count // if our cache is old, no harm done
    )
      return false;

    // advance the cyclic read pointer
    read_pointer++;

    // read value
    result = buffer[read_pointer & (buffer_size-1) ];

    // now we have read the value which means it can
    // be re-used, so decrease the live_count
    live_count--;

    return true;
  }

};
