#pragma once

#include <memory>

namespace vsx
{

template<class T>
class managed_singleton
{

  static std::unique_ptr<T>& get_storage() {
    static std::unique_ptr<T> r;
    return r;
  }

public:

  // only run from main thread
  static void create() {
    std::unique_ptr<T>& r = T::get_storage();
    r = std::unique_ptr<T>( new T() );
  }

  static T* get() {
    std::unique_ptr<T>& r = get_storage();
    return r.get();
  }

  static void destroy() {
    std::unique_ptr<T>& r = T::get_storage();
    r.reset(nullptr);
  }

};

}
