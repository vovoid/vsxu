#pragma once

#include <functional>

/*
 * Helper class which can observe a boolen and triggers the lambda
 * depending on method called.
 */


namespace vsx
{

template <bool initial_state = false>
class state
{
  bool current_state = initial_state;

public:

  void on_true(bool value, std::function<void()> action )
  {
    if (current_state == false && value == true)
      action();
    current_state = value;
  }

  void on_false(bool value, std::function<void()> action )
  {
    if (current_state == true && value == false)
      action();
    current_state = value;
  }

  void reset()
  {
    current_state = initial_state;
  }

};

}
