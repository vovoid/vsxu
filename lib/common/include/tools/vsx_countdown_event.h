#pragma once

#include <math/vsx_rand_singleton.h>
#include <tools/vsx_req.h>
#include <functional>

namespace vsx
{

template<size_t min_time_ms, size_t max_time_ms>
class countdown_event
{
  float value = 0.0f;

public:

  countdown_event()
  {
    value = (float)min_time_ms * 0.001f + (float)(max_time_ms - min_time_ms) * 0.001f * vsx_rand_singleton::get()->rand.frand();
  }

  void on_countdown_rand(float dt, std::function<void()> action)
  {
    value -= dt;
    req(value < 0.0f);
    value = (float)min_time_ms * 0.001f  + vsx_rand_singleton::get()->rand.frand() * (float)(max_time_ms - min_time_ms) * 0.001f;
    action();
  }

};

}
