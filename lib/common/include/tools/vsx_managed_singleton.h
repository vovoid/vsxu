#pragma once

#include <memory>
#include <vsx_platform.h>

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

  static inline VSX_ALWAYS_INLINE T* get() {
    std::unique_ptr<T>& r = get_storage();
    return r.get();
  }

  static inline void destroy() {
    std::unique_ptr<T>& r = get_storage();
    r.reset(nullptr);
  }

};

}
