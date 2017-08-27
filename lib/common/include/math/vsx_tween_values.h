#pragma once

#include <math/vsx_tween_ease.h>

namespace vsx
{

template <typename F = double>
class tween_values
{
public:

  /**
   * @brief linear tween
   * No easing, no acceleration.
   *
   * @param time A value between 0.0 and 1.0
   * @return
   */
  static inline F in_linear(F time, F start, F end)
  {
    return start + (end - start) * tween_ease<F>::in_linear(time);
  }

  /**
   * @brief in_sin
   * Sin ease in
   *
   * @param time A value between 0.0 and 1.0
   * @return
   */
  static inline F in_sin(F time, F start, F end)
  {
    return start + (end - start) * tween_ease<F>::in_sin(time);
  }

  /**
   * @brief out_sin
   * Sin ease in
   *
   * @param time A value between 0.0 and 1.0
   * @return
   */
  static inline F out_sin(F time, F start, F end)
  {
    return start + (end - start) * tween_ease<F>::out_sin(time);
  }

  /**
   * @brief in_out_sin
   * Sin ease in and out
   *
   * @param time A value between 0.0 and 1.0
   * @param start
   * @param end
   * @return
   */
  static inline F in_out_sin(F time, F start, F end)
  {
    return start + (end - start) * tween_ease<F>::in_out_sin(time);
  }

  /**
   * @brief in_out_sin_top_position
   * Sin ease in and out
   *
   * @param time A value between 0.0 and 1.0
   * @param start
   * @param end
   * @return
   */
  static inline F in_out_sin_top_position(F time, F start, F end, F top_position)
  {
    return start + (end - start) * tween_ease<F>::in_out_sin_top_position(time, top_position);
  }


  /**
   * @brief in_quad
   * Accelerating from zero velocity.
   *
   * @param time A value between 0.0 and 1.0
   * @param start
   * @param end
   * @return
   */

  static inline F in_quad(F time, F start, F end)
  {
    return start + (end - start) * tween_ease<F>::in_quad(time);
  }

  /**
   * @brief out_quad
   * Decelerating to zero velocity.
   *
   * @param time A value between 0.0 and 1.0
   * @param start
   * @param end
   * @return
   */
  static inline F out_quad(F time, F start, F end)
  {
      return start + (end - start) * tween_ease<F>::out_quad(time);
  }


  /**
   * @brief in_out_quad
   * Acceleration until halfway, then deceleration.
   *
   * @param time A value between 0.0 and 1.0
   * @param start
   * @param end
   * @return
   */
  static inline F in_out_quad(F time, F start, F end)
  {
    return start + (end - start) * tween_ease<F>::in_out_quad(time);
  }

  /**
   * @brief in_cubic
   * Accelerating from zero velocity.
   *
   * @param time A value between 0.0 and 1.0
   * @param start
   * @param end
   */
  static inline F in_cubic(F time, F start, F end)
  {
    return start + (end - start) * tween_ease<F>::in_cubic(time);
  }

  /**
   * @brief out_cubic
   * Decelerating to zero velocity.
   *
   * @param time A value between 0.0 and 1.0
   * @param start
   * @param end
   */
  static inline F out_cubic(F time, F start, F end)
  {
    return start + (end - start) * tween_ease<F>::out_cubic(time);
  }

  /**
   * @brief in_out_cubic
   * Acceleration until halfway, then deceleration.
   *
   * @param time A value between 0.0 and 1.0
   * @param start
   * @param end
   */
  static inline F in_out_cubic(F time, F start, F end)
  {
    return start + (end - start) * tween_ease<F>::in_out_cubic(time);
  }

  /**
   * @brief in_quart
   * Accelerating from zero velocity.
   *
   * @param time A value between 0.0 and 1.0
   * @param start
   * @param end
   */
  static inline F in_quart(F time, F start, F end)
  {
    return start + (end - start) * tween_ease<F>::in_quart(time);
  }

  /**
   * @brief out_quart
   * Decelerating to zero velocity.
   *
   * @param time A value between 0.0 and 1.0
   * @param start
   * @param end
   */
  static inline F out_quart(F time, F start, F end)
  {
    return start + (end - start) * tween_ease<F>::out_quart(time);
  }

  /**
   * @brief in_out_quart
   * Acceleration until halfway, then deceleration.
   *
   * @param time A value between 0.0 and 1.0
   * @param start
   * @param end
   */
  static inline F in_out_quart(F time, F start, F end)
  {
    return start + (end - start) * tween_ease<F>::in_out_quart(time);
  }

  /**
   * @brief in_quint
   * Accelerating from zero velocity.
   *
   * @param time A value between 0.0 and 1.0
   * @param start
   * @param end
   */
  static inline F in_quint(F time, F start, F end)
  {
    return start + (end - start) * tween_ease<F>::in_quint(time);
  }

  /**
   * @brief out_quint
   * Decelerating to zero velocity.
   *
   * @param time A value between 0.0 and 1.0
   * @param start
   * @param end
   */
  static inline F out_quint(F time, F start, F end)
  {
    return start + (end - start) * tween_ease<F>::out_quint(time);
  }

  /**
   * @brief in_out_quint
   * Acceleration until halfway, then deceleration.
   *
   * @param time A value between 0.0 and 1.0
   * @param start
   * @param end
   */
  static inline F in_out_quint(F time, F start, F end)
  {
    return start + (end - start) * tween_ease<F>::in_out_quint(time);
  }

};

}
