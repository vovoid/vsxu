#pragma once

#include <memory>

namespace vsx
{

template<class T>
class managed_singleton
{

  static inline std::unique_ptr<T>& get_storage() {
    static std::unique_ptr<T> r;
    return r;
  }

public:

  // only run from main thread
  static inline void create() {
    std::unique_ptr<T>& r = get_storage();
    r = std::unique_ptr<T>( new T() );
  }

  static inline __attribute__((always_inline)) T* get() {
    std::unique_ptr<T>& r = get_storage();
    return r.get();
  }

  static inline void destroy() {
    std::unique_ptr<T>& r = get_storage();
    r.reset(nullptr);
  }

};

}
