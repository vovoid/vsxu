#pragma once

#include <atomic>

class vsx_lock
{
  std::atomic_uint_fast64_t ticket_number;
  std::atomic_uint_fast64_t turn;

public:

  vsx_lock()
  {
    ticket_number = 0;
    turn = 0;
  }

  inline void aquire()
  {
    uint64_t my_turn =  ticket_number.fetch_add(1);
    while (turn != my_turn)
      continue;
  }

  inline void release()
  {
    turn.fetch_add(1);
  }

  inline const vsx_lock& operator=(const vsx_lock& other)
  {
    this->ticket_number = other.ticket_number.load();
    this->turn = other.turn.load();
    return *this;
  }

};
