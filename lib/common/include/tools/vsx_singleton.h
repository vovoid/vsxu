#pragma once

namespace vsx
{

template<class T>
class singleton
{
public:

  static inline __attribute__((always_inline)) T* get() {
    static T t;
    return &t;
  }
};

}
