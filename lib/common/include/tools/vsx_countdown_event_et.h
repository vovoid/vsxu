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
class countdown_event_et
{
  float value = 0.0f;
  float et_prev = 0.0f;
public:

  countdown_event_et()
  {
    value = (float)min_time_ms * 0.001f + (float)(max_time_ms - min_time_ms) * 0.001f * vsx_rand_singleton::get()->rand.frand();
  }

  /**
   * @brief on_countdown_rand
   * @param et  Elapsed time (seconds) since program has started
   * @param action
   */
  void on_countdown_rand(float et, std::function<void()> action)
  {
//    if (et_prev < 0.0)
//    {
//      et_prev = et;
//      return;
//    }

    value -= (et - et_prev);
    et_prev = et;
    req(value < 0.0f);
    value = (float)min_time_ms * 0.001f  + vsx_rand_singleton::get()->rand.frand() * (float)(max_time_ms - min_time_ms) * 0.001f;
    action();
  }

};

}
