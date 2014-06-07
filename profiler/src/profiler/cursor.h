#ifndef CURSOR_H
#define CURSOR_H

class cursor
{
public:
  vsx_vector3<> pos;

  inline static cursor* get_instance() __attribute__((always_inline))
  {
    static cursor c;
    return &c;
  }
};



#endif
