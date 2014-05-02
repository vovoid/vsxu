#ifndef VSX_WIDGET_TIME_HOLDER_H
#define VSX_WIDGET_TIME_HOLDER_H

class vsx_widget_time_holder
{
  float start;
  float end;
  float cur;

public:

  inline float end_get()
  {
    return end;
  }

  inline void end_set(float n)
  {
    end = n;
  }

  inline void end_inc(float n)
  {
    end += n;
  }

  inline float start_get()
  {
    return start;
  }

  inline void start_set(float n)
  {
    start = n;
  }

  inline void start_inc(float n)
  {
    start += n;
  }

  inline float cur_get()
  {
    return cur;
  }

  inline void cur_set(float n)
  {
    cur = n;
  }

  inline float diff()
  {
    return end - start;
  }

  vsx_widget_time_holder()
    :
    start( -0.5f ),
    end( 5.0f ),
    cur( 0.0f )
  {}
};

#endif
