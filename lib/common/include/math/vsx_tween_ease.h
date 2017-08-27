#pragma once

namespace vsx
{

template <typename F = double>
class tween_ease
{
public:

  /**
   * @brief linear tween
   * No easing, no acceleration.
   *
   * @param time A value between 0.0 and 1.0
   * @return
   */
  static inline F in_linear(F time)
  {
    return time;
  }


  /**
   * @brief in_sin
   * Sin ease in from 0..1
   *
   * @param time A value between 0.0 and 1.0
   * @return
   */
  static inline F in_sin(F time)
  {
    return (F)0.5 * (F)sin(time * (F)PI - (F)0.5 * (F)PI) + (F)0.5;
  }

  /**
   * @brief out_sin
   * Sin ease in from 1 to 0
   *
   * @param time A value between 0.0 and 1.0
   * @return
   */
  static inline F out_sin(F time)
  {
    return (F)0.5 * (F)sin(time * (F)PI + (F)0.5 * (F)PI) + (F)0.5;
  }

  /**
   * @brief in_out_sin
   * Sin eas in & out
   *
   * @param time A value between 0.0 and 1.0
   * @return
   */
  static inline F in_out_sin(F time)
  {
    return time < (F)0.5 ?
      in_sin(time * (F)2.0)
      :
      out_sin((time - (F)0.5) * (F)2.0)
    ;
  }

  /**
   * @brief in_out_sin_top_position
   * Sin eas in & out
   *
   * @param time A value between 0.0 and 1.0
   * @param top_position where in time (between 0.0 and 1.0)
   *        the top occurs, normally this should be 0.5 for
   *        a centered curve
   * @return
   */
  static inline F in_out_sin_top_position(F time, F top_position)
  {
    return time < top_position ?
      in_sin(time * ((F)1.0 / top_position))
      :
      out_sin((time - top_position) * ((F)1.0 / ((F)1.0 - top_position)))
    ;
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

  static inline F in_quad(F time)
  {
    return time * time;
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
  static inline F out_quad(F time)
  {
      return time * ((F)2.0 - time);
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
  static inline F in_out_quad(F time)
  {
    return
      (
        time < (F)0.5 ?
            (F)2.0 * time * time
          :
            (F)-1.0 + ((F)4.0 - (F)2.0 * time) * time
      )
    ;
  }

  /**
   * @brief in_cubic
   * Accelerating from zero velocity.
   *
   * @param time A value between 0.0 and 1.0
   * @param start
   * @param end
   */
  static inline F in_cubic(F time)
  {
    return time * time * time;
  }

  /**
   * @brief out_cubic
   * Decelerating to zero velocity.
   *
   * @param time A value between 0.0 and 1.0
   * @param start
   * @param end
   */
  static inline F out_cubic(F time)
  {
    return ( (--time) * time * time + (F)1.0);
  }

  /**
   * @brief in_out_cubic
   * Acceleration until halfway, then deceleration.
   *
   * @param time A value between 0.0 and 1.0
   * @param start
   * @param end
   */
  static inline F in_out_cubic(F time)
  {
    return
      (
        time < (F)0.5 ?
          (F)4.0 * time * time * time
        :
          (time - (F)1.0) * ((F)2.0 * time - (F)2.0) * ((F)2.0 * time - (F)2.0) + (F)1.0
      )
    ;
  }

  /**
   * @brief in_quart
   * Accelerating from zero velocity.
   *
   * @param time A value between 0.0 and 1.0
   * @param start
   * @param end
   */
  static inline F in_quart(F time)
  {
    return time * time * time * time;
  }

  /**
   * @brief out_quart
   * Decelerating to zero velocity.
   *
   * @param time A value between 0.0 and 1.0
   * @param start
   * @param end
   */
  static inline F out_quart(F time)
  {
    return ((F)1.0 - (--time) * time * time * time);
  }

  /**
   * @brief in_out_quart
   * Acceleration until halfway, then deceleration.
   *
   * @param time A value between 0.0 and 1.0
   * @param start
   * @param end
   */
  static inline F in_out_quart(F time)
  {
    return
      time < (F)0.5
        ?
          (F)8.0 * time * time * time * time
        :
          (F)1.0 - (F)8.0 * (--time) * time * time * time;
  }

  /**
   * @brief in_quint
   * Accelerating from zero velocity.
   *
   * @param time A value between 0.0 and 1.0
   * @param start
   * @param end
   */
  static inline F in_quint(F time)
  {
    return
        time * time * time * time * time;
  }

  /**
   * @brief out_quint
   * Decelerating to zero velocity.
   *
   * @param time A value between 0.0 and 1.0
   * @param start
   * @param end
   */
  static inline F out_quint(F time)
  {
    return
        ( (F)1.0 + (--time) * time * time * time * time );
  }

  /**
   * @brief in_out_quint
   * Acceleration until halfway, then deceleration.
   *
   * @param time A value between 0.0 and 1.0
   * @param start
   * @param end
   */
  static inline F in_out_quint(F time)
  {
    return
      (
          time < (F)0.5
          ?
            (F)16.0 * time * time * time * time * time
          :
            (F)1.0 + (F)16.0 * (--time) * time * time * time * time
      )
    ;
  }

};

}
