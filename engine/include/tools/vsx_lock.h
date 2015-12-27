#pragma once

class vsx_lock
{
  volatile __attribute__((aligned(64))) int64_t ticket_number = 0;
  volatile __attribute__((aligned(64))) int64_t turn = 0;

public:

  void aquire()
  {
    int64_t my_turn = __sync_fetch_and_add( &ticket_number, 1);
    while (turn != my_turn)
      continue;
  }

  void release()
  {
    __sync_fetch_and_add( &turn, 1);
  }
};
