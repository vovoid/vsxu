#ifndef VSX_INTEGRATOR_H
#define VSX_INTEGRATOR_H

template <class T = float, typename I = double>
class vsx_integrator
{
  T value;
  T target;
  I speed;

public:

  integrator(T v, I s)
    :
      value(v),
      target(v),
      speed(s)
  {}

  /**
   * @brief update
   * @param delta_time seconds
   */
  void update(float delta_time)
  {
    I tt = delta_time * speed;

    vsx_printf(L"tt: %f\n", tt);

    if (tt > 1.0)
      tt = 1.0;

    T temp = value * (1.0 - tt) + target * tt;

    value = temp;
  }

  /**
   * @brief set_speed normalization
   * @param new_speed speed
   */
  void set_speed(I new_speed)
  {
    speed = new_speed;
  }

  /**
   * @brief set_target
   * @param new_target
   */
  void set_target(T new_target)
  {
    vsx_printf(L"setting target\n");
    target = new_target;
  }

  /**
   * @brief get_value
   * @return T
   */
  T get_value()
  {
    return value;
  }

};


#endif
