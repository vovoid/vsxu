#pragma once

#include <math/vsx_rand_singleton.h>
#include <tools/vsx_req.h>
#include <functional>

namespace vsx
{

/**
 * Triggers function when timer has counted down.
 */
template<size_t min_time_ms, size_t max_time_ms>
class countdown_event_dt
{
  float value = 0.0f;

public:

  countdown_event_dt()
  {
    value = (float)min_time_ms * 0.001f + (float)(max_time_ms - min_time_ms) * 0.001f * vsx_rand_singleton::get()->rand.frand();
  }

  /**
   * @brief on_countdown_rand
   * @param dt  Delta time (per frame)
   * @param action
   */
  void on_countdown_rand(float dt, std::function<void()> action)
  {
    value -= dt;
    req(value < 0.0f);
    value = (float)min_time_ms * 0.001f  + vsx_rand_singleton::get()->rand.frand() * (float)(max_time_ms - min_time_ms) * 0.001f;
    action();
  }

  void on_countdown_min(float dt, std::function<void()> action)
  {
    value -= dt;
    req(value < 0.0f);
    value = (float)min_time_ms * 0.001f;
    action();
  }

  void set_to_max()
  {
    value = (float)max_time_ms * 0.001f;
  }

};

}
